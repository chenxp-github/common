package com.jni.common;

public class CMiniBson{
    private long __obj = 0;
    private void __dummy(){}

    public CMiniBson()
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
    
    public static String BsonString(String str)
    {
    	if(str == null)return "";
    	return str;
    }

    public native int _gc();
    public native boolean _new();
    public native boolean loadRawBuf(byte[] data);
    public native boolean loadRawBuf(CMem buf);
    public native boolean copy(CMiniBson p);
    public native boolean endDocument();
    public native boolean startDocument();
    public native boolean loadBson(CFileBase data);
    public native boolean loadBson(String fn);
    public native boolean loadBson();
    public native boolean isEnd();
    public native boolean getDocument(String name, CMiniBson doc);
    public native long startDocument(CMem name);
    public native long startDocument(String name);
    public native boolean endDocument(long offset);
    public native boolean resetPointer();
    public native boolean getBinary(String name, CMem bin);
    public native boolean getString(String name, CMem str);
    public native String getString(String name);
    public native boolean getBoolean(String name);
    public native boolean skipString();
    public native double getDouble(String name);
    public native long getInt64(String name);
    public native String toJson(boolean bracket);
    public native int getInt32(String name);
    public native int getUInt32(String name);
    public native short getInt16(String name);
    public native short getUInt16(String name);
    public native byte getInt8(String name);
    public native byte getUInt8(String name);
    public native boolean putDouble(String name, double d);
    public native boolean putDouble(CMem name, double d);
    public native boolean putBoolean(CMem name, boolean b);
    public native boolean putBoolean(String name, boolean b);
    public native boolean putDocument(String name, CMiniBson bson);
    public native int getDocumentSize();
    public native boolean putDocument(CMem name, CMiniBson bson);
    public native boolean writeInt64(long i);
    public native boolean putInt64(CMem name, long i);
    public native boolean putInt64(String name, long i);
    public native boolean putBinary(String name, CFileBase bin);
    public native boolean putBinary(CMem name, CFileBase bin);
    public native boolean putBinary(String name, byte[] bin);
    public native boolean putBinary(CMem name, byte[] bin);
    public native boolean putBinary(String name, short[] bin);
    public native boolean putBinary(CMem name, short[] bin);
    public native boolean putBinary(String name, int[] bin);
    public native boolean putBinary(CMem name, int[] bin);
    public native boolean putBinary(String name, long[] bin);
    public native boolean putBinary(CMem name, long[] bin);
    public native boolean putBinary(String name, float[] bin);
    public native boolean putBinary(CMem name, float[] bin);
    public native boolean putBinary(String name, double[] bin);
    public native boolean putBinary(CMem name, double[] bin);
    public native boolean putString(String name, String str);
    public native boolean putString(String name, CMem str);
    public native boolean putString(CMem name, CMem str);
    public native boolean putInt32(String name, int i);
    public native boolean putUInt32(String name, int i);
    public native boolean putInt16(String name, short i);
    public native boolean putUInt16(String name, short i);
    public native boolean putInt8(String name, byte i);
    public native boolean putUInt8(String name, byte i);
    public native boolean putInt32(CMem name, int i);
    public native boolean putUInt32(CMem name, int i);
    public native boolean setRawBuf(CMem buf);
    public native CMem getRawData();
    public native long getPointerPos();
    public native boolean setPointerPos(long pos);
    public native long startArray(CMem name);
    public native long startArray(String name);
    public native boolean endArray(long offset, int array_len);
    public native int getArray(String name, CMiniBson doc);
    public native boolean peekNext(CStream out);
    public native byte[] getBinaryByteArray(String name);
    public native short[] getBinaryShortArray(String name);
    public native int[] getBinaryIntArray(String name);
    public native long[] getBinaryLongArray(String name);
    public native float[] getBinaryFloatArray(String name);
    public native double[] getBinaryDoubleArray(String name);
    public native boolean putFloat(String name, float f);
    public native float getFloat(String name);
}
