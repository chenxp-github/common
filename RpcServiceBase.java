package com.jni.common;

public abstract class RpcServiceBase extends PeerServiceBase{

	public RpcServiceBase()
	{		
		super();
	}
	
	public boolean sendReturnValue(RpcCallContext context, RpcParamBase ret)
	{
		CMiniBson bson = new CMiniBson();
		bson.startDocument();		
		ret.saveBson(bson);		
		bson.endDocument();
		
		Message msg = new Message();
		msg.to = context.getFrom();
		msg.callback_id = context.getCallbackId();
		msg.method = context.getMethod();
		msg.body = bson;
		return this.sendResponse(msg);
	}
	
	public boolean sendPartReturnValue(RpcCallContext context, RpcParamBase ret)
	{
		CMiniBson bson = new CMiniBson();
		bson.startDocument();		
		ret.saveBson(bson);		
		bson.endDocument();
		
		Message msg = new Message();
		msg.to = context.getFrom();
		msg.callback_id = context.getCallbackId();
		msg.method = context.getMethod();
		msg.body = bson;
		return this.sendPartResponse(msg);
	}
		
	
	public boolean sendRequest(RpcParamBase params,int method, int callback_id)
	{
		Message msg = new Message();
		msg.to = this.m_dest_peer_name;
		msg.method = method;
		msg.callback_id = callback_id;
		
		if(params != null)
		{
			CMiniBson bson = new CMiniBson();
			bson.startDocument();
			params.saveBson(bson);
			bson.endDocument();
			msg.body = bson;
		}		
		return this.sendRequest(msg);		
	}
	
}
