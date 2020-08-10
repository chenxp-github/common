#include "asn_to_json.h"
#include "syslog.h"
#include "OCTET_STRING.h"
#include "IA5String.h"
#include "BIT_STRING.h"
#include "asn_helper.h"
#include "constr_SET_OF.h"
#include "asn_SET_OF.h"

int asn_to_json_alloc_id()
{
    static int id = 1;
    return id++;
}

#define SET_PENDING_COMMA()\
(*pending_comma)=1\

#define PUT_COMMA() do{\
if(*pending_comma)\
{\
    *pending_comma=0;\
    out->Putc(',');\
    out->Eol();\
}}while(0)\

static status_t asn_choice_to_json(ASN_TO_JSON_PARAM_DEF)
{
    ASSERT(obj_ptr);

    out->Printf("{");
    out->Eol();
    out->IncLogLevel(1);    
   
    out->Log("\"present\" : %d,",descriptor->GetAsnChoicePresent(obj_ptr));

    int index = descriptor->GetAsnChoiceMemberIndex(obj_ptr);
    if(index >= 0)
    {
        CAsnMember m;
        descriptor->GetMember(index,&m);
        CAsnDescriptor d;
        m.GetDescriptor(&d);
        if(!asn_to_json(&d,&m,(char*)obj_ptr+m.GetOffset(),level+1,out,pending_comma))
        {
            return ERROR;
        }       
    }
    
    out->IncLogLevel(-1);    
    out->Eol();out->Tab();
    out->Printf("}");
    SET_PENDING_COMMA();   
    return OK;
}

static status_t asn_sequence_to_json(ASN_TO_JSON_PARAM_DEF)
{
    ASSERT(obj_ptr);

    out->Printf("{");
    out->Eol();
    out->IncLogLevel(1);

    for(int i = 0; i < descriptor->GetMembersCount(); i++)
    {
        CAsnMember m;
        descriptor->GetMember(i,&m);
        CAsnDescriptor d;
        m.GetDescriptor(&d);        
        if(!asn_to_json(&d,&m,(char*)obj_ptr+m.GetOffset(),level+1,out,pending_comma))
        {
            return ERROR;
        }
    }

    out->IncLogLevel(-1);  
    out->Eol();out->Tab();
    out->Printf("}"); 
    SET_PENDING_COMMA();
    return OK;
}

static status_t asn_integer_to_json(ASN_TO_JSON_PARAM_DEF)
{
    ASSERT(obj_ptr);
    long *p = (long*)obj_ptr;
    ASSERT(p);
    out->Printf("%d",*p);
    SET_PENDING_COMMA();
    return OK;
}

static status_t asn_octet_string_to_json(ASN_TO_JSON_PARAM_DEF)
{
    ASSERT(obj_ptr);
    OCTET_STRING_t *str = (OCTET_STRING_t*)obj_ptr;

    out->Printf("\"octet_string:");
    for(unsigned int i = 0; i < str->size; i++)
    {
        out->Printf("%02x",str->buf[i]);
    }
    out->Printf("\"");
    SET_PENDING_COMMA();
    return OK;
}

static status_t asn_ia5_string_to_json(ASN_TO_JSON_PARAM_DEF)
{
    ASSERT(obj_ptr);
    IA5String_t *str = (IA5String_t*)obj_ptr;

    out->Printf("\"ia5_string:");
    for(unsigned int i = 0; i < str->size; i++)
    {
        out->Printf("%02x",str->buf[i]);
    }
    out->Printf("\"");
    SET_PENDING_COMMA();
    return OK;
}

static status_t asn_enumerated_to_json(ASN_TO_JSON_PARAM_DEF)
{
    ASSERT(obj_ptr);
    long *p = (long*)obj_ptr;
    LOCAL_MEM(mem);
    descriptor->GetAsnEnumeratedValue(*p,&mem);
    out->Printf("%d",*p);
    SET_PENDING_COMMA();
    return OK;
}

static status_t asn_bit_string_to_json(ASN_TO_JSON_PARAM_DEF)
{
    ASSERT(obj_ptr);
    BIT_STRING_t *str = (BIT_STRING_t*)obj_ptr;
    out->Printf("\"bit_string:");
    int bits = str->size*8 - str->bits_unused;
    for(int i = 0; i < bits; i++)
    {
        uint8_t v;
        BIT_STRING_GET_BIT(*str,i,v);
        out->Printf("%d",v);
    }
    out->Printf("\"");
    SET_PENDING_COMMA();
    return OK;
}

typedef A_SET_OF(int) SET_OF_INT;

static status_t asn_sequence_of_to_json(ASN_TO_JSON_PARAM_DEF)
{
    ASSERT(obj_ptr);
    ASSERT(descriptor->GetMembersCount() == 1);

    out->Printf("[");
    out->Eol();
    out->IncLogLevel(1);
    CAsnMember m;
    descriptor->GetMember(0,&m);
    CAsnDescriptor d;
    m.GetDescriptor(&d);
    
    SET_OF_INT *list = (SET_OF_INT*)obj_ptr;

    for(int i = 0; i < list->count; i++)
    {
        if(!asn_to_json(&d,&m,(char*)list->array[i],level+1,out,pending_comma))
            return ERROR;
    }
    out->IncLogLevel(-1);
    out->Eol();out->Tab();
    out->Printf("]");  
    SET_PENDING_COMMA();
    return OK;
}


#define CALL_LIST descriptor,member,obj_ptr,level+1,out,pending_comma
status_t asn_to_json(ASN_TO_JSON_PARAM_DEF)
{
    ASSERT(descriptor && out && obj_ptr && pending_comma);
    ASSERT(descriptor->Get());

    if(member && member->IsOptional())
    {
        ASSERT(member->IsPointer());
        void **p = (void**)obj_ptr;        
        obj_ptr = *p;
        if(!obj_ptr){return OK;}
    }
    
    if(member)
    {        
        PUT_COMMA();
        out->Tab();
        if(member->GetName()[0])
        {            
            out->Printf("\"%s\" : ",member->GetName());
        }
    }

    if(descriptor->IsAsnChoice())
    {        
        asn_choice_to_json(CALL_LIST);
    }
    else if(descriptor->IsAsnSequence())
    {
        asn_sequence_to_json(CALL_LIST);
    }
    else if(descriptor->IsAsnInteger())
    {
        asn_integer_to_json(CALL_LIST);
    }
    else if(descriptor->IsAsnOctetString())
    {
        asn_octet_string_to_json(CALL_LIST);
    }
    else if(descriptor->IsAsnEnumerated())
    {
        asn_enumerated_to_json(CALL_LIST);
    }
    else if(descriptor->IsAsnBitString())
    {
        asn_bit_string_to_json(CALL_LIST);
    }
    else if(descriptor->IsAsnSequenceOf())
    {
        asn_sequence_of_to_json(CALL_LIST);
    }
    else if(descriptor->IsAsnIA5String())
    {
        asn_ia5_string_to_json(CALL_LIST);
    }
    else 
    {
        XLOG(LOG_MODULE_USER,LOG_LEVEL_ERROR,
            "unknown asn type: %s",descriptor->GetName());
        return ERROR;
    }
    return OK;
}

