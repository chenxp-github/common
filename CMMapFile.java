package com.jni.common;

public class CMMapFile{
    private long __obj = 0;
    private void __dummy(){}

    public CMMapFile()
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

    public native int _gc();
    public native boolean _new();
    public native boolean unlink();
    public native boolean sync();
    public native long getSize();
    public native boolean openReadWrite(String filename);
    public native boolean openCreate(String filename, int size);
    public native boolean openReadOnly(String filename);
    public native boolean close();
    public native boolean setFileName(String filename);
    public native CStream stream();
}
