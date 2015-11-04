// Change to your package name
package com.ccm.usbaudiodemo.usbcontrol;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;
import java.util.Vector;

import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;


/**
 *  
 * This class iterates over the connected USB devices known from the Java side and calls
 * into native code to actually try and open it using libusb.
 * 
 * Don't forget to call cleanUp() when you exit the app.
 * 
 * Note that this class uses a lot of reflection so it can be used in a project that uses Android versions prior to 3.1.
 *
 */
public class USBDeviceManager
{
    Context m_context;
    
    final static String s_logTag = "USBTester";
    
    // Reflection
    final static String s_UsbManagerClassName = "android.hardware.usb.UsbManager"; 
    final static String s_UsbDeviceClassName = "android.hardware.usb.UsbDevice";
    final static String s_UsbDeviceConnectionClassName = "android.hardware.usb.UsbDeviceConnection";
    final static String s_UsbDeviceDetached = "android.hardware.usb.action.USB_DEVICE_DETACHED";
    final static String s_actionUsbPermission = "com.esd.USB_PERMISSION";
    
    
    // The USB devices that are found to be useful and were initialized are registered here
    private Vector<Object> m_usbDevices = new Vector<Object>();
    private Vector<Object> m_connections = new Vector<Object>(); // UsbDeviceConnection
    
    // When a USB device is attached or detached, messages are sent to this handler on the UI thread
    private Handler m_handler = null;
    
    // Path to a file name where logs should be appended using appendLog() 
    private String m_logFileName;
    boolean m_logToFile = false;
    
    // Number of devices that are detected to be USB audio devices or potential USB audio devices
    private int m_devicesToQuery = 0;
    
    // Actually queried devices in the BroadcastReceiver
    private int m_queriedDevices = 0;
    
    // device classes according to the USB specification
    final static int s_deviceClassAudio = 1;
    final static int s_deviceClassMisc = 239;
    final static int s_deviceClassVendorSpecific = 255;
    
    private USBControl m_usbControl;
    
    
    // When the user accepts to give an app permission for a USB device, onReceive() will be called on this class
    private final BroadcastReceiver m_usbReceiver = new BroadcastReceiver()
    {
        public void onReceive(Context i_context, Intent i_intent)
        {
            try
            {
                String action = i_intent.getAction();
                Log.v(s_logTag, "action = " + action + ", this = " + this);
                if (s_actionUsbPermission.equals(action)) // the device got permission or was denied access
                {
                    synchronized (this)
                    {
                        Log.v(s_logTag, "Permission response");
                        
                        m_queriedDevices++;
                        Log.v(s_logTag, "m_queriedDevices = " + m_queriedDevices);
                        
                        // get which UsbDevice got permission (reflection)
                        Object device = (Object) i_intent.getParcelableExtra("device"); //UsbManager.EXTRA_DEVICE);
                    
                        // the device got permission to be accessed!
                        if (i_intent.getBooleanExtra("permission" /*UsbManager.EXTRA_PERMISSION_GRANTED*/, false))
                        {
                            if (device != null)
                            {
                                Log.v(s_logTag, "permission granted!");
                                Object manager; // UsbManager
                                manager = m_context.getSystemService("usb"); //Context.USB_SERVICE);
                                
                                // Open the USB device on the Java side to get a file descriptor that we can pass to libusb
                                Object connection = openDevice(manager, device);
                                
                                if (connection != null)
                                {
                                    int fd = getFileDescriptorFromConnection(connection); //.getFileDescriptor();
                                    initUSBDevice(device, connection, fd);
                                }
                                else
                                {
                                    appendLog("Failed to open USB device");
                                    if (m_handler != null)
                                    {
                                        Message msg = m_handler.obtainMessage();  
                                        Bundle b = new Bundle();
                                        b.putString("Command", "USBOpenFailed");
                                        msg.setData(b);  
                                        m_handler.sendMessage(msg);
                                    }
                                }
                            }
                        } 
                        else
                        {
                            Log.d(s_logTag, "permission denied for device " + device);
                        }
                    }
                }
                else if (action.equals(s_UsbDeviceDetached)) // UsbManager.ACTION_USB_DEVICE_DETACHED.equals(action))
                {
                    Object device = (Object) i_intent.getParcelableExtra("device"); //UsbManager.EXTRA_DEVICE);
                    handleDeviceDetached(device);
                }
            }
            catch (Exception e)
            {
                Log.e(s_logTag, "Exception in USB onReceive: " + e.getMessage());
            }
        }
    };
    
    
    /**
     * @param i_context Pass your main activity in here
     * @param i_handler Handler that will be sent messages of USB initialization or failure
     * @param i_logFileName Pass in a file name you wish this class to log to
     * @param i_logToFile When true, logs to file, otherwise to the debugger
     */
    public USBDeviceManager(Context i_context,
                            Handler i_handler,
                            String i_logFileName,
                            boolean i_logToFile,
                            USBControl i_usbControl)
    {
        m_context = i_context;
        m_handler = i_handler;
        m_logFileName = i_logFileName;
        m_logToFile = i_logToFile;
        m_usbControl = i_usbControl;
        
        // Register detachment of USB devices
        IntentFilter filter = new IntentFilter();
        filter.addAction(s_UsbDeviceDetached); //UsbManager.ACTION_USB_DEVICE_DETACHED);
        m_context.registerReceiver(m_usbReceiver, filter);
    }
    
    
    
    /**
     * Try to initialize the USB device using native libusb code. Only call after you got permission to use the device.
     * @param i_device UsbDevice (reflection)
     * @param i_connection UsbConnection (reflection)
     * @param i_fd File descriptor that we must pass to libusb to open the Linux file handle.
     */
    private void initUSBDevice(Object i_device, Object i_connection, int i_fd)
    {
        Log.v(s_logTag, "initUSBDevice");
        
        if (m_usbControl == null)
        {
            return;
        }
        
        if (m_usbControl.initUSBDevice(i_fd, getProductIdFromDevice(i_device), getVendorIdFromDevice(i_device)) == false)
        {
            Log.v(s_logTag, "Fail: product id = " + getProductIdFromDevice(i_device));
            
            if (m_queriedDevices == m_devicesToQuery)
            {
                Log.v(s_logTag, "m_queriedDevices == m_devicesToQuery");
                if (m_handler != null)
                {
                    Message msg = m_handler.obtainMessage();  
                    Bundle b = new Bundle();
                    
                    if (m_usbControl.getOpenDeviceErrorMessage().length() > 0)
                    {
                        b.putString("Command", "USBInitializeFailedWithError");
                        b.putString("ExtraErrorMsg", m_usbControl.getOpenDeviceErrorMessage());
                    }
                    else
                    {
                        b.putString("Command", "USBInitializeFailed");
                    }
                    
                    msg.setData(b);  
                    m_handler.sendMessage(msg);
                }
            }
            else
            {
                Log.v(s_logTag, "Silently skipping device");
                //appendLog("Silently skipping device!");
            }
        }
        else // success!
        {
            m_usbDevices.add(i_device);
            m_connections.add(i_connection);
            
            if (m_handler != null)
            {
                Message msg = m_handler.obtainMessage();  
                Bundle b = new Bundle();
                b.putString("Command", "USBInitialized");
                msg.setData(b);  
                m_handler.sendMessage(msg);
            }
        }
    }
    
    
    /**
     * Checks to see if the device was detached was one that we opened. If so, remove it from our connections and devices list and
     * send a message of detachment to the Handler.
     * 
     * @param i_device the UsbDevice that is detached (reflection)
     */
    private void handleDeviceDetached(Object i_device)
    {
        Log.v(s_logTag, "USB device DETACHED!");
        
        for (int i = 0; i < m_usbDevices.size(); i++)
        {
            if (i_device.equals(m_usbDevices.get(i)))
            {
                Object connection = m_connections.get(i);
                closeConnection(connection);
                m_connections.remove(connection);
                m_usbDevices.remove(i_device);
                
                Log.v(s_logTag, "m_connections = " + m_connections.size() + ", m_usbDevices = " + m_usbDevices.size());

                if (m_handler != null)
                {
                    Message msg = m_handler.obtainMessage();
                    Bundle b = new Bundle();
                    b.putString("Command", "USBDetached");
                    msg.setData(b);
                    m_handler.sendMessage(msg);
                }
            }
        }
    }
    
    
    /**
     * Logs the given text to a file when m_logToFile is true, otherwise it logs to the debugger.
     * @param i_text Text to log
     */
    private void appendLog(String i_text) 
    {
        if (m_logToFile)
        {
            if (m_logFileName != null && m_logFileName.length() > 0)
            {
                File logFile = new File(m_logFileName); 
                if (!logFile.exists()) 
                { 
                    try 
                    { 
                        logFile.createNewFile();
                    }  
                    catch (IOException e) 
                    { 
                        e.printStackTrace(); 
                    } 
                } 
                try 
                { 
                    //BufferedWriter for performance, true to set append to file flag 
                    BufferedWriter buf = new BufferedWriter(new FileWriter(logFile, true));  
                    buf.append(i_text); 
                    buf.append("\r\n"); 
                    buf.close(); 
                } 
                catch (IOException e) 
                { 
                    e.printStackTrace(); 
                } 
            }
        }
        else
        {
            Log.v(s_logTag, i_text);
        }
    }
    
    
    /**
     * After instantiating this class, call this method to find all detached USB devices that meet our specification
     * 
     * @return Number of devices that will be queried.
     */
    public int getUSBAudioDevices()
    {
        Class<?> usbManagerClass = getUsbManagerClass();
        if (usbManagerClass == null)
        {
            return 0;
        }

        Object manager;
        manager = m_context.getSystemService("usb"); //Context.USB_SERVICE);
        
        appendLog("getUSBAudioDevices");
        
        // Register our USB receiver class to receive a callback when a device got permission to be accessed
        PendingIntent mPermissionIntent = PendingIntent.getBroadcast(m_context, 0, new Intent(s_actionUsbPermission), 0);
        IntentFilter filter = new IntentFilter(s_actionUsbPermission);
        m_context.registerReceiver(m_usbReceiver, filter);
        
        HashMap<String, Object> devices = getDeviceList(manager);
        
        // count the number of devices to query first
        m_devicesToQuery = 0;
        Iterator< Map.Entry<String, Object> > it = devices.entrySet().iterator(); 
        while (it.hasNext())
        { 
            @SuppressWarnings("rawtypes")
            Map.Entry pairs = (Map.Entry) it.next(); 
            Object dev = (Object) pairs.getValue();
            
            if (dev != null)
            {
                int deviceClass = getDeviceClassFromDevice(dev);
                if ( ((deviceClass == s_deviceClassAudio)   ||
                      (deviceClass == 0)   ||
                      (deviceClass == s_deviceClassMisc) ||
                      (deviceClass == s_deviceClassVendorSpecific)) &&
                     getVendorIdFromDevice(dev) != 0x05C6) // inspect interface device class, misc = 239, 255 = vendor specific
                {
                    m_devicesToQuery++;
                }
            }
        }
        appendLog("Devices to query = " + m_devicesToQuery);
        
        it = devices.entrySet().iterator(); 
        while (it.hasNext())
        { 
            Map.Entry<String, Object> pairs = (Map.Entry<String, Object>) it.next();
            if (pairs != null)
            {
                Object dev = (Object) pairs.getValue();
                
                if (dev != null)
                {
                    int deviceClass = getDeviceClassFromDevice(dev);
                    appendLog("deviceClass = " + deviceClass + ", dev.getVendorId() = " + getVendorIdFromDevice(dev));
                    
                    // check if the device class is anything we could be interested in. Note that not all USB audio devices have
                    // their device class set to s_deviceClassAudio. That would have been too easy I guess!
                    if ( ((deviceClass == s_deviceClassAudio)   ||
                          (deviceClass == 0)   ||
                          (deviceClass == s_deviceClassMisc) ||
                          (deviceClass == s_deviceClassVendorSpecific)) &&
                         getVendorIdFromDevice(dev) != 0x05C6) // inspect interface device class, misc = 239, 255 = vendor specific
                    {
                        appendLog("requestPermission");
                        
                        Method requestPermissionMethod = null;
                        try
                        {
                            requestPermissionMethod = usbManagerClass.getMethod("requestPermission", getUsbDeviceClass(), PendingIntent.class);
                        }
                        catch (SecurityException e)
                        {
                            appendLog("SecurityException");
                            Log.e(s_logTag, "SecurityException!");
                            return m_devicesToQuery;
                        }
                        catch (NoSuchMethodException e)
                        {
                            appendLog("NoSuchMethodException");
                            Log.e(s_logTag, "NoSuchMethodException!");
                            return m_devicesToQuery;
                        }
                        
                        // request permission from the user to access the device (reflection)
                        try
                        {
                          //manager.requestPermission(dev, mPermissionIntent);
                            requestPermissionMethod.invoke(manager, dev, mPermissionIntent);
                        }
                        catch (IllegalArgumentException e)
                        {
                            appendLog("IllegalArgumentException");
                            Log.e(s_logTag, "IllegalArgumentException!");
                            return m_devicesToQuery;
                        }
                        catch (IllegalAccessException e)
                        {
                            appendLog("IllegalAccessException");
                            Log.e(s_logTag, "IllegalAccessException!");
                            return m_devicesToQuery;
                        }
                        catch (InvocationTargetException e)
                        {
                            appendLog("InvocationTargetException");
                            Log.e(s_logTag, "InvocationTargetException!");
                            return m_devicesToQuery;
                        }
                    }
                }
            }
        }
        
        return m_devicesToQuery;
    }


    public void cleanUp()
    {
        if (m_context != null && m_usbReceiver != null)
        {
            Log.v(s_logTag, "unregisterReceiver");
            m_context.unregisterReceiver(m_usbReceiver);
        }
        
        for (Object o: m_connections)
        {
            if (o != null)
            {
                closeConnection(o); //.close();
            }
        }
    }
    
    
    /**
     * The following methods are convenience methods for reflection.
     */
    private Class<?> getUsbManagerClass()
    {
        @SuppressWarnings("rawtypes")
        Class managerClass;
        
        try
        {
            managerClass = Class.forName(s_UsbManagerClassName);
            return managerClass;
        }
        catch (ClassNotFoundException e)
        {
            //Toast.makeText(m_context, "UsbManager not found: USB requires at least Android version 3.1!", Toast.LENGTH_LONG).show();
            return null;
        }
        catch (Exception e)
        {
            // Unknown exception
            return null;
        }
    }

    
    private Object openDevice(Object i_manager, Object i_device)
    {
        Class<?> managerClass = getUsbManagerClass();
        
        if (managerClass == null)
        {
            return null;
        }
        
        Object connection = null;
        
        Method openDeviceMethod = null;
        try
        {
            openDeviceMethod = managerClass.getMethod("openDevice", getUsbDeviceClass());
        }
        catch (SecurityException e)
        {
            appendLog("openDeviceMethod: SecurityException");
            Log.e(s_logTag, "openDeviceMethod: SecurityException!");
            return null;
        }
        catch (NoSuchMethodException e)
        {
            appendLog("openDeviceMethod: NoSuchMethodException");
            Log.e(s_logTag, "openDeviceMethod: NoSuchMethodException!");
            return null;
        }
        
        try
        {
            //m_connection = manager.openDevice(m_usbDevice);
            connection = openDeviceMethod.invoke(i_manager, i_device);
        }
        catch (IllegalArgumentException e)
        {
            appendLog("openDeviceMethod: IllegalArgumentException");
            Log.e(s_logTag, "openDeviceMethod: IllegalArgumentException!");
            return null;
        }
        catch (IllegalAccessException e)
        {
            appendLog("openDeviceMethod: IllegalAccessException");
            Log.e(s_logTag, "openDeviceMethod: IllegalAccessException!");
            return null;
        }
        catch (InvocationTargetException e)
        {
            appendLog("openDeviceMethod: InvocationTargetException");
            Log.e(s_logTag, "openDeviceMethod: InvocationTargetException!");
            return null;
        }
        
        return connection;
    }
    
    
    @SuppressWarnings("unchecked")
    HashMap<String, Object> getDeviceList(Object i_manager)
    {
        Class<?> managerClass = getUsbManagerClass();
        
        if (managerClass == null)
        {
            return null;
        }

        
        HashMap<String, Object> deviceIDs = new HashMap<String, Object>();
        
        Method getDeviceListMethod = null;
        try
        {
            getDeviceListMethod = managerClass.getMethod("getDeviceList");
        }
        catch (SecurityException e)
        {
            appendLog("getDeviceList: SecurityException");
            Log.e(s_logTag, "SecurityException!");
            return deviceIDs;
        }
        catch (NoSuchMethodException e)
        {
            appendLog("getDeviceList: NoSuchMethodException");
            Log.e(s_logTag, "NoSuchMethodException!");
            return deviceIDs;
        }
        
        
        try
        {
            deviceIDs = (HashMap<String, Object>) getDeviceListMethod.invoke(i_manager);
        }
        catch (IllegalArgumentException e)
        {
            appendLog("getDeviceList: IllegalArgumentException");
            Log.e(s_logTag, "IllegalArgumentException!");
            return deviceIDs;
        }
        catch (IllegalAccessException e)
        {
            appendLog("getDeviceList: IllegalAccessException");
            Log.e(s_logTag, "IllegalAccessException!");
            return deviceIDs;
        }
        catch (InvocationTargetException e)
        {
            appendLog("getDeviceList: InvocationTargetException");
            Log.e(s_logTag, "InvocationTargetException!");
            return deviceIDs;
        }
        //manager.getDeviceList();
        
        return deviceIDs;
    }
    
    
    private Class<?> getUsbDeviceClass()
    {
        @SuppressWarnings("rawtypes")
        Class usbDeviceClass;
        
        try
        {
            usbDeviceClass = Class.forName(s_UsbDeviceClassName);
            return usbDeviceClass;
        }
        catch (ClassNotFoundException e)
        {
            //Toast.makeText(m_context, "UsbDevice class not found: USB requires at least Android version 3.1!", Toast.LENGTH_LONG).show();
            return null;
        }
        catch (Exception e)
        {
            // Unknown exception
            return null;
        }
    }
    
    
    int getProductIdFromDevice(Object i_device)
    {
        Method getProductIdMethod = null;
        try
        {
            Class<?> usbDeviceClass = getUsbDeviceClass();
            
            if (usbDeviceClass != null)
            {
                getProductIdMethod = usbDeviceClass.getMethod("getProductId");
                
                if (getProductIdMethod != null)
                {
                    return (Integer) getProductIdMethod.invoke(i_device);
                }
            }
        }
        catch(Exception e)
        {
            return -1;
        }
        
        return -1;
    }
    
    
    int getVendorIdFromDevice(Object i_device)
    {
        Method getVendorIdMethod = null;
        try
        {
            Class<?> usbDeviceClass = getUsbDeviceClass();
            
            if (usbDeviceClass != null)
            {
                getVendorIdMethod = usbDeviceClass.getMethod("getVendorId");
                
                if (getVendorIdMethod != null)
                {
                    return (Integer) getVendorIdMethod.invoke(i_device);
                }
            }
        }
        catch(Exception e)
        {
            return -1;
        }
        
        return -1;
    }
    
    
    int getDeviceClassFromDevice(Object i_device)
    {
        Method getDeviceClassMethod = null;
        try
        {
            Class<?> usbDeviceClass = getUsbDeviceClass();
            
            if (usbDeviceClass != null)
            {
                getDeviceClassMethod = usbDeviceClass.getMethod("getDeviceClass");
                
                if (getDeviceClassMethod != null)
                {
                    return (Integer) getDeviceClassMethod.invoke(i_device);
                }
            }
        }
        catch(Exception e)
        {
            return -1;
        }
        
        return -1;
    }
    
    
    private Class<?> getUsbConnectionClass()
    {
        @SuppressWarnings("rawtypes")
        Class usbDeviceClass;
        
        try
        {
            usbDeviceClass = Class.forName(s_UsbDeviceConnectionClassName);
            return usbDeviceClass;
        }
        catch (ClassNotFoundException e)
        {
            //Toast.makeText(m_context, "UsbDeviceConnection class not found: USB requires at least Android version 3.1!", Toast.LENGTH_LONG).show();
            return null;
        }
        catch (Exception e)
        {
            // Unknown exception
            return null;
        }
    }
    
    
    int getFileDescriptorFromConnection(Object i_connection)
    {
        Method getFileDescriptorMethod = null;
        try
        {
            Class<?> usbDeviceClass = getUsbConnectionClass();
            
            if (usbDeviceClass != null)
            {
                getFileDescriptorMethod = usbDeviceClass.getMethod("getFileDescriptor");
                
                if (getFileDescriptorMethod != null)
                {
                    return (Integer) getFileDescriptorMethod.invoke(i_connection);
                }
            }
        }
        catch(Exception e)
        {
            return -1;
        }
        
        return -1;
    }
    
    
    void closeConnection(Object i_connection)
    {
        Method closeMethod = null;
        try
        {
            Class<?> usbDeviceClass = getUsbConnectionClass();
            
            if (usbDeviceClass != null)
            {
                closeMethod = usbDeviceClass.getMethod("close");
                
                if (closeMethod != null)
                {
                    closeMethod.invoke(i_connection);
                }
            }
        }
        catch(Exception e)
        {
        }
    }
}

