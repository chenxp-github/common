#include "lualib_xml.h"
#include "mem_tool.h"
#include "syslog.h"
#include "lualib_filebase.h"
#include "lualib_mem.h"
#include "lualib_xmlnode.h"

LUA_IS_VALID_USER_DATA_FUNC(CXml,xml)
LUA_GET_OBJ_FROM_USER_DATA_FUNC(CXml,xml)
LUA_NEW_USER_DATA_FUNC(CXml,xml,XML)
LUA_GC_FUNC(CXml,xml)
LUA_IS_SAME_FUNC(CXml,xml)
LUA_TO_STRING_FUNC(CXml,xml)

bool is_xml(lua_State *L, int idx)
{        
    const char* ud_names[] = {
        LUA_USERDATA_XML,
    };            
    lua_userdata *ud = NULL;
    for(size_t i = 0; i < sizeof(ud_names)/sizeof(ud_names[0]); i++)
    {
        ud = (lua_userdata*)luaL_testudata(L, idx, ud_names[i]);
        if(ud)break;
    }
    return xml_is_userdata_valid(ud);  
}
/****************************************************/
static status_t xml_new(lua_State *L)
{
    CXml *pxml;
    NEW(pxml,CXml);
    pxml->Init();
    xml_new_userdata(L,pxml,0);
    return 1;
}

static status_t xml_loadxml_v1(lua_State *L)
{
    CXml *pxml = get_xml(L,1);
    ASSERT(pxml);
    const char* fn = (const char*)lua_tostring(L,2);
    ASSERT(fn);
    status_t ret0 = pxml->LoadXml(fn);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t xml_loadxml_v2(lua_State *L)
{
    CXml *pxml = get_xml(L,1);
    ASSERT(pxml);
    CFileBase *file = get_filebase(L,2);
    ASSERT(file);
    status_t ret0 = pxml->LoadXml(file);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t xml_getnodebypath(lua_State *L)
{
    CXml *pxml = get_xml(L,1);
    ASSERT(pxml);
    const char* path = (const char*)lua_tostring(L,2);
    ASSERT(path);
    CXmlNode *ret0 = pxml->GetNodeByPath(path);
    ASSERT(ret0);
    xmlnode_new_userdata(L,ret0,1);
    return 1;
}

static status_t xml_writetofile_v1(lua_State *L)
{
    CXml *pxml = get_xml(L,1);
    ASSERT(pxml);
    const char* fn = (const char*)lua_tostring(L,2);
    ASSERT(fn);
    CMem *header = get_mem(L,3);
    ASSERT(header);
    status_t ret0 = pxml->WriteToFile(fn,header);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t xml_writetofile_v2(lua_State *L)
{
    CXml *pxml = get_xml(L,1);
    ASSERT(pxml);
    CFileBase *file = get_filebase(L,2);
    ASSERT(file);
    CMem *header = get_mem(L,3);
    ASSERT(header);
    status_t ret0 = pxml->WriteToFile(file,header);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t xml_destroy(lua_State *L)
{
    CXml *pxml = get_xml(L,1);
    ASSERT(pxml);
    SAVE_WEAK_REF_ID(*pxml,w);
    status_t ret0 = pxml->Destroy();
    RESTORE_WEAK_REF_ID(*pxml,w);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t xml_getroot(lua_State *L)
{
    CXml *pxml = get_xml(L,1);
    ASSERT(pxml);
    CXmlNode *ret0 = pxml->GetRoot();
    ASSERT(ret0);
    xmlnode_new_userdata(L,ret0,1);
    return 1;
}

static status_t xml_addroot(lua_State *L)
{
    CXml *pxml = get_xml(L,1);
    ASSERT(pxml);
    CXmlNode *node = get_xmlnode(L,2);
    ASSERT(node);
    status_t ret0 = pxml->AddRoot(node);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t xml_loadxml(lua_State *L)
{
    if(lua_isstring(L,2))
    {
        return xml_loadxml_v1(L);
    }

    if(is_filebase(L,2))
    {
        return xml_loadxml_v2(L);
    }

    return 0;
}

static status_t xml_writetofile(lua_State *L)
{
    if(lua_isstring(L,2) && is_mem(L,3))
    {
        return xml_writetofile_v1(L);
    }

    if(is_filebase(L,2) && is_mem(L,3))
    {
        return xml_writetofile_v2(L);
    }

    return 0;
}

/****************************************************/
static const luaL_Reg xml_funcs_[] = {
    {"new",xml_new},    
    {"__gc",xml_gc_},
    {"__tostring",xml_tostring_},
    {"__is_same",xml_issame_},
    {"WriteToFile",xml_writetofile},
    {"AddRoot",xml_addroot},
    {"LoadXml",xml_loadxml},
    {"GetNodeByPath",xml_getnodebypath},
    {"GetRoot",xml_getroot},
    {"Destroy",xml_destroy},
    {NULL,NULL},
};

const luaL_Reg* get_xml_funcs()
{
    return xml_funcs_;
}

static int luaL_register_xml(lua_State *L)
{	
    static luaL_Reg _xml_funcs_[MAX_LUA_FUNCS];
    int _index = 0;        

    CLuaVm::CombineLuaFuncTable(_xml_funcs_,&_index,get_xml_funcs(),true);

    luaL_newmetatable(L, LUA_USERDATA_XML);
    lua_pushvalue(L, -1);	
    lua_setfield(L, -2, "__index");	
    luaL_setfuncs(L,_xml_funcs_,0);	
    lua_pop(L, 1);
    luaL_newlib(L,_xml_funcs_);
    return 1;
}        

int luaopen_xml(lua_State *L)
{
    luaL_requiref(L, "Xml",luaL_register_xml,1);
    lua_pop(L, 1);
    return 0;
}        

