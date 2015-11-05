package com.ccm.usbaudiodemo.adapter;

import android.annotation.SuppressLint;
import android.content.Context;
import android.util.SparseArray;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.ImageView;
import android.widget.TextView;

public class ViewHolder {
	/***和Map类似，但是比Map效率，不过键只能为Integer*/
	private final SparseArray<View> mViews;
	private View mConvertView;
	private ViewHolder(Context context,ViewGroup parent,int layoutId,int postion){
		mViews=new SparseArray<View>();
		mConvertView=LayoutInflater.from(context).inflate(layoutId, parent,false);
		mConvertView.setTag(this);
	}
	/**
	 * 拿到一个ViewHolder对象
	 * @param context
	 * @param converView
	 * @param parent
	 * @param layoutId
	 * @param postion
	 * @return
	 */
	public static ViewHolder get(Context context,View converView,ViewGroup parent,int layoutId,int postion){
		if(converView==null){
			return new ViewHolder(context, parent, layoutId, postion);
		}
		return (ViewHolder)converView.getTag();
	}
	@SuppressWarnings("unchecked")
	public <T extends View> T getView(int viewId){
		View view=mViews.get(viewId);
		if(view==null){
			view=mConvertView.findViewById(viewId);
			mViews.put(viewId, view);
		}
		return (T) view;
	}
	public View getConverView(){
		return mConvertView;
	}
	public void setString(int viewId,String value){
		TextView textView=getView(viewId);
		textView.setText(value);
	}
	public void setImage(int viewId,int value){
		ImageView imageView=getView(viewId);
		imageView.setBackgroundResource(value);
	
	}
	@SuppressLint("NewApi") 
	public void setBackground(int viewId,int value){
		View view=getView(viewId);
		if(value!=0){
			view.setBackgroundResource(value);
		}else{
			view.setBackground(null);
		}
	}
}
