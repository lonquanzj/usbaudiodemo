package com.ccm.usbaudiodemo.mixer;
import android.util.Log;

public class LogUtil {
	public static void log(Class tag, String msg){
		Log.i(tag.getSimpleName(), msg);
	};
	public static void i(Class tag, String msg){
		Log.i(tag.getSimpleName(), msg);
	};
	public static void i(String tag, String msg){
		Log.i(tag, msg);
	};
}
