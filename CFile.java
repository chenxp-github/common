package com.jni.common;

public class CFile extends CFileBase{
    private long __obj = 0;
    private void __dummy(){}

    public CFile()
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
    public native boolean isOpened();
    public native boolean openFile(String fn, String mode);
    public native boolean closeFile();
    public native long getSizeReal();
    public native boolean hasBuffer();
    public native boolean setBufSize(long size);
}
