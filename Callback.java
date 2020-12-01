package com.cvtest.common;

import java.util.HashMap;
import java.util.Iterator;
import java.util.Map.Entry;

import android.util.Log;

public abstract class Callback {	
	public HashMap<String,Object> params = new HashMap<String,Object>();	
	
	public abstract Object run();
	
	
	public Object get(String key)
	{
		return params.get(key);
	}
	
	public Object put(String key, Object value)
	{
		return params.put(key, value);
	}
	
	public void dump()
	{
		Iterator<Entry<String, Object>> iter = params.entrySet().iterator();
		while (iter.hasNext())
		{
			Entry<String, Object> entry = iter.next();
			Object key = entry.getKey();
			Object val = entry.getValue();			
			Log.e("callback",key+" = "+val);
		}
	}
	
	public void clear()
	{
		this.params.clear();
	}
}



