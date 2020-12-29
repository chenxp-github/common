package com.jni.common;

import android.os.Handler;
import android.os.Message;

public class NativeBridge {    
	public boolean need_msg_loop = true;
	public Handler handler = null;
	
	public native int initNative();
	public native int destroyNative();
	public native int mainLoop();
	
	class MyHandler extends Handler{
		public NativeBridge m_host = null;
		public MyHandler(NativeBridge host)
		{
			this.m_host = host;
		}
		public void handleMessage(Message msg) 
		{
			if(m_host.need_msg_loop)
				m_host.mainLoop();
			sendMessageDelayed(Message.obtain(), 1);
		} 
	}
	
	public void installMainLoop() {
		this.need_msg_loop = true;
		if(handler == null)
		{
			handler = new MyHandler(this);
			handler.sendMessageDelayed(Message.obtain(), 1000);
		}
	}	
	
	public void uninstallMainLoop() {
		this.need_msg_loop = false;
	}
}

