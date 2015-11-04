///**
// * Copyright (c) www.longdw.com
// */
//package com.ldw.music.adapter;
//
//import java.util.ArrayList;
//import java.util.List;
//
//import android.content.Context;
//import android.graphics.Color;
//import android.util.Log;
//import android.view.LayoutInflater;
//import android.view.View;
//import android.view.ViewGroup;
//import android.widget.BaseAdapter;
//import android.widget.TextView;
//
//import com.ldw.music.R;
//import com.ldw.music.model.LyricSentence;
//
//public class LyricAdapter extends BaseAdapter {
//	private static final String TAG = LyricAdapter.class.getSimpleName();
//
//	/** 姝岃瘝鍙ュ瓙闆嗗悎 */
//	List<LyricSentence> mLyricSentences = null;
//
//	Context mContext = null;
//
//	/** 褰撳墠鐨勫彞瀛愮储寮曞彿 */
//	int mIndexOfCurrentSentence = 0;
//
//	float mCurrentSize = 20;
//	float mNotCurrentSize = 17;
//
//	public LyricAdapter(Context context) {
//		mContext = context;
//		mLyricSentences = new ArrayList<LyricSentence>();
//		mIndexOfCurrentSentence = 0;
//	}
//
//	/** 璁剧疆姝岃瘝锛岀敱澶栭儴璋冪敤锛� */
//	public void setLyric(List<LyricSentence> lyric) {
//		mLyricSentences.clear();
//		if (lyric != null) {
//			mLyricSentences.addAll(lyric);
//			Log.i(TAG, "姝岃瘝鍙ュ瓙鏁扮洰=" + mLyricSentences.size());
//		}
//		mIndexOfCurrentSentence = 0;
//	}
//
//	@Override
//	public boolean isEmpty() {
//		// 姝岃瘝涓虹┖鏃讹紝璁㎜istView鏄剧ずEmptyView
//		if (mLyricSentences == null) {
//			Log.i(TAG, "isEmpty:null");
//			return true;
//		} else if (mLyricSentences.size() == 0) {
//			Log.i(TAG, "isEmpty:size=0");
//			return true;
//		} else {
//			Log.i(TAG, "isEmpty:not empty");
//			return false;
//		}
//	}
//
//	@Override
//	public boolean isEnabled(int position) {
//		// 绂佹鍦ㄥ垪琛ㄦ潯鐩笂鐐瑰嚮
//		return false;
//	}
//
//	@Override
//	public int getCount() {
//		return mLyricSentences.size();
//	}
//
//	@Override
//	public Object getItem(int position) {
//		return mLyricSentences.get(position).getContentText();
//	}
//
//	@Override
//	public long getItemId(int position) {
//		return position;
//	}
//
//	@Override
//	public View getView(int position, View convertView, ViewGroup parent) {
//		ViewHolder holder = null;
//		if (convertView == null) {
//			holder = new ViewHolder();
//			LayoutInflater inflater = (LayoutInflater) mContext
//					.getSystemService(Context.LAYOUT_INFLATER_SERVICE);
//			convertView = inflater.inflate(R.layout.lyric_line, null);
//			holder.lyric_line = (TextView) convertView
//					.findViewById(R.id.lyric_line_text);
//			convertView.setTag(holder);
//		} else {
//			holder = (ViewHolder) convertView.getTag();
//		}
//		if (position >= 0 && position < mLyricSentences.size()) {
//			holder.lyric_line.setText(mLyricSentences.get(position)
//					.getContentText());
//		}
//		if (mIndexOfCurrentSentence == position) {
//			// 褰撳墠鎾斁鍒扮殑鍙ュ瓙璁剧疆涓虹櫧鑹诧紝瀛椾綋澶у皬鏇村ぇ
//			holder.lyric_line.setTextColor(Color.WHITE);
//			holder.lyric_line.setTextSize(mCurrentSize);
//		} else {
//			// 鍏朵粬鐨勫彞瀛愯缃负鏆楄壊锛屽瓧浣撳ぇ灏忚緝灏�
//			holder.lyric_line.setTextColor(mContext.getResources().getColor(
//					R.color.trans_white));
//			holder.lyric_line.setTextSize(mNotCurrentSize);
//		}
//		return convertView;
//	}
//
//	public void setCurrentSentenceIndex(int index) {
//		mIndexOfCurrentSentence = index;
//	}
//
//	static class ViewHolder {
//		TextView lyric_line;
//	}
//}
