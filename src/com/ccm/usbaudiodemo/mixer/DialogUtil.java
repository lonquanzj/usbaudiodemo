package com.ccm.usbaudiodemo.mixer;



import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.content.DialogInterface.OnClickListener;
import android.widget.Toast;

public class DialogUtil {
	private static AlertDialog dialog=null;
	/**
	 * 弹出一个消息框
	 * @param context
	 * @param msg 消息内容
	 */
	public static void showToast(Context context,String msg){
		Toast.makeText(context, msg, Toast.LENGTH_SHORT).show();
	}
	
	/**
	 * 弹出一个选择对话框
	 * @param context
	 * @param title	对话框的标题
	 * @param items 对话框里面的数据项
	 * @param dialogInterface 点击对话框里面的子项的监听事件
	 */
	public static  void showAlertDialog(Context context,String title,String[] items,OnClickListener dialogInterface){
		AlertDialog.Builder builder=new AlertDialog.Builder(context);
		builder.setTitle(title).setItems(items,dialogInterface);
		dialog=builder.create();
		dialog.show();
	}
	/**
	 * 弹出一个选择对话框
	 * @param context
	 * @param title	对话框的标题
	 * @param items 对话框里面的数据项
	 * @param dialogInterface 点击对话框里面的子项的监听事件
	 */
	public static  void showAlertDialog(Context context,String title,int items,OnClickListener dialogInterface){
		AlertDialog.Builder builder=new AlertDialog.Builder(context);
		builder.setTitle(title).setItems(items,dialogInterface);
		dialog=builder.create();
		dialog.show();
	}
	/**
	 * 关闭弹出框
	 */
	public static void closeAlertDialog(){
		if(dialog!=null && dialog.isShowing()){
			dialog.dismiss();
		}
	}
	/**
	 * show a closeDialog
	 * @param context
	 * @param dialogInterface 确定按钮事件
	 */
	public static void showExitAlertDialog(Context context,OnClickListener dialogInterface){
		AlertDialog.Builder builder=new AlertDialog.Builder(context);
		builder.setTitle("退出").setMessage("您确认要推出吗？")
		.setNegativeButton("取消", new DialogInterface.OnClickListener() {

			@Override
			public void onClick(DialogInterface dialog, int which) {
				closeAlertDialog();
			}
		})
		.setPositiveButton("确定", dialogInterface);
		dialog=builder.create();
		dialog.show();
	}
}
