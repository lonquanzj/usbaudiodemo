package com.ccm.usbaudiodemo.adapter;

import java.util.List;

import android.annotation.SuppressLint;
import android.content.Context;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.BaseAdapter;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.TextView;

import com.ccm.usbaudiodemo.R;
import com.ccm.usbaudiodemo.mixer.Item;

public class MyAdapter extends BaseAdapter {

	private List<Item> listItems;
	private int res;
	private Context context;
	private LayoutInflater inflater;
	public MyAdapter(int res, List<Item> listItems,LayoutInflater inflater) {
		this.listItems = listItems;
		this.res = res;
		this.inflater=inflater;
	}

	@Override
	public int getCount() {

		return listItems.size();
	}

	@Override
	public Object getItem(int position) {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public long getItemId(int position) {
		// TODO Auto-generated method stub
		return 0;
	}

	@SuppressLint({ "ResourceAsColor", "NewApi" })
	@Override
	public View getView(int position, View convertView, ViewGroup parent) {

		if (convertView == null) {
			convertView = inflater.inflate(res, null);
		}

		ImageView item_img = (ImageView) convertView
				.findViewById(R.id.item_img);
		item_img.setBackgroundResource(listItems.get(position).imgRes);

		TextView item_tv_yinxiao = (TextView) convertView
				.findViewById(R.id.item_tv_yinxiao);

		item_tv_yinxiao.setText(listItems.get(position).name);
		LinearLayout ll_bg = (LinearLayout) convertView
				.findViewById(R.id.ll_bg);
		ll_bg.setBackground(null);
		if (listItems.get(position).isSelected) {
			ll_bg.setBackgroundResource(R.color.selectedBgColor);
		}
		return convertView;
	}

}
