package com.jni.common;

public class CFileBase{
    private long __obj = 0;
    private void __dummy(){}


    public void destroy()
    {
        this._gc();
    }

    protected void finalize()
    {
        this.destroy();
    }

    public native int _gc();
    public native boolean isEnd();
    public native byte charAt(long index);
    public native byte getLastChar(boolean empty_included);
    public native byte getc();
    public native byte unGetc();
    public native long getMaxSize();
    public native long getOffset();
    public native long getSize();
    public native long putc(byte ch);
    public native long puts(CFileBase str);
    public native long puts(String str);
    public native boolean readLeftStr(CFileBase file, boolean skip_empty);
    public native boolean readWord(CFileBase file);
    public native boolean readWordWithEmptyChar(CFileBase file);
    public native boolean readWordWithEmptyCharReverse(CFileBase file);
    public native boolean readWordReverse(CFileBase file);
    public native long searchBin(CFileBase file_bin);
    public native long searchStr(String str, boolean case_sensive, boolean word_only);
    public native long seek(long off);
    public native long seekBack(long back_bytes);
    public native long seekEnd();
    public native long split(CFileBase file);
    public native long writeFile(CFileBase file);
    public native long writeFile(CFileBase file, long start, long ws);
    public native long writeToFile(String fn);
    public native long writeToFile(String fn, long start, long wsize);
    public native long writeToFile(CFileBase file, long start, long wsize);
    public native int strCmp(CFileBase str);
    public native int strCmp(String str);
    public native int strICmp(CFileBase str);
    public native int strICmp(String str);
    public native long read(CFileBase buf, long n);
    public native long readZeroEndString(CFileBase str);
    public native long strLen();
    public native long write(CFileBase buf, long n);
    public native boolean isEmptyChar(byte ch);
    public native boolean delete(long del_size);
    public native boolean deleteLast(long bytes);
    public native boolean dump();
    public native boolean endWith(String str, boolean case_sensive, boolean skip_empty);
    public native boolean fillBlock(long fill_size, byte fill_ch);
    public native boolean insert(CFileBase file);
    public native boolean insert(CFileBase file, long start, long size);
    public native boolean isSpChar(byte ch);
    public native boolean loadFile(String fn);
    public native boolean readCStr(CFileBase file);
    public native boolean readLine(CFileBase file);
    public native boolean readLineReverse(CFileBase file);
    public native boolean readQuoteStr(String begin_and_end_char, CFileBase file);
    public native boolean readString(CFileBase file);
    public native boolean readStringReverse(CFileBase file);
    public native boolean replaceStr(String src_str, String des_str, boolean case_sensive, boolean word_only, CFileBase new_file);
    public native boolean reverse(long start, long end);
    public native boolean setChar(long index, byte ch);
    public native boolean setFileName(String fn);
    public native boolean setSize(long ssize);
    public native boolean setSplitChars(String sp_str);
    public native boolean skipEmptyChars();
    public native boolean skipEmptyCharsReverse();
    public native boolean startWith(String str, boolean case_sensive, boolean skip_empty);
    public native boolean strCat(String str);
    public native boolean strLwr();
    public native boolean strUpr();
    public native boolean trim();
    public native String getFileName();
    public native boolean readLineWithEol(CFileBase file, CFileBase eol);
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
    public native boolean putZeroEndString(String str);
    public native String getZeroEndString();
    
    public native String nextString();
    public native String nextWord();
    public native String nextLine();
}
