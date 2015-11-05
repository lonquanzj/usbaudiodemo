package com.ccm.usbaudiodemo.activity;

import android.app.TabActivity;
import android.content.Intent;
import android.os.Bundle;
import android.widget.TabHost;

import com.ccm.usbaudiodemo.R;
import com.lu.demo.annotation.ContentView;
import com.lu.demo.annotation.ViewInjectUtil;

@SuppressWarnings("deprecation")
@ContentView(value = R.layout.page_main)
public class PageMain_PluginActivity extends TabActivity{

	@SuppressWarnings("unused")
	private static final String TAG = "MainActivity";

	private TabHost mTabHost;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		intiTabHost();
	}

	

	@Override
	protected void onDestroy() {
		super.onDestroy();
	}

	private void intiTabHost() {
		ViewInjectUtil.inject(this);
		mTabHost = getTabHost();
		
		
		mTabHost.addTab(mTabHost.newTabSpec("混响器").setIndicator("混响器")
				.setContent(new Intent(this,Page1_HunXiangQiActivity.class)));
		mTabHost.addTab(mTabHost.newTabSpec("人声变调").setIndicator("人声变调")
				.setContent(new Intent(this,Page2_RenShengBianDiaoActivity.class)));
		mTabHost.addTab(mTabHost.newTabSpec("人声均衡").setIndicator("人声均衡")
				.setContent(new Intent(this,Page3_RenShengJunHengQiActivity.class)));
		 mTabHost.addTab(mTabHost.newTabSpec("音乐均衡").setIndicator("音乐均衡")
				 .setContent(new Intent(this,Page4_JunHengQiActivity.class)));
		mTabHost.setCurrentTab(0);
	}

}
