package com.ccm.usbaudiodemo.activity;

import java.util.ArrayList;
import java.util.List;

import android.view.View;
import android.widget.SeekBar;
import android.widget.TextView;

import com.ccm.usbaudiodemo.R;
import com.ccm.usbaudiodemo.mixer.DialogUtil;
import com.lu.demo.annotation.ContentView;
import com.lu.demo.annotation.ViewInject;

@ContentView(value = R.layout.page3_renshengjunhengqi)
public class Page3_RenShengJunHengQiActivity extends BaseActivity {
	@ViewInject(value = R.id.sb_junheng1) private SeekBar sb_junheng1;
	@ViewInject(value = R.id.tv_junheng1) private TextView tv_junheng1;

	@ViewInject(value = R.id.sb_junheng2) private SeekBar sb_junheng2;
	@ViewInject(value = R.id.tv_junheng2) private TextView tv_junheng2;

	@ViewInject(value = R.id.sb_junheng3) private SeekBar sb_junheng3;
	@ViewInject(value = R.id.tv_eq3)      private TextView tv_junheng3;

	@ViewInject(value = R.id.sb_junheng4) private SeekBar sb_junheng4;
	@ViewInject(value = R.id.tv_eq4)      private TextView tv_junheng4;

	@ViewInject(value = R.id.sb_junheng5) private SeekBar sb_junheng5;
	@ViewInject(value = R.id.tv_eq5)      private TextView tv_junheng5;

	@ViewInject(value = R.id.sb_junheng6) private SeekBar sb_junheng6;
	@ViewInject(value = R.id.tv_eq6)      private TextView tv_junheng6;

	@ViewInject(value = R.id.sb_junheng7) private SeekBar sb_junheng7;
	@ViewInject(value = R.id.tv_eq7)      private TextView tv_junheng7;

	@ViewInject(value = R.id.sb_junheng8) private SeekBar sb_junheng8;
	@ViewInject(value = R.id.tv_eq8)      private TextView tv_junheng8;

	private List<SeekBar> listJunHengQiSeekBars;
	private List<TextView> listTextViews= new ArrayList<TextView>();

	protected void setJunHengQi(int position) {
		// for (int i = 0; i < yuSheData[position].length; i++) {
		// listJunHengQiSeekBars.get(i).setProgress(yuSheData[position][i]);
		// }
	}

	@Override
	public void onProgressChanged(SeekBar seekBar, int progress,
			boolean fromUser) {

		// 如果是用户点击屏幕改变滑块值
		if (!fromUser) {

			// 当前所有滑块值是否对应配置

			// 对应配置,配置为选中状态

			// 没有对应配置,配置不选中
			return;
		}

		switch (seekBar.getId()) {
		case R.id.sb_junheng1:
			tv_junheng1.setText(String.valueOf(seekBar.getProgress()));
			sendJunHengQi(30, seekBar.getProgress());
			break;
		case R.id.sb_junheng2:
			sendJunHengQi(31, seekBar.getProgress());
			tv_junheng2.setText(String.valueOf(seekBar.getProgress()));
			break;
		case R.id.sb_junheng3:
			sendJunHengQi(32, seekBar.getProgress());
			tv_junheng3.setText(String.valueOf(seekBar.getProgress()));
			break;
		case R.id.sb_junheng4:
			sendJunHengQi(33, seekBar.getProgress());
			tv_junheng4.setText(String.valueOf(seekBar.getProgress()));
			break;
		case R.id.sb_junheng5:
			sendJunHengQi(34, seekBar.getProgress());
			tv_junheng5.setText(String.valueOf(seekBar.getProgress()));
			break;
		case R.id.sb_junheng6:
			sendJunHengQi(35, seekBar.getProgress());
			tv_junheng6.setText(String.valueOf(seekBar.getProgress()));
			break;
		case R.id.sb_junheng7:
			sendJunHengQi(36, seekBar.getProgress());
			tv_junheng7.setText(String.valueOf(seekBar.getProgress()));
			break;
		case R.id.sb_junheng8:
			sendJunHengQi(37, seekBar.getProgress());
			tv_junheng8.setText(String.valueOf(seekBar.getProgress()));
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

		// if (!isUsbConnection()) {
		// return;
		// }
		usbDevicesUtil.sendParamterDataNew(index, progress);
	}

	@Override
	protected void initList() {
		list = listAddItem(
				getResources().getStringArray(R.array.set_RenShengJunHeng),
				imgsNormal);
		this.currentSetRes = R.id.tv_junHengQiCurrentSet;
		this.gridViewRes = R.id.gridview_junhengqi;
	}

	@Override
	protected void initListener() {
		sb_junheng1.setOnSeekBarChangeListener(this);
		sb_junheng2.setOnSeekBarChangeListener(this);
		sb_junheng3.setOnSeekBarChangeListener(this);
		sb_junheng4.setOnSeekBarChangeListener(this);
		sb_junheng5.setOnSeekBarChangeListener(this);
		sb_junheng6.setOnSeekBarChangeListener(this);
		sb_junheng7.setOnSeekBarChangeListener(this);
		sb_junheng8.setOnSeekBarChangeListener(this);

		listJunHengQiSeekBars = new ArrayList<SeekBar>(10);
		listJunHengQiSeekBars.add(sb_junheng1);
		listJunHengQiSeekBars.add(sb_junheng2);
		listJunHengQiSeekBars.add(sb_junheng3);
		listJunHengQiSeekBars.add(sb_junheng4);
		listJunHengQiSeekBars.add(sb_junheng5);
		listJunHengQiSeekBars.add(sb_junheng6);
		listJunHengQiSeekBars.add(sb_junheng7);
		listJunHengQiSeekBars.add(sb_junheng8);
		listTextViews.add(tv_junheng1);
		listTextViews.add(tv_junheng2);
		listTextViews.add(tv_junheng3);
		listTextViews.add(tv_junheng4);
		listTextViews.add(tv_junheng5);
		listTextViews.add(tv_junheng6);
		listTextViews.add(tv_junheng7);
		listTextViews.add(tv_junheng8);

	}

	@Override
	protected void notifyDataSetChanged(int position) {
		// setJunHengQi(position);
		usbDevicesUtil.sendRenShengEQYuSheData(position);
		updateUI();
	}
	public void save(View view){
		byte[] data=new byte[9];
		data[0]=50;
		for(int i=0;i<8;i++){
			data[i+1]=(byte) listJunHengQiSeekBars.get(i).getProgress();
		}
		usbDevicesUtil.saveRenShengData(data);
		DialogUtil.showToast(this, "当前配置保存成功");
	}
	@Override
	protected void updateUI() {

		byte[] date = usbDevicesUtil.getCurrentRenShengData();
		//人声均衡数据从第2位开始
		for (int i = 0; i < 8; i++) {
			listJunHengQiSeekBars.get(i).setProgress(date[i+1]);
			listTextViews.get(i).setText(""+date[i+1]);
		}
	}

}
