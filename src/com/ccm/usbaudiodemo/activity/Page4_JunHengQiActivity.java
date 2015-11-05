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

@ContentView(value = R.layout.page4_junhengqi)
public class Page4_JunHengQiActivity extends BaseActivity {
	@ViewInject(value = R.id.sb_junheng1)  private SeekBar  sb_junheng1;
	@ViewInject(value = R.id.tv_junheng1)  private TextView tv_junheng1;

	@ViewInject(value = R.id.sb_junheng2)  private SeekBar  sb_junheng2;
	@ViewInject(value = R.id.tv_junheng2)  private TextView tv_junheng2;

	@ViewInject(value = R.id.sb_junheng3)  private SeekBar  sb_junheng3;
	@ViewInject(value = R.id.tv_eq3)  	   private TextView tv_junheng3;

	@ViewInject(value = R.id.sb_junheng4)  private SeekBar  sb_junheng4;
	@ViewInject(value = R.id.tv_eq4)       private TextView tv_junheng4;

	@ViewInject(value = R.id.sb_junheng5)  private SeekBar  sb_junheng5;
	@ViewInject(value = R.id.tv_eq5)       private TextView tv_junheng5;

	@ViewInject(value = R.id.sb_junheng6)  private SeekBar  sb_junheng6;
	@ViewInject(value = R.id.tv_eq6)       private TextView tv_junheng6;

	@ViewInject(value = R.id.sb_junheng7)  private SeekBar  sb_junheng7;
	@ViewInject(value = R.id.tv_eq7)       private TextView tv_junheng7;

	@ViewInject(value = R.id.sb_junheng8)  private SeekBar  sb_junheng8;
	@ViewInject(value = R.id.tv_eq8)       private TextView tv_junheng8;

	@ViewInject(value = R.id.sb_junheng0)  private SeekBar  sb_junheng0;
	@ViewInject(value = R.id.tv_junheng0)  private TextView tv_junheng0;

	@ViewInject(value = R.id.sb_junheng9)  private SeekBar  sb_junheng9;
	@ViewInject(value = R.id.tv_eq9)       private TextView tv_junheng9;

	private List<SeekBar>  listJunHengQiSeekBars;
	private List<TextView> listTextViews= new ArrayList<TextView>();

	protected void setJunHengQi(int position) {
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
		case R.id.sb_junheng0:
			tv_junheng0.setText("" + progress);
			sendJunHengQi(40, seekBar.getProgress());
			break;
		case R.id.sb_junheng1:
			tv_junheng1.setText(String.valueOf(seekBar.getProgress()));
			sendJunHengQi(41, seekBar.getProgress());
			break;
		case R.id.sb_junheng2:
			sendJunHengQi(42, seekBar.getProgress());
			tv_junheng2.setText(String.valueOf(seekBar.getProgress()));
			break;
		case R.id.sb_junheng3:
			sendJunHengQi(43, seekBar.getProgress());
			tv_junheng3.setText(String.valueOf(seekBar.getProgress()));
			break;
		case R.id.sb_junheng4:
			sendJunHengQi(44, seekBar.getProgress());
			tv_junheng4.setText(String.valueOf(seekBar.getProgress()));
			break;
		case R.id.sb_junheng5:
			sendJunHengQi(45, seekBar.getProgress());
			tv_junheng5.setText(String.valueOf(seekBar.getProgress()));
			break;
		case R.id.sb_junheng6:
			sendJunHengQi(46, seekBar.getProgress());
			tv_junheng6.setText(String.valueOf(seekBar.getProgress()));
			break;
		case R.id.sb_junheng7:
			sendJunHengQi(47, seekBar.getProgress());
			tv_junheng7.setText(String.valueOf(seekBar.getProgress()));
			break;
		case R.id.sb_junheng8:
			sendJunHengQi(48, seekBar.getProgress());
			tv_junheng8.setText(String.valueOf(seekBar.getProgress()));
			break;
		case R.id.sb_junheng9:
			sendJunHengQi(49, seekBar.getProgress());
			tv_junheng9.setText(String.valueOf(seekBar.getProgress()));
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

	public void save(View view) {
		byte[] data=new byte[10];
		for(int i=0;i<data.length;i++){
			data[i]=(byte) listJunHengQiSeekBars.get(i).getProgress();
		}
		usbDevicesUtil.saveMusicData(data);
		DialogUtil.showToast(this, "当前配置保存成功");
	}

	@Override
	protected void initList() {
		list = listAddItem(
				getResources().getStringArray(R.array.set_YinYueJunHeng),
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
		sb_junheng9.setOnSeekBarChangeListener(this);
		sb_junheng0.setOnSeekBarChangeListener(this);

		listJunHengQiSeekBars = new ArrayList<SeekBar>(10);
		listJunHengQiSeekBars.add(sb_junheng0);
		listJunHengQiSeekBars.add(sb_junheng1);
		listJunHengQiSeekBars.add(sb_junheng2);
		listJunHengQiSeekBars.add(sb_junheng3);
		listJunHengQiSeekBars.add(sb_junheng4);
		listJunHengQiSeekBars.add(sb_junheng5);
		listJunHengQiSeekBars.add(sb_junheng6);
		listJunHengQiSeekBars.add(sb_junheng7);
		listJunHengQiSeekBars.add(sb_junheng8);
		listJunHengQiSeekBars.add(sb_junheng9);

		listTextViews = new ArrayList<TextView>();
		listTextViews.add(tv_junheng0);
		listTextViews.add(tv_junheng1);
		listTextViews.add(tv_junheng2);
		listTextViews.add(tv_junheng3);
		listTextViews.add(tv_junheng4);
		listTextViews.add(tv_junheng5);
		listTextViews.add(tv_junheng6);
		listTextViews.add(tv_junheng7);
		listTextViews.add(tv_junheng8);
		listTextViews.add(tv_junheng9);

	}

	@Override
	protected void notifyDataSetChanged(int position) {
		usbDevicesUtil.sendJunHengQiYuSheDataMusic(position);
		updateUI();
	}

	@Override
	protected void updateUI() {

		byte[] date = usbDevicesUtil.getCurrentMusicData();
		// 人声均衡数据从第16位开始
		for (int i = 0; i < 10; i++) {
			listJunHengQiSeekBars.get(i).setProgress(date[i]);
			listTextViews.get(i).setText("" + date[i]);
		}
	}

}
