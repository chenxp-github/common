#include "lua_to_asn.h"
#include "xlog.h"
#include "mem_tool.h"
#include "INTEGER.h"
#include "OCTET_STRING.h"
#include "BIT_STRING.h"
#include "IA5String.h"
#include "asn_SET_OF.h"
#include "misc.h"
#include "asn_helper.h"
#include "asn_SEQUENCE_OF.h"

//LOGE("alloc:%s:%d:%s",descriptor->GetName(),sizeof(type)*items,#ptr);\

#define ASN_ZALLOC(ptr,type,items) do{\
ZALLOC(ptr,type,items);\
}while(0)\

#define CREATE_ASN_MEMBER(m) do{\
CAsnDescriptor d;\
m.GetDescriptor(&d);\
int top = lua_gettop(L);\
lua_pushstring(L,m.GetName());\
lua_gettable(L,idx);\
int new_top = top+1;\
if(!lua_isnoneornil(L,new_top))\
{\
    uint8_t *new_out = NULL;\
    if(!m.IsOptional())\
    {\
        new_out = (*out) + m.GetOffset();\
    }\
    if(!lua_to_asn(&d,L,new_top,&new_out))\
    {\
        lua_settop(L,top);\
        return ERROR;\
    }\
    if(m.IsOptional())\
    {\
        memcpy((*out) + m.GetOffset(),&new_out,sizeof(new_out));\
    }\
}\
lua_settop(L,top);\
}while(0)\

status_t lua_to_asn_choice(LUA_TO_ASN_PARAM_DEF)
{
    ASSERT(descriptor && L && out);
    asn_CHOICE_specifics_t *specifics = descriptor->GetChoiceSpecifics();
    ASSERT(specifics);
    int struct_size = specifics->struct_size;
    ASSERT(struct_size > 0);

    if((*out) == NULL)
    {        
        ASN_ZALLOC((*out),uint8_t,struct_size);
    }

    int top = lua_gettop(L);
    lua_pushstring(L,"present");
    lua_gettable(L,idx);    
    top++;

    ASSERT(lua_isinteger(L,top));
    int32_t present = (int32_t)lua_tointeger(L,top);
    lua_pop(L,1);

    ASSERT(specifics->pres_size == sizeof(present));
    memcpy((*out)+specifics->pres_offset,&present,sizeof(present));

    int index = descriptor->GetAsnChoiceMemberIndex(present);

    if(index >= 0)
    {
        CAsnMember m;
        descriptor->GetMember(index,&m);
        CREATE_ASN_MEMBER(m);
    }
    return OK;
}

status_t lua_to_asn_sequence(LUA_TO_ASN_PARAM_DEF)
{
    ASSERT(descriptor && L && out);

    asn_SEQUENCE_specifics_t *specifics = descriptor->GetSequenceSpecifics();
    ASSERT(specifics);
    int struct_size = specifics->struct_size;
    ASSERT(struct_size > 0);

    if((*out) == NULL)
    {
        ASN_ZALLOC((*out),uint8_t,struct_size);        
    }

    for(int i = 0; i < descriptor->GetMembersCount(); i++)
    {
        CAsnMember m;
        descriptor->GetMember(i,&m);
        CREATE_ASN_MEMBER(m);
    }

    return OK;
}

status_t lua_to_asn_integer(LUA_TO_ASN_PARAM_DEF)
{
    if((*out) == NULL)
    {
        ASN_ZALLOC((*out),uint8_t,sizeof(long));
    }

    long value = (long)lua_tointeger(L,idx);
    memcpy((*out),&value,sizeof(long));
    return OK;
}

status_t lua_to_asn_octet_string(LUA_TO_ASN_PARAM_DEF)
{
    if((*out) == NULL)
    {        
        ASN_ZALLOC((*out),uint8_t,sizeof(OCTET_STRING_t));
    }

    size_t len = 0;
    const char *str = lua_tolstring(L,idx,&len);
    const char *key = "octet_string:";
    size_t key_len = 13;

    ASSERT(len > key_len);
    ASSERT(memcmp(str,key,key_len) == 0);

    int bin_size = ((len-key_len)+1)/2;
    
    OCTET_STRING_t v; 
    ALLOC_OCTET_STRING(v,bin_size);
    hex_str_to_bin(str+key_len,len-key_len,v.buf,v.size);

    memcpy((*out),&v,sizeof(OCTET_STRING_t));
    return OK;
}

status_t lua_to_asn_bit_string(LUA_TO_ASN_PARAM_DEF)
{
    if((*out) == NULL)
    {
        ASN_ZALLOC((*out),uint8_t,sizeof(BIT_STRING_t));
    }

    size_t len = 0;
    const char *str = lua_tolstring(L,idx,&len);
    const char *key = "bit_string:";
    size_t key_len = 11;

    ASSERT(len > key_len);
    ASSERT(memcmp(str,key,key_len) == 0);

    int bits = len-key_len;
    const char* bits_str = str+key_len;

    BIT_STRING_t v; 
    ALLOC_BIT_STRING(v,bits);

    for(int i = 0; i < bits; i++)
    {
        BIT_STRING_SET_BIT(v,i,bits_str[i]!='0');
    }

    memcpy((*out),&v,sizeof(BIT_STRING_t));
    return OK;
}

status_t lua_to_asn_ia5_string(LUA_TO_ASN_PARAM_DEF)
{
    if((*out) == NULL)
    {
        ASN_ZALLOC((*out),uint8_t,sizeof(OCTET_STRING_t));
    }

    size_t len = 0;
    const char *str = lua_tolstring(L,idx,&len);
    const char *key = "ia5_string:";
    size_t key_len = 11;

    ASSERT(len > key_len);
    ASSERT(memcmp(str,key,key_len) == 0);

    int bin_size = ((len-key_len)+1)/2;
    
    OCTET_STRING_t v; 
    ALLOC_IA5_STRING(v,bin_size);
    hex_str_to_bin(str+key_len,len-key_len,v.buf,v.size);

    memcpy((*out),&v,sizeof(OCTET_STRING_t));
    return OK;
}

status_t lua_to_asn_enumerated(LUA_TO_ASN_PARAM_DEF)
{
    if((*out) == NULL)
    {        
        ASN_ZALLOC((*out),uint8_t,sizeof(long));
    }

    long value = (long)lua_tointeger(L,idx);
    memcpy((*out),&value,sizeof(long));
    return OK;
}

static int get_lua_list_len(lua_State *L, int idx)
{
    int len = 0;
    int top = lua_gettop(L);

    lua_pushnil(L);
    while (lua_next(L, idx) != 0) 
    {
        if(strcmp(lua_typename(L, lua_type(L, -2)),"number") == 0)
        {
            len++;
        }
        lua_pop(L, 1);
    }

    lua_settop(L,top);
    return len;
}

typedef A_SET_OF(int) SET_OF_INT;

status_t lua_to_asn_sequence_of(LUA_TO_ASN_PARAM_DEF)
{
    asn_SET_OF_specifics_t *specifics = descriptor->GetSetOfSpecifics();
    ASSERT(specifics);
    int struct_size = specifics->struct_size;
    ASSERT(struct_size > 0);

    if((*out) == NULL)
    {
        ASN_ZALLOC((*out),uint8_t,struct_size);        
    }

    ASSERT(descriptor->GetMembersCount() == 1);
    CAsnMember m;
    descriptor->GetMember(0,&m);
    CAsnDescriptor d;
    m.GetDescriptor(&d);
    
    int len = get_lua_list_len(L,idx);
    if(len <= 0)return ERROR;

    SET_OF_INT tmp_list;
    memset(&tmp_list,0,sizeof(tmp_list));
    ASN_ZALLOC(tmp_list.array,int*,len);
    tmp_list.count = len;
    tmp_list.size = len;

    for(int i = 0; i < len; i++)
    {
        int top = lua_gettop(L);
        lua_pushnumber(L,i+1);
        lua_gettable(L,idx);

        int new_top = top+1;
        uint8_t *new_out = NULL;
    
        ASSERT(!lua_isnoneornil(L,new_top));

        if(!lua_to_asn(&d,L,new_top,&new_out))
        {
            lua_settop(L,top);
            return ERROR;
        }
        ASSERT(new_out);
        tmp_list.array[i] = (int*)new_out;        
        lua_settop(L,top);
    }

    memcpy((*out),&tmp_list,sizeof(tmp_list));
    return OK;
}

status_t lua_to_asn(LUA_TO_ASN_PARAM_DEF)
{
    ASSERT(descriptor && L && out);
    ASSERT(descriptor->Get());

    #define CALL_LIST descriptor,L,idx,out

    if(descriptor->IsAsnChoice())
    {        
        lua_to_asn_choice(CALL_LIST);
    }
    else if(descriptor->IsAsnSequence())
    {
        lua_to_asn_sequence(CALL_LIST);
    }
    else if(descriptor->IsAsnInteger())
    {
        lua_to_asn_integer(CALL_LIST);
    }
    else if(descriptor->IsAsnOctetString())
    {
        lua_to_asn_octet_string(CALL_LIST);
    }    
    else if(descriptor->IsAsnBitString())
    {
        lua_to_asn_bit_string(CALL_LIST);
    }    
    else if(descriptor->IsAsnIA5String())
    {
        lua_to_asn_ia5_string(CALL_LIST);
    }
    else if(descriptor->IsAsnEnumerated())
    {
        lua_to_asn_enumerated(CALL_LIST);
    }
    else if(descriptor->IsAsnSequenceOf())
    {
        lua_to_asn_sequence_of(CALL_LIST);
    }
    else
    {
        XLOG(LOG_MODULE_USER,LOG_LEVEL_ERROR,
            "unknown asn type: %s",descriptor->GetName());
        return ERROR;
    }

    return OK;
}