package com.jni.common;

import com.cvtest.common.Callback;

public class CFileManager{
	
	public static boolean SearchDir(String dir, boolean recursive, Callback onsearchdir_callback_obj)
	{
		return CFileManager.SearchDir(dir, recursive,(Object)onsearchdir_callback_obj);
	}
	 
    public static native boolean DeleteFile(String name);
    public static native String ReadLink(String link_name);
    public static native boolean SaveFile(String path, CFileBase file);
    public static native boolean CreateDir(String path);
    public static native boolean CopySingleFile(String from, String to, long start, long size, long to_start, int mode);
    public static native boolean CopySingleFile(String from, String to, long start, long size, int mode);
    public static native boolean CopySingleFile(String from, String to, int mode);
    public static native boolean IsTextFile(CFileBase file);
    public static native boolean IsTextFile(String fn);
    public static native boolean ConvertEol(CFileBase in, CFileBase out, int mode);
    public static native boolean ConvertEol(String in, String out, int mode);
    public static native boolean IsFileSame(CFileBase src1, CFileBase src2);
    public static native boolean IsFileSame(String fn1, String fn2);
    public static native boolean DeleteDir(String dir);
    private static native boolean SearchDir(String dir, boolean recursive, Object onsearchdir_callback_obj);
}
