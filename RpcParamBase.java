package com.jni.common;

public abstract class RpcParamBase {
	public abstract boolean saveBson(CMiniBson bson);
	public abstract boolean saveBson(CMem mem);
	public abstract boolean loadBson(CMiniBson bson);
	public abstract boolean loadBson(CMem mem);
}

