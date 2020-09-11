#include "lualib_websocketserver.h"
#include "mem_tool.h"
#include "syslog.h"
#include "lualib_websocketmessage.h"
#include "lualib_tcpsocket.h"
#include "lualib_mem.h"
#include "lua_helper.h"

LUA_IS_VALID_USER_DATA_FUNC(CWebSocketServer,websocketserver)
LUA_GET_OBJ_FROM_USER_DATA_FUNC(CWebSocketServer,websocketserver)
LUA_NEW_USER_DATA_FUNC(CWebSocketServer,websocketserver,WEBSOCKETSERVER)
LUA_GC_FUNC(CWebSocketServer,websocketserver)
LUA_IS_SAME_FUNC(CWebSocketServer,websocketserver)
LUA_TO_STRING_FUNC(CWebSocketServer,websocketserver)

bool is_websocketserver(lua_State *L, int idx)
{        
    const char* ud_names[] = {
        LUA_USERDATA_WEBSOCKETSERVER,
    };            
    lua_userdata *ud = NULL;
    for(size_t i = 0; i < sizeof(ud_names)/sizeof(ud_names[0]); i++)
    {
        ud = (lua_userdata*)luaL_testudata(L, idx, ud_names[i]);
        if(ud)break;
    }
    return websocketserver_is_userdata_valid(ud);  
}

/****************************************************/
static status_t websocketserver_new(lua_State *L)
{
    CWebSocketServer *pwebsocketserver;
    NEW(pwebsocketserver,CWebSocketServer);
    pwebsocketserver->Init(how_to_get_global_taskmgr(L));
    websocketserver_new_userdata(L,pwebsocketserver,0);
    return 1;
}

static status_t websocketserver_isconnected(lua_State *L)
{
    CWebSocketServer *pwebsocketserver = get_websocketserver(L,1);
    ASSERT(pwebsocketserver);
    bool ret0 = pwebsocketserver->IsConnected();
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t websocketserver_sendmessage(lua_State *L)
{
    CWebSocketServer *pwebsocketserver = get_websocketserver(L,1);
    ASSERT(pwebsocketserver);
    CWebSocketMessage *msg = get_websocketmessage(L,2);
    ASSERT(msg);
    
    lua_userdata *ud = (lua_userdata*)lua_touserdata(L,2);
    ASSERT(ud);
    //the websocket server will take charge to free the message
    //so just make it as weak reference.
    ud->is_attached = 1; 
    msg->SetIsOnHeap(true); 
    status_t ret0 = pwebsocketserver->SendMessage(msg);
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t websocketserver_onnewclientarrive(lua_State *L)
{
    CWebSocketServer *pwebsocketserver = get_websocketserver(L,1);
    ASSERT(pwebsocketserver);
    CTcpSocket *socket = get_tcpsocket(L,2);
    ASSERT(socket);
    bool clear = lua_toboolean(L,3)!=0;
    status_t ret0 = pwebsocketserver->OnNewClientArrive(socket->GetSocketFd(),clear);
    socket->socket_num = -1; //pass ownership to websocket server
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t websocketserver_reset(lua_State *L)
{
    CWebSocketServer *pwebsocketserver = get_websocketserver(L,1);
    ASSERT(pwebsocketserver);
    SAVE_WEAK_REF_ID(*pwebsocketserver,w);
    pwebsocketserver->Destroy();
    pwebsocketserver->Init(how_to_get_global_taskmgr(L));
    RESTORE_WEAK_REF_ID(*pwebsocketserver,w);
    return 0;
}

static status_t websocketserver_callback_onmessage(lua_State *L, int _funcid, bool _once, CMem *payload)
{
    ASSERT(payload);

    if(_funcid == LUA_REFNIL)
    {
        return ERROR;
    }

    CLuaVm vm;
    vm.Init(L);
    lua_rawgeti(L,LUA_REGISTRYINDEX,_funcid);
    if(_once)
    {
        luaL_unref(L,LUA_REGISTRYINDEX,_funcid);
    }
    mem_new_userdata(L,payload,1);
    vm.Run(1,0);
    vm.ClearStack();
    return OK;
}

#define INDEX_CALLBACK 10
#define INDEX_L 11
static status_t websocketserver_setonmessage(lua_State *L)
{
    CWebSocketServer *pwebsocketserver = get_websocketserver(L,1);
    ASSERT(pwebsocketserver);

    if(pwebsocketserver->Callback()->GetParamType(INDEX_CALLBACK) == PARAM_TYPE_INT)
    {
        int old = pwebsocketserver->Callback()->GetParamInt(INDEX_CALLBACK);
        CLuaVm::ReleaseFunction(L,old);
    }

    int callback = CLuaVm::ToFunction(L,2);

    BEGIN_CLOSURE_FUNC(func)
    {        
        CLOSURE_PARAM_INT(e,0);        
        CLOSURE_PARAM_INT(callback,INDEX_CALLBACK);
        CLOSURE_PARAM_PTR(lua_State*,L,INDEX_L);

        if(e == CWebSocketServer::EVENT_GOT_PAYLOAD)
        {
            CLOSURE_PARAM_PTR(CMem*,payload,1);
            CLOSURE_PARAM_PTR(CWebSocketFrameHeader*,header,2);           		
            websocketserver_callback_onmessage(L,callback,false,payload);
        }
        return OK;
    }
    END_CLOSURE_FUNC(func);
    
    pwebsocketserver->Callback()->SetFunc(func);
    pwebsocketserver->Callback()->SetParamInt(INDEX_CALLBACK,callback);        
    pwebsocketserver->Callback()->SetParamPointer(INDEX_L,L);
    return 0;
}

/****************************************************/
static const luaL_Reg websocketserver_funcs_[] = {
    {"__gc",websocketserver_gc_},
    {"__tostring",websocketserver_tostring_},
    {"__is_same",websocketserver_issame_},
    {"new",websocketserver_new},
    {"IsConnected",websocketserver_isconnected},
    {"SendMessage",websocketserver_sendmessage},
    {"OnNewClientArrive",websocketserver_onnewclientarrive},
    {"Reset",websocketserver_reset},
    {"SetOnMessage",websocketserver_setonmessage},
    {NULL,NULL},
};

const luaL_Reg* get_websocketserver_funcs()
{
    return websocketserver_funcs_;
}

static int luaL_register_websocketserver(lua_State *L)
{	
    static luaL_Reg _websocketserver_funcs_[MAX_LUA_FUNCS];
    int _index = 0;        

    CLuaVm::CombineLuaFuncTable(_websocketserver_funcs_,&_index,get_websocketserver_funcs(),true);

    luaL_newmetatable(L, LUA_USERDATA_WEBSOCKETSERVER);
    lua_pushvalue(L, -1);	
    lua_setfield(L, -2, "__index");	
    luaL_setfuncs(L,_websocketserver_funcs_,0);	
    lua_pop(L, 1);
    luaL_newlib(L,_websocketserver_funcs_);
    return 1;
}        

int luaopen_websocketserver(lua_State *L)
{
    luaL_requiref(L, "WebSocketServer",luaL_register_websocketserver,1);
    lua_pop(L, 1);
    return 0;
}        

