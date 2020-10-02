#include "asn_to_js_tree.h"
#include "syslog.h"
#include "OCTET_STRING.h"
#include "IA5String.h"
#include "BIT_STRING.h"
#include "asn_helper.h"
#include "constr_SET_OF.h"
#include "asn_SET_OF.h"

int asn_to_js_tree_alloc_id()
{
    static int id = 1;
    return id++;
}

static status_t asn_choice_to_js_tree(ASN_TO_JS_TREE_PARAM_DEF)
{
    ASSERT(obj_ptr);
    out->Log("native_type:\"CHOICE\",");
    out->Log("children:[");
    out->IncLogLevel(1);
    out->Log("{");
    out->IncLogLevel(1);    
    out->Log("type:\"INTEGER\",");
    out->Log("id:%d,",asn_to_js_tree_alloc_id());
    out->Log("name:\"present\",");
    out->Log("value:%d,",descriptor->GetAsnChoicePresent(obj_ptr));
    out->IncLogLevel(-1);
    out->Log("},");             

    out->Log("{");
    out->IncLogLevel(1);

    int index = descriptor->GetAsnChoiceMemberIndex(obj_ptr);
    if(index >= 0)
    {
        CAsnMember m;
        descriptor->GetMember(index,&m);
        CAsnDescriptor d;
        m.GetDescriptor(&d);
        out->Log("id:%d,",asn_to_js_tree_alloc_id());
        out->Log("name:\"%s\",",m.GetName());
        if(!asn_to_js_tree(&d,&m,(char*)obj_ptr+m.GetOffset(),level+1,out))
        {
            return ERROR;
        }
    }

    out->IncLogLevel(-1);   
    out->Log("},");             

    out->IncLogLevel(-1);   
    out->Log("],");  

    return OK;
}

static status_t asn_sequence_to_js_tree(ASN_TO_JS_TREE_PARAM_DEF)
{
    ASSERT(obj_ptr);
    out->Log("native_type:\"SEQUENCE\",");
    out->Log("children:[");
    out->IncLogLevel(1);

    for(int i = 0; i < descriptor->GetMembersCount(); i++)
    {
        out->Log("{");
        out->IncLogLevel(1);
        CAsnMember m;
        descriptor->GetMember(i,&m);
        CAsnDescriptor d;
        m.GetDescriptor(&d);        
        out->Log("optional:%d,",m.IsOptional());
        out->Log("id:%d,",asn_to_js_tree_alloc_id());
        out->Log("name:\"%s\",",m.GetName());
        if(!asn_to_js_tree(&d,&m,(char*)obj_ptr+m.GetOffset(),level+1,out))
        {
            return ERROR;
        }
        out->IncLogLevel(-1);   
        out->Log("}");  
        out->Log(",");
    }

    out->IncLogLevel(-1);   
    out->Log("],");  
    return OK;
}

static status_t asn_integer_to_js_tree(ASN_TO_JS_TREE_PARAM_DEF)
{
    ASSERT(obj_ptr);
    long *p = (long*)obj_ptr;
    out->Log("native_type:\"INTEGER\",");
    out->Log("value:%d,",*p);
    return OK;
}

static status_t asn_octet_string_to_js_tree(ASN_TO_JS_TREE_PARAM_DEF)
{
    ASSERT(obj_ptr);
    OCTET_STRING_t *str = (OCTET_STRING_t*)obj_ptr;

    out->Log("native_type:\"OCTET_STRING\",");
    out->Log(NULL);
    out->Printf("value:\"");
    for(unsigned int i = 0; i < str->size; i++)
    {
        if(i > 0)out->Puts(" ");
        out->Printf("%02x",str->buf[i]);
    }
    out->Printf("\",");
    out->Log("");

    return OK;
}

static status_t asn_ia5_string_to_js_tree(ASN_TO_JS_TREE_PARAM_DEF)
{
    ASSERT(obj_ptr);
    IA5String_t *str = (IA5String_t*)obj_ptr;

    out->Log("native_type:\"IA5_STRING\",");
    out->Log(NULL);
    out->Printf("value:\"");

    CMem tmp;
    tmp.Init();
    tmp.SetRawBuf(str->buf,str->size,true);
    tmp.Seek(0);
    
    for(unsigned int i = 0; i < str->size; i++)
    {
        if(i > 0)out->Puts(" ");
        out->Printf("%02x",str->buf[i]);
    }
    out->Printf("\",");
    out->Log("");

    return OK;
}

static status_t asn_enumerated_to_js_tree(ASN_TO_JS_TREE_PARAM_DEF)
{
    ASSERT(obj_ptr);
    long *p = (long*)obj_ptr;
    out->Log("native_type:\"ENUMERATED\",");

    LOCAL_MEM(mem);
    descriptor->GetAsnEnumeratedValue(*p,&mem);

    out->Log("value:\"%d(%s)\",",*p,mem.CStr());
    return OK;
}

static status_t asn_bit_string_to_js_tree(ASN_TO_JS_TREE_PARAM_DEF)
{
    ASSERT(obj_ptr);
    BIT_STRING_t *str = (BIT_STRING_t*)obj_ptr;
    out->Log("native_type:\"BIT_STRING\",");
    out->Log(NULL);
    out->Printf("value:\"");

    int bits = str->size*8 - str->bits_unused;
    for(int i = 0; i < bits; i++)
    {
        uint8_t v;
        BIT_STRING_GET_BIT(*str,i,v);
        out->Printf("%d",v);
    }
    out->Printf("\",");
    out->Log("");    
    return OK;
}

typedef A_SET_OF(int) SET_OF_INT;

static status_t asn_sequence_of_to_js_tree(ASN_TO_JS_TREE_PARAM_DEF)
{
    ASSERT(obj_ptr);
    ASSERT(descriptor->GetMembersCount() == 1);

    out->Log("native_type:\"SEQUENCE_OF\",");
    out->Log("children:[");
    out->IncLogLevel(1);

    CAsnMember m;
    descriptor->GetMember(0,&m);
    CAsnDescriptor d;
    m.GetDescriptor(&d);
    
    SET_OF_INT *list = (SET_OF_INT*)obj_ptr;

    for(int i = 0; i < list->count; i++)
    {
        out->Log("{");
        out->IncLogLevel(1);
        out->Log("id:%d,",asn_to_js_tree_alloc_id());
        out->Log("name:\"%d\",",i);
        if(!asn_to_js_tree(&d,&m,(char*)list->array[i],level+1,out))
            return ERROR;
        out->IncLogLevel(-1);   
        out->Log("}");  
        out->Log(",");
    }
    out->IncLogLevel(-1);   
    out->Log("],");  
    return OK;
}


#define CALL_LIST descriptor,member,obj_ptr,level+1,out
status_t asn_to_js_tree(ASN_TO_JS_TREE_PARAM_DEF)
{
    ASSERT(descriptor && out && obj_ptr);
    ASSERT(descriptor->Get());

    if(member && member->IsOptional())
    {
        ASSERT(member->IsPointer());
        void **p = (void**)obj_ptr;        
        obj_ptr = *p;
        if(!obj_ptr)return OK;
    }    
    out->Log("type:\"%s\",",descriptor->GetName());
    if(descriptor->IsAsnChoice())
    {        
        asn_choice_to_js_tree(CALL_LIST);
    }
    else if(descriptor->IsAsnSequence())
    {
        asn_sequence_to_js_tree(CALL_LIST);
    }
    else if(descriptor->IsAsnInteger())
    {
        asn_integer_to_js_tree(CALL_LIST);
    }
    else if(descriptor->IsAsnOctetString())
    {
        asn_octet_string_to_js_tree(CALL_LIST);
    }
    else if(descriptor->IsAsnEnumerated())
    {
        asn_enumerated_to_js_tree(CALL_LIST);
    }
    else if(descriptor->IsAsnBitString())
    {
        asn_bit_string_to_js_tree(CALL_LIST);
    }
    else if(descriptor->IsAsnSequenceOf())
    {
        asn_sequence_of_to_js_tree(CALL_LIST);
    }
    else if(descriptor->IsAsnIA5String())
    {
        asn_ia5_string_to_js_tree(CALL_LIST);
    }
    else 
    {
        XLOG(LOG_MODULE_USER,LOG_LEVEL_ERROR,
            "unknown asn type: %s",descriptor->GetName());
        return ERROR;
    }
    return OK;
}

