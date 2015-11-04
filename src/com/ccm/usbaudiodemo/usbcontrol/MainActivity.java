//package com.extreamsd.usbtester;
//
//import java.io.File;
//import java.lang.ref.WeakReference;
//
//import com.example.tianlaiui.YinXiaoActivity;
//import com.lu.demo.annotation.ContentView;
//import com.lu.demo.annotation.ViewInject;
//
//import android.app.Activity;
//import android.content.Intent;
//import android.os.Build;
//import android.os.Bundle;
//import android.os.Environment;
//import android.os.Handler;
//import android.os.Message;
//import android.util.Log;
//import android.view.View;
//import android.widget.Button;
//import android.widget.Toast;
//
////@ContentView(value = R.layout.activity_main)
//public class MainActivity extends Activity
//						  implements android.view.View.OnClickListener
//{
//    final static int s_defaultOpenSLESBufferSizeInFrames = 1024;
//    final static int s_defaultUSBBufferSizeInFrames = 4096;
//    final static int s_defaultSampleRate = 48000;
//    
//    private USBControl m_usbControl;
//    private USBDeviceManager m_usbDeviceManager;
//    
//    private Button m_connectButton;
//    private Button m_playButton;
//    private Button m_recButton;
//    private Button m_stopButton;
//    private Button m_yinxiaoButton;
//    
//
//    @Override
//    protected void onCreate(Bundle savedInstanceState)
//    {
//        super.onCreate(savedInstanceState);
//        setContentView(R.layout.activity_main);
//        
//        System.loadLibrary("esdusb");
//        System.loadLibrary("usbtestnative");
//        
//        m_usbControl = new USBControl();
//        
//        setupButtons();
//        
//        connectClicked();
//    }
//    
//    
//    @Override
//    protected void onDestroy()
//    {
//        m_usbControl.cleanUp();
//        if (m_usbDeviceManager != null)
//        {
//            m_usbDeviceManager.cleanUp();
//        }
//        
//        super.onDestroy();
//    }
//
//
//    void setupButtons()
//    {
//       m_connectButton = (Button) findViewById(R.id.connectButton);
//       m_connectButton.setOnClickListener(this);
//       m_playButton = (Button) findViewById(R.id.playButton);
//       m_playButton.setOnClickListener(this);
//       m_recButton = (Button) findViewById(R.id.playButton2);
//       m_recButton.setOnClickListener(this);
//       m_stopButton = (Button) findViewById(R.id.stopButton);
//       m_stopButton.setOnClickListener(this);
//       m_yinxiaoButton = (Button) findViewById(R.id.yinxiaoButton);
//       m_yinxiaoButton.setOnClickListener(this);
//    }
//    
//        
//    private void play(boolean i_play, boolean i_record)
//    {
//        int openSLESBufferSize = getOpenSLESBufferSize(s_defaultSampleRate);
//        
//        m_usbControl.startUSBTransfers(i_play, i_record, s_defaultSampleRate, false, s_defaultUSBBufferSizeInFrames, openSLESBufferSize);
//    }
//    
//    
//    private void stopClicked()
//    {
//        m_usbControl.stopUSBTransfers();
//    }
//    
//    
//    private int getOpenSLESBufferSize(int i_requestedSampleRate)
//    {
//        if (Build.VERSION.SDK_INT >= 17)
//        {
//            try
//            {
//                AudioSystemParams parms = AudioSystemParams.createInstance(this);
//                
//                Log.v("Main", "Native sample rate: " + Integer.toString(parms.getSampleRate()));
//                Log.v("Main", "Native buffer size: " + Integer.toString(parms.getBufferSize()));
//                
//                if (parms.getSampleRate() == i_requestedSampleRate)
//                {
//                    if (parms.getBufferSize() > 0)
//                    {
//                        return parms.getBufferSize();
//                    }
//                }
//            }
//            catch (Exception e)
//            {
//                e.printStackTrace();
//            }
//        }
//        
//        return s_defaultOpenSLESBufferSizeInFrames;
//    }
//
//
//    private boolean connectClicked()
//    {
//        if (m_usbControl.initUSB())
//        {
//            String logFileName = new String("/mnt/sdcard/USBTester.txt");
//            String storageState = Environment.getExternalStorageState();
//            
//            if (Environment.MEDIA_MOUNTED.equals(storageState))
//            {
//                File storageDir = Environment.getExternalStorageDirectory();
//
//                logFileName = new String(storageDir + "/USBTester.txt");
//            }
//            
//            m_usbDeviceManager = new USBDeviceManager(getApplicationContext(), m_handler, logFileName, false, m_usbControl);
//            if (m_usbDeviceManager.getUSBAudioDevices() > 0)
//            {
//                return true;
//            }
//            else
//            {
//                Toast.makeText(getApplicationContext(), "No USB devices found!", Toast.LENGTH_SHORT).show();
//                return false;
//            }
//        }
//        else
//        {
//            Toast.makeText(getApplicationContext(), "Error initialising USB!", Toast.LENGTH_SHORT).show();
//        }
//        
//        return false;
//    }
//
//
//    private void usbAudioReady()
//    {
//        Log.v("Main", "USB audio is ready!");
//        enableTransportControls();
//        
//        try
//        {
//        }
//        catch (Exception e)
//        {
//            e.printStackTrace();
//        }
//    }
//  
//    
//    private void enableTransportControls()
//    {
//        runOnUiThread(new Runnable()
//        {
//            public void run()
//            {
//                try
//                {
//                    if (m_playButton != null)
//                    {
//                        m_playButton.setEnabled(true);
//                    }
//                    
//                    if (m_recButton != null)
//                    {
//                    	m_recButton.setEnabled(true);
//                    }
//                    
//                    if (m_stopButton != null)
//                    {
//                        m_stopButton.setEnabled(true);
//                    }
//                }
//                catch(Exception e)
//                {
//                    Log.e("Main", "Exception: " + e.getMessage());
//                }
//            }
//        });
//        
//    }
//    
//    
//    static class MyHandler extends Handler
//    {
//        private final WeakReference<MainActivity> m_activity;
//        
//        MyHandler(MainActivity i_activity)
//        {
//            m_activity = new WeakReference< MainActivity >(i_activity);
//        }
//        
//        public void handleMessage(Message msg)  
//        {  
//            Bundle b = msg.getData();
//            String command = b.getString("Command");
//            
//            if (command == null)
//            {
//                Log.v("Main", "null message!");
//                return;
//            }
//            
//            Log.v("Main", command);
//            
//            if (command.contentEquals("USBInitialized") && m_activity != null)
//            {
//                Toast.makeText(m_activity.get(), "USB audio interface initialized!", Toast.LENGTH_SHORT).show();
//                
//                m_activity.get().enableTransportControls();
////                AudioDevice usbAudioDevice = Core.AddUSBAudioDeviceToAudioDeviceManager();
////                
////                if (usbAudioDevice != null)
////                {
////                    if (usbAudioDevice.getNumberOfInputModes() > 0)
////                    {
////                    }
////                    
////                    if (usbAudioDevice.getNumberOfOutputModes() > 0)
////                    {
////                    }
////                }
//            }
//            else if (command.contentEquals("USBDetached") && m_activity.get() != null)
//            {
//                Toast.makeText(m_activity.get(), "Disconnecting a USB audio device while the app is running is not recommended! You should restart the app.", Toast.LENGTH_SHORT).show();
//            }
//            else if (command.contentEquals("USBNoDevicesFound") && m_activity != null)
//            {
//                String extraMsg = b.getString("ExtraErrorMsg");
//            }
//            else if (command.contentEquals("USBInitializeFailed") && m_activity.get() != null)
//            {
//            }
//            else if (command.contentEquals("USBInitializeFailedWithError") && m_activity.get() != null)
//            {
//            }
//            else if (command.contentEquals("USBOpenFailed") && m_activity.get() != null)
//            {
//            }
//        }  
//    };
//    
//    public final MyHandler m_handler = new MyHandler(this);
//
//
//	@Override
//	public void onClick(View v) {
//		switch (v.getId()) {
//		case R.id.connectButton:
//			if (connectClicked()) {
//				m_connectButton.setEnabled(false);
//			}
//			break;
//		case R.id.playButton:
//			play(true, false);
//			m_playButton.setEnabled(false);
//			m_recButton.setEnabled(false);
//			break;
//		case R.id.playButton2:
//			play(true, true);
//			m_playButton.setEnabled(false);
//			m_recButton.setEnabled(false);
//			break;
//		case R.id.stopButton:
//			if (m_stopButton != null) {
//				stopClicked();
//				m_recButton.setEnabled(true);
//				m_playButton.setEnabled(true);
//				break;
//			}
//		default:
//		case R.id.yinxiaoButton:
//			Intent intent = new Intent(MainActivity.this, YinXiaoActivity.class);
//			startActivity(intent);
//			break;
//		}
//	}
//}
