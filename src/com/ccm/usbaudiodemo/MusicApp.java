package com.ccm.usbaudiodemo;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.nio.channels.FileChannel;

import android.app.Application;
import android.os.Environment;
import android.util.Log;

public class MusicApp extends Application {
	
	public static boolean mIsSleepClockSetting = false;
	private static String rootPath = "/ccmMusic";
	public static String lrcPath = "/lrc";
	public static String musicPath = "/musicFiles";
	public static String recPath = "/recFiles";
	public static String recFileName = null;
	public static String logFileName = null;
	public static String mCurrFileName = null;
	public static final String TAG = "MyUSBAdio";
	
	@Override
	public void onCreate() {
		super.onCreate();
		Log.d(TAG, "Application OnCreate");
		initPath();
//		setDefaultWavFile(R.raw.test_48k, "/test_48k.wav");
//		setDefaultWavFile(R.raw.test_44_1k, "/test_44_1k.wav");
		createRecordFile();
	}
	
	private void initPath() {//
		String ROOT = "";
		if(Environment.getExternalStorageState().equals(Environment.MEDIA_MOUNTED)) {
			ROOT = Environment.getExternalStorageDirectory().getPath();
		}
		Log.d(TAG, "根目录= " + ROOT);
		rootPath = ROOT + rootPath;
		lrcPath = rootPath + lrcPath;
		musicPath = rootPath + musicPath;
		recPath = rootPath + recPath;
		File lrcFile = new File(lrcPath);
		if(!lrcFile.exists()) {
			lrcFile.mkdirs();
		}
		File musicFile = new File(musicPath);
		if(!musicFile.exists()) {
			musicFile.mkdirs();
		}
		File recFile = new File(recPath);
		if(!recFile.exists()) {
			recFile.mkdirs();
		}
		
		logFileName = new String(rootPath + "/USBTester.txt");
		recFileName = new String(musicPath + "/record.wav");
	}
	
	/**
	 * 设置默认的音频文件
	 * @param rawID 音乐ID编号
	 * @param fileName wav文件名
	 */
    private void setDefaultWavFile(int rawID, String fileName){//在外部目录复制一个wav文件
    	String path = musicPath + fileName;
    	File file = new File(path); 
        if(file.exists()){
       	 return;
        }
    	InputStream inputStream = getResources().openRawResource(rawID);
    	 try {
			byte[] reader = new byte[inputStream.available()];
			 while (inputStream.read(reader) != -1) {
				 writefile(reader,path); 
			 }   
		} catch (IOException e) {
			// TODO Auto-generated catch block
			 Log.i(TAG, e.getMessage());   
		} 
    }
    
    private void writefile(byte[]str, String path){
    	File file; 
        FileOutputStream out; 
         try { 
             //创建文件  
             file = new File(path); 
             file.createNewFile(); 
             //打开文件file的OutputStream  
             out = new FileOutputStream(file); 
              
             //将字符串转换成byte数组写入文件  
             out.write(str); 
             //关闭文件file的OutputStream  
             out.close();  
         } catch (IOException e) { 
             //将出错信息打印到Logcat  
        	 Log.i(TAG, e.getMessage());  
         } 
    }
    
    private void createRecordFile(){//为了保证第一次进入时有record文件
    	String path = musicPath + "/record.wav";
    	File file = new File(path); 
        if(file.exists()){
       	 return;
        }else{
            try {
				file.createNewFile();
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			} 
        }
    }

}