package com.ccm.usbaudiodemo.mixer;


public class Item {
	public String name;
	public int imgRes;
	public int imgPressRes;
	public boolean isSelected;
	public Item(String name,int imgRes,boolean isSelected){
		this.name=name;
		this.imgRes=imgRes;
		this.isSelected=isSelected;
	};
	public Item(String name,int imgRes,int imgPressRes,boolean isSelected){
		this(name, imgRes, isSelected);
		this.imgPressRes=imgPressRes;
	}
}
