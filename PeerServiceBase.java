package com.jni.common;

import java.util.HashMap;

public abstract class PeerServiceBase {
	public final static int  BODY_TYPE_STRING = 0x01<<4;
	public final static int  BODY_TYPE_BSON = 0x02<<4;
	public final static int  BODY_TYPE_RAW = 0x03<<4;
	public final static int  BODY_TYPE_JSON = 0x04<<4;
	public final static int  MSG_TYPE_REQUEST =  0x01;
	public final static int  MSG_TYPE_RESPONSE  = 0x02;
	public final static int  MSG_TYPE_SIGNAL = 0x03;
	public final static int  MSG_TYPE_PART_RESPONSE  = 0x04;
	public final static int  BODY_TYPE_MASK = 0x000000f0;
	public final static int  MSG_TYPE_MASK = 0x0000000f;

	public final static int  PEER_EVENT_GOT_MESSAGE = 9000;
	public final static int  PEER_EVENT_CONNECTED = 9001;
	public final static int  PEER_EVENT_DISCONNECTED = 9002;
	public final static int  PEER_EVENT_STOPPED= 9003;
	public final static int  PEER_EVENT_CAN_FETCH_MESSAGE = 9004;
	
	private CServerSidePeer m_server_side_peer;
	private CMessagePeer m_client_side_peer;
	private boolean m_is_server_side = false;
	private CallbackMap m_callback_map = new CallbackMap();
	private Callback m_OnMessage;
	String m_dest_peer_name = "";
	
	public class Message{
		public int event;
		public int method;
		public String from;
		public int callback_id;
		public int flags;
		public int body_type;
		public int msg_type;
		public String to;
		public CMiniBson body;
	}	
	
	public abstract void onRequest(Message msg);
	
	public PeerServiceBase()
	{		
	}
	
	public void destroy()
	{
		this.m_callback_map.destroy();
		if(this.m_server_side_peer!=null)
		{
			this.m_server_side_peer.destroy();
			this.m_server_side_peer = null;
		}
		if(this.m_client_side_peer != null)
		{
			this.m_client_side_peer.destroy();
			this.m_client_side_peer = null;
		}
	}
	
	public void initServerSidePeer()
	{
		this.m_server_side_peer = new CServerSidePeer();
		this.m_is_server_side = true;
	}
	
	public void initClientSidePeer(String server, int port)
	{
		this.m_client_side_peer = new CMessagePeer();
		this.m_client_side_peer.setServer(server);
		this.m_client_side_peer.setPort(port);
		this.m_is_server_side = false;
	}
	
	public void start()
	{
		final PeerServiceBase self = this;

		m_OnMessage = new Callback(){
			public Object run() {
				self.innerOnMessage(this.params);
				return null;
			}
        	
        };
		
		if(this.m_is_server_side)
		{
			this.m_server_side_peer.start();
	        this.m_server_side_peer.startFetchMessageTask();
	        this.m_server_side_peer.setCanFetchMessage(true);
	        this.m_server_side_peer.setOnMessage(m_OnMessage);
		}
		else
		{
			this.m_client_side_peer.start();
			this.m_client_side_peer.setOnMessage(m_OnMessage);
		}
	}
	
	public void innerOnMessage(HashMap<String,Object> params)
	{	
		Message msg = new Message();
		
		msg.event = (Integer)params.get("event");

		if(msg.event == PEER_EVENT_CONNECTED)
		{
			this.onSocketConnected();
		}
		else if(msg.event == PEER_EVENT_DISCONNECTED)
		{
			this.onSocketDisconnected();
		}
		else if(msg.event == PEER_EVENT_GOT_MESSAGE)
		{			
			msg.method = (Integer) params.get("method");
			msg.from = (String) params.get("from");
			msg.callback_id = (Integer) params.get("callback_id");
			msg.flags = (Integer)params.get("flags");
			msg.body_type = (Integer)params.get("body_type");
			msg.msg_type = (Integer)params.get("msg_type");
			
			if(msg.body_type == BODY_TYPE_BSON)
			{
				msg.body =(CMiniBson)params.get("body");
				this.onMessage(msg);
			}
		}
	}
	
	public void onMessage(Message msg)
	{
		if(msg.msg_type == MSG_TYPE_REQUEST)
		{
			this.setDestPeerName(msg.from);
			this.onRequest(msg);
		}
		else if(msg.msg_type == MSG_TYPE_RESPONSE)
		{
			this.onResponse(msg);
		}
		else if(msg.msg_type == MSG_TYPE_PART_RESPONSE)
		{
			this.onPartResponse(msg);
		}
	}
	
	public void setName(String name)
	{
		if(this.m_is_server_side)
		{
			this.m_server_side_peer.setName(name);
		}
		else
		{
			this.m_client_side_peer.setName(name);
		}
	}
	
	
	public boolean sendMessage(Message msg)
	{
		if(this.m_is_server_side) 
		{
			return this.m_server_side_peer.sendMessage(
				msg.to,msg.method,msg.callback_id,msg.flags,msg.body
			);
		}
		else
		{
			return this.m_client_side_peer.sendMessage(
				msg.to,msg.method,msg.callback_id,msg.flags,msg.body
			);
		}		
	}	
	
	public void connect(String name)
	{
		if(this.m_is_server_side) 
		{
			this.m_server_side_peer.connect(name);
		}
		else
		{
			this.m_client_side_peer.connect(name);
		}
	}
	
	public String getName()
	{
		if(this.m_is_server_side) 
		{
			return this.m_server_side_peer.getName();
		}
		else
		{
			return this.m_client_side_peer.getName();
		}
	}
	
	public boolean isConnected()
	{
		if(this.m_is_server_side)
		{
			return true;
		}
		else
		{
			return this.m_client_side_peer.isConnected();
		}
	}
	
	public int getSendingQueueLength()
	{
		if(this.m_is_server_side)
		{
			return this.m_server_side_peer.getSendingQueueLength();
		}
		else
		{
			return this.m_client_side_peer.getSendingQueueLength();
		}
	}
	
	public void setMaxRetries(int max)
	{
		if(!this.m_is_server_side)
		{
			this.m_client_side_peer.setMaxRetries(max);
		}
	}
	
	public void onResponse(Message msg)
	{
		Callback cb = this.m_callback_map.get(msg.callback_id);
		cb.put("result", Consts.E_OK);
		cb.put("value",msg.body);
		cb.run();		
		this.m_callback_map.remove(msg.callback_id);
	}
	
	public void onPartResponse(Message msg)
	{
		Callback cb = this.m_callback_map.get(msg.callback_id);
		cb.put("result", Consts.E_PART_OK);
		cb.put("value",msg.body);
		cb.run();
	}
	
	public int addCallback(Callback cb, int timeout)
	{
		return this.m_callback_map.put(cb, timeout);
	}
	
	public void setDestPeerName(String name)
	{
		this.m_dest_peer_name = name;
	}
	
	public boolean sendResponse(Message msg)
	{	    
	    msg.flags |= (MSG_TYPE_RESPONSE |BODY_TYPE_BSON);
	    return this.sendMessage(msg);
	}
	
	public boolean sendPartResponse(Message msg)
	{	    
	    msg.flags |= (MSG_TYPE_PART_RESPONSE|BODY_TYPE_BSON);
	    return this.sendMessage(msg);
	}	
	
	public boolean sendRequest(Message msg)
	{	    
	    msg.flags |= (MSG_TYPE_REQUEST|BODY_TYPE_BSON);
	    return this.sendMessage(msg);
	}	
	
	public RpcCallContext getCallContext(Message msg)
	{
		RpcCallContext context = new RpcCallContext();
    	context.setFrom(msg.from);
    	context.setMethod(msg.method);
    	context.setCallbackId(msg.callback_id);
    	return context;
	}
	
	public void onSocketConnected()
	{
	}
	
	public void onSocketDisconnected()
	{
		this.m_callback_map.cancelAll();
	}
	
}

