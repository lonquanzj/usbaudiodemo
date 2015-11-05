package com.ccm.usbaudiodemo.activity;

import android.annotation.SuppressLint;
import android.content.Intent;
import android.os.Handler;
import android.os.Message;
import android.widget.SeekBar;
import android.widget.TextView;

import com.ccm.usbaudiodemo.R;
import com.ccm.usbaudiodemo.mixer.DialogUtil;
import com.ccm.usbaudiodemo.mixer.UsbDeviceConstant;
import com.ccm.usbaudiodemo.mixer.UsbDevicesUtil;
import com.lu.demo.annotation.ContentView;
import com.lu.demo.annotation.ViewInject;

@ContentView(value=R.layout.tiaoyin_main)
public class YinXiaoActivity extends BaseActivity{

	/** 音效图片 */
	private int[] imgsYinXiao = {  R.drawable.zdy_normal,
			R.drawable.zdy_normal, R.drawable.zdy_normal,
			R.drawable.zdy_normal, R.drawable.zdy_normal,
			R.drawable.zdy_normal, R.drawable.zdy_normal,
			R.drawable.zdy_normal,R.drawable.zdy_normal, R.drawable.zdy_normal,  };

	
	@ViewInject(value = R.id.sb_banzou)    private SeekBar sb_banzou;
	@ViewInject(value = R.id.sb_huatong)   private SeekBar sb_huatong;
	@ViewInject(value = R.id.sb_mainVoice) private SeekBar sb_mainVoice;
	@ViewInject(value = R.id.tv_voice)     private TextView tv_voice;
	@ViewInject(R.id.tv_banzou)            private TextView tv_banzou;
	@ViewInject(R.id.tv_mainVoice)         private TextView tv_mainVoice;
	
	private UsbDevicesUtil usbDevicesUtil;

	@SuppressLint("HandlerLeak")
	private Handler handler = new Handler() {

		@Override
		public void handleMessage(Message msg) {

			switch (msg.what) {
			case UsbDeviceConstant.DEVICE_CONNECTION_SUCCESS:
//				DialogUtil.showToast(getApplicationContext(), "设备连接成功");
				currentSet.setText("设备连接成功");
				break;
			case UsbDeviceConstant.DEVICE_CONNECTION_FAIL:
//				DialogUtil.showToast(getApplicationContext(), "设备连接失败");
				currentSet.setText("设备连接失败");
				break;
				
			case UsbDeviceConstant.ACTION_USB_DEVICE_ATTACHED:
//				DialogUtil.showToast(getApplicationContext(), "检测设备已插入");
				currentSet.setText("检测设备已插入,正在连接中...");
				break;
			case UsbDeviceConstant.ACTION_USB_DEVICE_DETACHED :
//				DialogUtil.showToast(getApplicationContext(), "检测设备已删除");
				currentSet.setText("检测设备已删除");
				break;
			default:
				break;
			}

		}

	};
	
	@Override
	protected void initList(){
		list = listAddItem(getResources()
				.getStringArray(R.array.yinxiao), imgsYinXiao);
		this.currentSetRes=R.id.tv_currentYinXiao;
		this.gridViewRes=R.id.gridview;
		
		usbDevicesUtil =  UsbDevicesUtil.getInstance(this, handler);
	};
	@Override
	protected void initListener() {
		sb_huatong.setOnSeekBarChangeListener(this);
		sb_banzou.setOnSeekBarChangeListener(this);
		sb_mainVoice.setOnSeekBarChangeListener(this);
		
	}
	@Override
	public void onProgressChanged(SeekBar seekBar, int progress,
			boolean fromUser) {
		switch (seekBar.getId()) {
		
		case R.id.sb_huatong:
			tv_voice.setText(String.valueOf(seekBar.getProgress()));
			sendVoice(0, seekBar.getProgress());
			break;
		case R.id.sb_banzou:
			tv_banzou.setText(String.valueOf(progress));
			sendVoice(1, seekBar.getProgress());
			break;
		case R.id.sb_mainVoice:
			tv_mainVoice.setText(String.valueOf(progress));
			sendVoice(2, seekBar.getProgress());
			break;
		default:
			break;
		}
		
	}
	private void sendVoice(int i, int progress) {
//		if(!isUsbConnection()){
//			return ; 
//			
//		}	
		try{
			usbDevicesUtil.sendParamterDataNew(i, progress);
		}catch(Exception e){
			DialogUtil.showToast(this, "发送异常");
		}
		
		 
	}
	@Override
	protected void notifyDataSetChanged(int position){
		
		if(position==list.size()-1){
			startActivity(new Intent(this,PageMain_PluginActivity.class));
			return;
		}
//		
//		if(!isUsbConnection()){
//			return ; 
//		}
		currentSet.setText("当前音效 ：" + list.get(position).name);
//		try{
		if(position>5){
			position=5;
		}
		try {
		usbDevicesUtil.sendEffectDataNew(position);
		} catch (Exception e) {
			DialogUtil.showToast(this, "发送异常");
		}
		
//		DataUtil.currentEffect=position;
//			usbDevicesUtil.setCurrentEffect(position);
//		}catch(Exception e){
//			DialogUtil.showToast(this, "发送异常");
//		}
		
	 
	}
	@Override 
	protected void updateUI() {
		
	}
	@Override
	protected void onResume() {
		// TODO Auto-generated method stub
		super.onResume();
	}
}
