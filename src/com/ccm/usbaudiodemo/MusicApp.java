package com.ccm.usbaudiodemo;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStream;

import android.app.Application;
import android.os.Environment;
import android.util.Log;

public class MusicApp extends Application {
	
	public static boolean mIsSleepClockSetting = false;
	private static String rootPath = "/ccmMusic";
	public static String lrcPath = "/lrc";
	public static String musicPath = "/musicFiles";
	public static String recPath = "/recFiles";
	public static String logFileName = null;
	public static String mCurrFileName = null;
	public static final String TAG = "MyUSBAdio";
	
	@Override
	public void onCreate() {
		super.onCreate();
		Log.d(TAG, "Application OnCreate");
		initPath();
	}
	
	private void initPath() {//
		String ROOT = "";
		if(Environment.getExternalStorageState().equals(Environment.MEDIA_MOUNTED)) {
			ROOT = Environment.getExternalStorageDirectory().getPath();
		}
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
	}
}