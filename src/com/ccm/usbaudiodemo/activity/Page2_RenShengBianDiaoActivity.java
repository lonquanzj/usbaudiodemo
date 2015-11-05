package com.ccm.usbaudiodemo.activity;

import android.view.View;
import android.widget.SeekBar;
import android.widget.TextView;

import com.ccm.usbaudiodemo.R;
import com.ccm.usbaudiodemo.mixer.DialogUtil;
import com.lu.demo.annotation.ContentView;
import com.lu.demo.annotation.ViewInject;

@ContentView(value = R.layout.page2_renshengbd)
public class Page2_RenShengBianDiaoActivity extends BaseActivity {
	

	@ViewInject(value = R.id.sb_renshengbiandiao) private SeekBar sb_renshengbiandiao;
	@ViewInject(value = R.id.tv_renshengbiandiao) private TextView tv_renshengbiandiao;


//	private List<SeekBar> listJunHengQiSeekBars;

	protected void setJunHengQi(int position) {
	}

	@Override
	public void onProgressChanged(SeekBar seekBar, int progress,
			boolean fromUser) {
		
		
		//如果是用户点击屏幕改变滑块值
		if(!fromUser){
			
			//当前所有滑块值是否对应配置
			
			//对应配置,配置为选中状态
			
			//没有对应配置,配置不选中
			return;
		}
		

		switch (seekBar.getId()) {
		case R.id.sb_renshengbiandiao:
			sendJunHengQi(20, seekBar.getProgress());
			tv_renshengbiandiao.setText(String.valueOf(seekBar.getProgress()));
			break;
		}

	}

	/**
	 * 发送均衡器数据
	 * 
	 * @param index
	 * @param progress
	 */
	private void sendJunHengQi(int index, int progress) {

//		if (!isUsbConnection()) {
//			return;
//		}
		usbDevicesUtil.sendParamterDataNew(index, progress);
	}

	@Override
	protected void initList() {
		list = listAddItem(
				getResources().getStringArray(R.array.set_RenShengBianDiao),
				imgsNormal);
		this.currentSetRes = R.id.tv_junHengQiCurrentSet;
		this.gridViewRes = R.id.gridview_renshengbiandiao;
	}

	@Override
	protected void initListener() {
		sb_renshengbiandiao.setOnSeekBarChangeListener(this);

//		listJunHengQiSeekBars = new ArrayList<SeekBar>(10);

	}

	@Override
	protected void notifyDataSetChanged(int position) {
		
		usbDevicesUtil.sendRenShengBiaoDiaoYuSheDate(position);
		updateUI();
	}
	public void save(View view){
		DialogUtil.showToast(this, "当前配置保存成功");
	}
	@Override
	protected void updateUI() {

		byte[] data = usbDevicesUtil.getCurrentRenShengData();
		sb_renshengbiandiao.setProgress(data[0]);
		tv_renshengbiandiao.setText(""+data[0]);
	}

}
