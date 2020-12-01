package com.jni.common;

public class CStream extends CMem{
    private long __obj = 0;
    private void __dummy(){}

    public CStream()
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
    public native boolean copy(CStream _p);
    public native boolean putString(String str);
    public native String getString();
    public native boolean putString(CMem str);
    public native boolean getBinary(CMem out);
    public native byte[] getBinary();
    public native boolean putBinary(CMem bin);
    public native boolean getZeroEndString(CMem str);
    public native String getZeroEndString();
    public native boolean putZeroEndString(String str);
    public native boolean putDouble(double d);
    public native double getDouble();
    public native boolean putBool(boolean b);
    public native boolean getBool();
    public native boolean putInt8(byte i);
    public native byte getInt8();
    public native boolean putInt16(short i);
    public native short getInt16();
    public native boolean putInt32(int i);
    public native int getInt32();
    public native boolean putInt64(long i);
    public native long getInt64();
    public native boolean putUInt8(byte i);
    public native byte getUInt8();
    public native boolean putUInt16(short i);
    public native short getUInt16();
    public native boolean putUInt32(int i);
    public native int getUInt32();
    public native boolean putUInt64(long i);
    public native long getUInt64();
}
