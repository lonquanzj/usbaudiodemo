package com.ccm.usbaudiodemo.activity;

import java.io.File;
import java.lang.ref.WeakReference;
import java.util.ArrayList;

import android.app.Activity;
import android.content.Intent;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.ImageButton;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

import com.ccm.usbaudiodemo.MusicApp;
import com.ccm.usbaudiodemo.R;
import com.ccm.usbaudiodemo.usbcontrol.USBControl;
import com.ccm.usbaudiodemo.usbcontrol.USBDeviceManager;


public class MainActivity extends Activity {
	
	public static final int UPDATE_LOG = 1;
	
    final static int s_defaultOpenSLESBufferSizeInFrames = 960;
    final static int s_defaultUSBBufferSizeInFrames = 4096;
    final static int s_defaultSampleRate = 48000;
	
    private USBControl m_usbControl;
    private USBDeviceManager m_usbDeviceManager;
    public boolean mIsConnectDev = false;
	
	private ListView mListView;
	private ArrayList<String> mS = new ArrayList<String>();
	private Button mBtn_connect;
	private Button mBtn_mixer;
	private ImageButton mBtn_rec;
	private ImageButton mBtn_play;
	private ImageButton mBtn_stop;
	private static TextView mtvLog;//log文本框
	private static TextView mTvTile;//标题文本
	private StringBuffer mLog = new StringBuffer();
	private myOnClickListener mClickListner;
	
	private boolean mIsRecStat = false;//判断是否是录音->停止
	
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        
        System.loadLibrary("esdusb");
        System.loadLibrary("usbtestnative");
        
        m_usbControl = new USBControl();
        
        initListView();
        initButton();
        
		// 设定默认的文件 确保每次进入都有默认的音乐文件
		if (mS.size() > 0) {
			setChooseFile(mS.get(0));
		}
    }


    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();
        if (id == R.id.action_settings) {
            return true;
        }
        return super.onOptionsItemSelected(item);
    }
    
    @Override
    protected void onDestroy() {
    	m_usbControl.cleanUp();
        if (m_usbDeviceManager != null)
        {
            m_usbDeviceManager.cleanUp();
        }
    	super.onDestroy();
    }
    
    private boolean connectClicked()
    {
        if (m_usbControl.initUSB())
        {
            String logFileName = MusicApp.logFileName;
            m_usbDeviceManager = new USBDeviceManager(getApplicationContext(), mHandler, logFileName, false, m_usbControl);
            if (m_usbDeviceManager.getUSBAudioDevices() > 0) {
                return true;
            }else{
                Toast.makeText(getApplicationContext(), "No USB devices found!", Toast.LENGTH_SHORT).show();
                return false;
            }
        }else{
            Toast.makeText(getApplicationContext(), "Error initialising USB!", Toast.LENGTH_SHORT).show();
        }
        return false;
    }
    
    private void play(boolean i_play,String playWavName, boolean i_record,String recWavName)
    {
//        int openSLESBufferSize = getOpenSLESBufferSize(s_defaultSampleRate);
        
        m_usbControl.startUSBTransfers(i_play, playWavName, i_record, recWavName, s_defaultSampleRate, false, s_defaultUSBBufferSizeInFrames, s_defaultOpenSLESBufferSizeInFrames);
    }
    
    
    private void stopClicked()
    {
        m_usbControl.stopUSBTransfers();
    }
    
    
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
    
    static class MyHandler extends Handler
    {
        private final WeakReference<MainActivity> m_activity;
        
        MyHandler(MainActivity i_activity)
        {
            m_activity = new WeakReference< MainActivity >(i_activity);
        }
        
        public void handleMessage(Message msg)  
        {  
            Bundle b = msg.getData();
            String command = b.getString("Command");
            
            switch (msg.what) {
			case UPDATE_LOG://更新log信息
				String data = b.getString("logInfo");
//				mLog.append(data + "\n");
				mtvLog.append(data + "\n");
				break;
			default:
				break;
            }
            
            if (command == null)
            {
                Log.v(MusicApp.TAG, "null message!");
                return;
            }
            
            Log.v(MusicApp.TAG, command);
            
            if (command.contentEquals("USBInitialized") && m_activity != null)
            {
                Toast.makeText(m_activity.get(), "USB audio interface initialized!", Toast.LENGTH_SHORT).show();
                
                m_activity.get().mIsConnectDev = true;
//                AudioDevice usbAudioDevice = Core.AddUSBAudioDeviceToAudioDeviceManager();
//                
//                if (usbAudioDevice != null)
//                {
//                    if (usbAudioDevice.getNumberOfInputModes() > 0)
//                    {
//                    }
//                    
//                    if (usbAudioDevice.getNumberOfOutputModes() > 0)
//                    {
//                    }
//                }
            }
            else if (command.contentEquals("USBDetached") && m_activity.get() != null)
            {
                Toast.makeText(m_activity.get(), "Disconnecting a USB audio device while the app is running is not recommended! You should restart the app.", Toast.LENGTH_SHORT).show();
            }
            else if (command.contentEquals("USBNoDevicesFound") && m_activity != null)
            {
                String extraMsg = b.getString("ExtraErrorMsg");
            }
            else if (command.contentEquals("USBInitializeFailed") && m_activity.get() != null)
            {
            }
            else if (command.contentEquals("USBInitializeFailedWithError") && m_activity.get() != null)
            {
            }
            else if (command.contentEquals("USBOpenFailed") && m_activity.get() != null)
            {
            }
        }  
    };
    
    public final MyHandler mHandler = new MyHandler(this);
    
    private void initButton(){
    	mClickListner = new myOnClickListener();
    	mBtn_connect = (Button) findViewById(R.id.btn_connectDev);
    	mBtn_connect.setOnClickListener(mClickListner);
    	mBtn_mixer = (Button) findViewById(R.id.btn_mixer);
    	mBtn_mixer.setOnClickListener(mClickListner);
    	mBtn_rec = (ImageButton) findViewById(R.id.btn_record);
    	mBtn_rec.setOnClickListener(mClickListner);
    	mBtn_play = (ImageButton) findViewById(R.id.btn_play);
    	mBtn_play.setOnClickListener(mClickListner);
    	mBtn_stop = (ImageButton) findViewById(R.id.btn_stop);
    	mBtn_stop.setOnClickListener(mClickListner);
    	mtvLog = (TextView) findViewById(R.id.tv_log);
		mTvTile = (TextView)findViewById(R.id.tv_curr_file);
    }
    
    class myOnClickListener implements android.view.View.OnClickListener{

		@Override
		public void onClick(View v) {
			switch (v.getId()) {
			case R.id.btn_connectDev:
				if (connectClicked()) {
					mBtn_connect.setEnabled(false);
					mBtn_connect.setText("已连接");
					mIsConnectDev = true;
				}
				break;
			case R.id.btn_mixer://混音
				Intent intent = new Intent(MainActivity.this, YinXiaoActivity.class);
				startActivity(intent);
				break;
			case R.id.btn_record://录音
				if(mIsConnectDev){
					play(false, null , true, MusicApp.recFileName);
					mBtn_play.setEnabled(false);
					mBtn_rec.setEnabled(false);
					mIsRecStat = true;
					sendMsgAppendLog("开始录音");
				}else{
					sendMsgAppendLog("设备未连接");
				}
				break;
			case R.id.btn_play://放音
				if(MusicApp.mCurrFileName == null){
					sendMsgAppendLog("文件为空");
					break;
				}
				if(mIsConnectDev){
					play(true, MusicApp.mCurrFileName, false, null);
					sendMsgAppendLog("开始播放");
					mBtn_rec.setEnabled(false);
					mBtn_play.setEnabled(false);
				}else{
					sendMsgAppendLog("设备未连接");
				}
				break;
			case R.id.btn_stop://停止
				if(mIsConnectDev){
					stopClicked();
					sendMsgAppendLog("停止");
					mBtn_play.setEnabled(true);;
					mBtn_rec.setEnabled(true);;
					if(mIsRecStat){//设置录音文件为默认打开的文件
						setChooseFile("record");
					}
					mIsRecStat = false;
				}else{
//					sendMsgAppendLog("设备未连接");
				}
				break;
			default:
				break;
			}
		}
    }
    
    
    private void initListView() {
		
		mListView = (ListView) findViewById(R.id.music_listview);
		getFileName();
		ArrayAdapter<String> adapter = new ArrayAdapter<String>(MainActivity.this, R.layout.listitem, mS);  
		mListView.setAdapter(adapter); 
		mListView.setOnItemClickListener(new OnItemClickListener() {
			@Override
			public void onItemClick(AdapterView<?> parent, View view,
					int position, long id) {
				//获取音乐标题
				String s = mS.get(position);
				setChooseFile(s);
			}
		});
	}
    
    private void setChooseFile(String filename){
		//在顶部设置当前选中的音乐标题
		mTvTile.setText(getResources().getString(R.string.curr_file) + filename);
		//设置当前选中音乐的完整路径
		MusicApp.mCurrFileName = MusicApp.musicPath + "/" + filename + ".wav";
    }
	
	private void getFileName() {//获取文件夹内的.wav文件名称
		File path = new File(MusicApp.musicPath);
		File[] files = path.listFiles();// 读取文件夹列表
		if (files != null) {// 先判断目录是否为空，否则会报空指针
			for (File file : files) {
				String fileName = file.getName();
				if (fileName.endsWith(".wav")) {
					String s = fileName.substring(0, fileName.lastIndexOf(".")).toString();
					mS.add(s);
				}
			}
		}
	}
	
	private void sendMsgAppendLog(String data){//发送更新log消息
		Message msg = new Message();
		msg.what = UPDATE_LOG;
		Bundle bundle = new Bundle();
		bundle.putString("logInfo", data);
		msg.setData(bundle);
		mHandler.sendMessage(msg);
	}
	
	public void sendEmptyMessageFromNative(int msgType){
		mHandler.sendEmptyMessage(msgType);
	}
}
