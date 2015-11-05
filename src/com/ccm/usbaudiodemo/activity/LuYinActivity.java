package com.ccm.usbaudiodemo.activity;
//
//import com.lu.demo.annotation.ContentView;
//import com.lu.demo.annotation.ViewInject;
//import com.lu.demo.annotation.ViewInjectUtil;
//import com.lu.demo.util.DialogUtil;
//import com.lu.demo.util.UsbDevicesUtil;
//
//import android.app.Activity;
//import android.os.Bundle;
//import android.view.View;
//import android.widget.Button;
//import android.widget.TextView;
//
//@ContentView(value=R.layout.activity_usbreadwritemain)
//public class LuYinActivity extends Activity{
//	
//	@ViewInject(value = R.id.tv_message)
//	private TextView tv_message;
//
//	@ViewInject(value = R.id.btn_receivedMusicData)
//	private Button btn_receivedMusicData;
//	
//	protected UsbDevicesUtil usbDevicesUtil;
//	
//	@Override
//	protected void onCreate(Bundle savedInstanceState) {
//		// TODO Auto-generated method stub
//		super.onCreate(savedInstanceState);
//		
//		usbDevicesUtil = UsbDevicesUtil.getInstance();
//		
//		ViewInjectUtil.inject(this);
//		
//		
//	}
//	
//	// ¼��
//		private boolean isRecord = false;
//
//		public void receivedMusicData(View view) {
//
//			if (!usbDevicesUtil.isConnection()) {
//				DialogUtil.showToast(this, "�豸Ϊ��");
//				return;
//			}
//
//			if (!isRecord) {
//				usbDevicesUtil.startRecord();
//				btn_receivedMusicData.setText("ֹͣ¼��");
//			} else {
//				usbDevicesUtil.stopRecord();
//				btn_receivedMusicData.setText("��ʼ¼��");
//			}
//			isRecord = !isRecord;
//		}
//}
