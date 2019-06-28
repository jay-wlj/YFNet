/*
 * @project :MangoTv
 * @author  :Jijia.Deng 
 * @date    :2015-1-8
 */
package com.yunfan.net;

/**
 *
 */
public class TaskInfo{
	
	/**
	 * 任务的hash
	 */
	public byte hash[] = new byte[41];
	
	/**
	 * 文件的大小，Byte
	 */
	public long filelen;
	
	/**
	 * 已下载的大小,Byte
	 */
	public long downloadlen;
	
	/**
	 * 已上传的大小,Byte
	 */
	public long uploadlen;

	/**
	 * 下载速度，字节/秒
	 */
	public int downloadspeed;
	
	/**
	 * 上传速度，字节/秒
	 */
	public int uploadspeed; 
	
	/**
	 * 当前任务的状态，1:下载 2:暂停下载，若任务已下载完，则此状态无意义 3:下载/上传完成
	 */
	public byte status;
	
	/**
	 * 文件的存储路径
	 */
	public String filepath;

}
