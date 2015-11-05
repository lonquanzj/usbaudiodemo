package com.ccm.usbaudiodemo.activity;

import java.util.ArrayList;
import java.util.List;

import android.app.Activity;
import android.os.Bundle;
import android.view.View;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.GridView;
import android.widget.SeekBar;
import android.widget.SeekBar.OnSeekBarChangeListener;
import android.widget.TextView;

import com.ccm.usbaudiodemo.R;
import com.ccm.usbaudiodemo.adapter.LuAdapter;
import com.ccm.usbaudiodemo.adapter.ViewHolder;
import com.ccm.usbaudiodemo.mixer.Item;
import com.ccm.usbaudiodemo.mixer.UsbDevicesUtil;
import com.lu.demo.annotation.ViewInjectUtil;


public abstract class BaseActivity extends Activity implements OnSeekBarChangeListener,OnItemClickListener{
	protected UsbDevicesUtil usbDevicesUtil;
	protected LuAdapter<Item> luAdapter;
	protected List<Item> list ;
	protected TextView currentSet; 
	protected GridView gridView;
	protected int currentSetRes;
	protected int gridViewRes;
	protected boolean isShanBi=false;
	
	/** 设置图片1-7,最后一张是自定义 */
	protected int[] imgsNormal = { R.drawable.zdy_normal,
			R.drawable.zdy_normal, R.drawable.zdy_normal,
			R.drawable.zdy_normal, R.drawable.zdy_normal,
			R.drawable.zdy_normal, R.drawable.zdy_normal,
			R.drawable.zdy_normal,R.drawable.zdy_normal, R.drawable.zdy_normal,  };
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		
		ViewInjectUtil.inject(this);
		
		usbDevicesUtil=UsbDevicesUtil.getInstance();
		isShanBi();
		if(!isShanBi){
			initList();
			initAdapter();
			initWidget();
		}
		initListener();
	}
	protected void isShanBi(){
	}
	/**
	 * 子类实现,初始化 list,currentSetRes,gridViewRes
	 */
	protected abstract void initList();

	protected void initAdapter() {
		luAdapter = new LuAdapter<Item>(this, list, R.layout.griditem_yinxiao) {

			@Override
			public void convert(ViewHolder helper, Item item) {
				helper.setImage(R.id.item_img, item.imgRes);
				helper.setString(R.id.item_tv_yinxiao, item.name);
				helper.setBackground(R.id.ll_bg, item.isSelected?R.color.selectedBgColor:0);
			}
		};
		
	}
	/**
	 * 子类实现
	 */
	protected abstract void initListener();
	/**
	 * 父类实例化 TextView,GridView
	 */
	protected void initWidget() {
		
		currentSet=(TextView) findViewById(currentSetRes);
		gridView=(GridView) findViewById(gridViewRes);
		gridView.setAdapter(luAdapter);
		gridView.setOnItemClickListener(this);
	}
	
	
	
	@Override
	public void onProgressChanged(SeekBar seekBar, int progress,
			boolean fromUser) {
		
	}

	@Override
	public void onStartTrackingTouch(SeekBar seekBar) {
		
	}

	@Override
	public void onStopTrackingTouch(SeekBar seekBar) {
		
	}
	protected List<Item> listAddItem(String[] array, int[] res) {
		List<Item> list = new ArrayList<Item>();
		for (int i = 0; i < array.length; i++) {
			list.add(new Item(array[i], res[i], false));
		}
		list.set(array.length - 1, new Item(array[array.length - 1],
				res[res.length - 1], false));
		return list;
	}

	@Override
	public void onItemClick(AdapterView<?> parent, View view, int position,
			long id) {
		if(currentSet!=null){
				currentSet.setText("当前配置 ：" + list.get(position).name);
		}
		
		for (int i = 0; i < list.size(); i++) {
			if (i != position) {
				list.get(i).isSelected = false;
			} else {
				list.get(i).isSelected = true;
			}
		}
		luAdapter.notifyDataSetChanged();
		notifyDataSetChanged(position);
	}
	/**
	 * gridView 点击回调方法
	 * @param position
	 */
	protected abstract void notifyDataSetChanged(int position);
	protected abstract void updateUI();
	/**
	 * 设备是否连接成功
	 * @return true 连接成功
	 */
	protected boolean isUsbConnection(){
		/*if(usbDevicesUtil!=null && usbDevicesUtil.isConnection()){
			return true;
		}else*/{
//			DialogUtil.showToast(this, "设备未连接");
			currentSet.setText( "设备未连接");
			return false;
		}
	}
	
}
