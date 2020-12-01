package com.cvtest.common;

import com.jni.common.CMem;
import com.jni.common.CMiniBson;

public abstract class RpcParamBase {
	public abstract boolean saveBson(CMiniBson bson);
	public abstract boolean saveBson(CMem mem);
	public abstract boolean loadBson(CMiniBson bson);
	public abstract boolean loadBson(CMem mem);
}

