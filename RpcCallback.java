package com.cvtest.common;

import com.jni.common.CMiniBson;

public abstract class RpcCallback extends Callback{
	public RpcParamBase ret;
	
	public Object run()
	{	
		int result = (Integer)this.params.get("result");
		if(ret != null)
		{
			CMiniBson bson = (CMiniBson)this.params.get("value");
			if(bson != null)
			{
				ret.loadBson(bson);			
			}
		}
		this.run(result,ret);
		return null;
	}
	
	public abstract void run(int result,RpcParamBase value);
}



