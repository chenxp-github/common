package com.jni.common;

import com.cvtest.common.Callback;

public class CServerSidePeer{
    private long __obj = 0;
    private void __dummy(){}

    public CServerSidePeer()
    {
        this._new();
    }

    public void destroy()
    {
        this._gc();
    }

    protected void finalize()
    {
        this.destroy();
    }

    public boolean setOnMessage(Callback callback)
    {
    	return this.setOnMessage((Object)callback);
    }
    
    public native int _gc();
    public native boolean _new();
    public native int getAliveClientNumber();
    public native boolean clearSendingQueue();
    public native int getSendingQueueLength();
    public native boolean resumeFetchMessageTask();
    public native boolean suspendFetchMessageTask();
    public native boolean startFetchMessageTask();
    public native boolean setCanFetchMessage(boolean can);
    public native boolean connect(String peer_name);
    public native boolean start();
    public native String getName();
    public native boolean setName(String name);
    
    public native boolean sendMessage(String to, int func, int cbid, int flags, String body);
    public native boolean sendMessage(String to, int func, int cbid, int flags, CMiniBson bson);    
    private native boolean setOnMessage(Object obj);    
}
