package com.jni.common;

import com.cvtest.common.Callback;

public class CMessagePeer{
    private long __obj = 0;
    private void __dummy(){}

    public CMessagePeer()
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
    public native boolean setMaxRetries(int max_retries);
    public native boolean clearSendingQueue();
    public native int getSendingQueueLength();
    public native boolean isConnected();
    public native boolean setCanFetchMessage(boolean can_fetch);
    public native String[] getAllConnectedPeers();
    public native boolean connect(String peer_name);
    public native boolean start();
    public native boolean setName(CMem name);
    public native boolean setName(String name);
    public native boolean setServer(CMem server);
    public native boolean setServer(String server);
    public native boolean setPort(int port);
    public native String getName();
    public native String getServer();
    public native int getPort();
    public native boolean sendMessage(String to, int func, int cbid, int flags, String body);
    public native boolean sendMessage(String to, int func, int cbid, int flags, CMiniBson bson);    
    private native boolean setOnMessage(Object obj);
}
