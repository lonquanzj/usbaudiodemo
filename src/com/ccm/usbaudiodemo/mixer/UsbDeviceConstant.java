package com.ccm.usbaudiodemo.mixer;

/**
 *  设备接口常量类
 * @author Administrator
 *
 */
public class UsbDeviceConstant {
	
	
	/**
	 * 向设备发送数据成功
	 */
	public final static int SEND_DATA_SUCCESS=0;
	/**
	 * 向设备发送数据成功
	 */
	public final static int SEND_DATA_FAIL=1;
	
	/**
	 * 无效设备
	 */
	public final static int INVALID_DEVICE=3;
	
	/**
	 * 有效设备
	 */
	public final static int VALID_DEVICE=4;
	
	/**
	 * 设备连接成功
	 */
	public final static int DEVICE_CONNECTION_SUCCESS = 77;
	/**
	 * 设备连接失败
	 */
	public final static int DEVICE_CONNECTION_FAIL = 78;
	
	/**
	 * 设备已插入
	 */
	public final static int ACTION_USB_DEVICE_ATTACHED = 79;
	/**
	 * 设备已删除
	 */
	public final static int ACTION_USB_DEVICE_DETACHED = 80;
	
	
	/**
	 * 音效预设[录音棚,KTV,经典男声,经典女声,演唱会,音乐厅,空谷,留声机]
	 */
	public final static byte[][] tianLaieffectDataNew = {

			// 原声
			{ 45, 42, 42, 45, 50, 55, 59,50, 50, 50, 50, 50, 50, 50, 50, 50, 
					50, 50, 50, 50, 50, 50, 50, 50, 50, 50, },
				
			// 录音棚
			{ 0, 7, 100, 51, 40, 100, 50, 50, 42, 45, 50, 55, 59, 63, 62, 58,
					45, 42, 42, 45, 50, 55, 59, 63, 62, 58, },
			// 演唱会
			{ 31, 25, 65, 66, 35, 100, 50, 50, 49, 66, 66, 75, 52, 44, 33, 21,
					25, 43, 49, 66, 66, 75, 52, 44, 33, 21, },
			// KTV
			{ 14, 11, 80, 71, 55, 100, 46, 50, 63, 62, 67, 75, 66, 56, 47, 29,
					55, 59, 63, 62, 67, 75, 66, 56, 47, 29 },
			// 经典男声
			{ 0, 8, 83, 23, 0, 100, 66, 50, 64, 72, 80, 84, 77, 64, 54, 45, 47,
					57, 64, 72, 80, 84, 77, 64, 54, 45 },

			// 经典女声
			{ 0, 8, 83, 23, 0, 100, 66, 50, 54, 60, 65, 66, 54, 49, 41, 27, 30,
					45, 54, 60, 65, 66, 54, 49, 41, 27 },

	};

	/**
	 * 混响器的预设数据
	 */
	public final static byte[][] hunXiangQiNew = {
			// 小厅效果
			{ 0, 7, 100, 51, 40, 100, 50, },
			// 房间效果
			{ 11, 20, 46, 73, 39, 100, 46, },
			// KTV效果
			{ 14, 11, 80, 71, 55, 100, 46, },
			// 浴室效果
			{ 0, 8, 83, 23, 0, 100, 66, },
			// 体育馆效果
			{ 31, 25, 65, 66, 35, 100, 50, },
			// 音乐厅效果
			{ 22, 43, 96, 66, 38, 100, 40, }

	};

	/**
	 * 均衡器的预设
	 */
	// private int[][] junHengQi = new int[6][10];

	public final static byte[][] junHengQiMusicNew = {
			// 摇滚
			{ 74, 63, 59, 54, 49, 49, 54, 59, 62, 74 },
			// 流行
			{ 54, 59, 62, 65, 75, 74, 67, 62, 59, 54 },
			// 爵士
			{ 58, 62, 63, 59, 56, 49, 45, 40, 38, 45 },
			// 古典
			{ 45, 40, 41, 44, 49, 54, 59, 63, 63, 58 },
			// 重低音
			{ 49, 62, 63, 49, 35, 24, 38, 63, 73, 63, }

	};

	public final static byte[][] junHengQiRenShengNew = {
			// 经典男声
			{ 64, 72, 79, 84, 77, 64, 54, 45, },
			// 经典女声
			{ 54, 60, 65, 66, 54, 49, 41, 27, },
			// 民族
			{ 41, 44, 49, 54, 59, 63, 63, 58 },

	};

	/**
	 * 人声变调预设,小孩声,女高声，男低声，怪兽声
	 */
	public final static byte[] junHengQiVoiceNew = { 80, 62, 46, 25

	};
	
	
}
