package com.jni.common;

import java.util.HashMap;
import java.util.Iterator;
import java.util.Timer;
import java.util.TimerTask;
import java.util.Map.Entry;

public class CallbackMap {
	public class Item {
		public Callback m_callback;
		public int m_life_time;
		public int m_timeout;
		public int m_flags;
		public int m_id;
	}

	private HashMap<Integer,Item> m_map = new HashMap<Integer,Item>();
	private int m_auto_id = 0;
	private Timer m_auto_clear_timer;
	
	CallbackMap()
	{
	}
	
	public void destroy()
	{
		this.cancelAll();
	}
	
	private int allocId()
	{
		this.m_auto_id++;
		if(this.m_auto_id == 0)
			this.m_auto_id++;		
		return this.m_auto_id;
	}
	
	public int put(Callback cb, int timeout)
	{
		if(timeout == 0)timeout = 10*1000;		
		Item item = new Item();
		item.m_id = this.allocId();
		item.m_callback = cb;
		item.m_timeout = timeout;
		item.m_flags = 0;
		item.m_life_time = 0;				
		m_map.put(item.m_id, item);		
		this.startAutoClearTimer();
		return item.m_id;		
	}
	
	public void stopAutoClearTimer()
	{
		if(this.m_auto_clear_timer != null)
		{
			this.m_auto_clear_timer.cancel();
			this.m_auto_clear_timer = null;
		}
	}
	
	public void startAutoClearTimer()
	{
		if(this.m_auto_clear_timer != null)
			return;
		final CallbackMap self = this;
		this.m_auto_clear_timer = new Timer();
		this.m_auto_clear_timer.schedule(new TimerTask(){
			@Override
			public void run() {
				self.onAutoClear(1000);				
			}		
		}, 0, 1000);
	}
	
	public void onAutoClear(int interval)
	{
		Iterator<Entry<Integer, Item>> iter = this.m_map.entrySet().iterator();
		while (iter.hasNext())
		{
			Entry<Integer, Item> entry = iter.next();
			
			Item item = entry.getValue();
			item.m_life_time += interval;
			
			if(item.m_timeout > 0 && item.m_life_time > item.m_timeout)
			{
				item.m_callback.clear();
				item.m_callback.put("result",Consts.E_TIMEOUT);
				item.m_callback.run();				
				iter.remove();
			}	
		}
		
		if(this.m_map.size() == 0)
		{
			this.stopAutoClearTimer();
		}
	}
	
	public Callback get(int id)
	{
		Item item = this.m_map.get(id);
		if(item == null)return null;
		return item.m_callback;
	}
	
	public void remove(int id)
	{
		this.m_map.remove(id);
	}
	
	public void cancelAll()
	{
		Iterator<Entry<Integer, Item>> iter = this.m_map.entrySet().iterator();
		while (iter.hasNext())
		{
			Entry<Integer, Item> entry = iter.next();			
			Item item = entry.getValue();
			item.m_callback.clear();
			item.m_callback.put("result",Consts.E_TIMEOUT);
			item.m_callback.run();
		}
		
		this.m_map.clear();
		this.stopAutoClearTimer();
	}
		
}
