package com.jni.common;

public class CMem extends CFileBase{
    private long __obj = 0;
    private void __dummy(){}

    public CMem()
    {
        this._new();
    }

    public void destroy()
    {
        this._gc();
    }

    protected void finalize()
    {
        super.destroy();
    }

    public native int _gc();
    public native boolean _new();
    public native boolean isMalloc();
    public native boolean transfer(CMem from);
    public native boolean realloc(long newsize);
    public native boolean strEqu(CMem str, boolean case_sensitive);
    public native byte c(int index);
    public native boolean malloc(long asize);
    public native boolean zero();
    public native boolean copy(CFileBase file);
    public native String cStr();
    public native boolean init();
    public native long getRawBuf();
    public native boolean setRawBuf(long buf, long size, boolean is_const);
}
