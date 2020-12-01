package com.jni.common;

public class CMemFile extends CFileBase{
    private long __obj = 0;
    private void __dummy(){}

    public CMemFile()
    {
        this._new();
    }
    public CMemFile(long page_size, long max_pages)
    {
        this._new(page_size,max_pages);
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
    public native boolean _new(long page_size, long max_pages);
    public native boolean transfer(CMemFile from);
}
