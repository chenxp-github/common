/*##Begin Header##*/
package com.cvtest.common;

import com.jni.common.CMiniBson;
import com.jni.common.CMem;
import com.jni.common.CFileBase;
/*##End Header##*/

public class RpcCallContext{
/*##Begin Members##*/
    private String m_from;
    private int m_callback_id;
    private int m_method;
/*##End Members##*/

    public RpcCallContext()
    {
/*##Begin InitMembers##*/
/*##End InitMembers##*/
    }


/*@@Begin Function getFrom@@*/
    public String getFrom()
    {
        return this.m_from;
    }
/*@@End  Function getFrom@@*/

/*@@Begin Function getCallbackId@@*/
    public int getCallbackId()
    {
        return this.m_callback_id;
    }
/*@@End  Function getCallbackId@@*/

/*@@Begin Function getMethod@@*/
    public int getMethod()
    {
        return this.m_method;
    }
/*@@End  Function getMethod@@*/


/*@@Begin Function setFrom@@*/
    public void setFrom(String _from)
    {
        this.m_from = _from;
    }
/*@@End  Function setFrom@@*/

/*@@Begin Function setCallbackId@@*/
    public void setCallbackId(int _callback_id)
    {
        this.m_callback_id = _callback_id;
    }
/*@@End  Function setCallbackId@@*/

/*@@Begin Function setMethod@@*/
    public void setMethod(int _method)
    {
        this.m_method = _method;
    }
/*@@End  Function setMethod@@*/


    public String toString()
    {
        StringBuilder _buf = new StringBuilder();
        this.toString(_buf);
        return _buf.toString();
    }

    public void toString(StringBuilder _buf)
    {
/*##Begin toString##*/
        _buf.append("from = "+this.m_from+"\n");
        _buf.append("callback_id = "+this.m_callback_id+"\n");
        _buf.append("method = "+this.m_method+"\n");
/*##End toString##*/
    }

    public RpcCallContext clone()
    {
        RpcCallContext _tmp = new RpcCallContext();
/*##Begin clone##*/
        _tmp.setFrom(this.getFrom());
        _tmp.setCallbackId(this.getCallbackId());
        _tmp.setMethod(this.getMethod());
/*##End clone##*/
        return _tmp;
    }


    /*@@ Insert Function Here @@*/
    
}
