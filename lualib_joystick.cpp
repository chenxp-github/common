#include "lualib_joystick.h"
#include "mem_tool.h"
#include "syslog.h"
#include "lua_helper.h"
#include "tasktimer.h"
#include "weak_pointer.h"

LUA_IS_VALID_USER_DATA_FUNC(CJoystick,joystick)
LUA_GET_OBJ_FROM_USER_DATA_FUNC(CJoystick,joystick)
LUA_NEW_USER_DATA_FUNC(CJoystick,joystick,JOYSTICK)
LUA_GC_FUNC(CJoystick,joystick)
LUA_IS_SAME_FUNC(CJoystick,joystick)
LUA_TO_STRING_FUNC(CJoystick,joystick)

bool is_joystick(lua_State *L, int idx)
{        
    const char* ud_names[] = {
        LUA_USERDATA_JOYSTICK,
    };            
    lua_userdata *ud = NULL;
    for(size_t i = 0; i < sizeof(ud_names)/sizeof(ud_names[0]); i++)
    {
        ud = (lua_userdata*)luaL_testudata(L, idx, ud_names[i]);
        if(ud)break;
    }
    return joystick_is_userdata_valid(ud);  
}

/****************************************************/
static status_t joystick_new(lua_State *L)
{
    CJoystick *pjoystick;
    NEW(pjoystick,CJoystick);
    pjoystick->Init();
    lua_userdata *ud = joystick_new_userdata(L,pjoystick,0);
    ASSERT(ud);
    ud->userdata = (void*)LUA_REFNIL;
    return 1;
}

static status_t joystick_open(lua_State *L)
{
    CJoystick *pjoystick = get_joystick(L,1);
    ASSERT(pjoystick);
    const char* dev_name = (const char*)lua_tostring(L,2);
    ASSERT(dev_name);
    status_t ret0 = pjoystick->Open(dev_name);
    if(ret0)
    {
        pjoystick->ClearCachedEvents();
    }    
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t joystick_close(lua_State *L)
{
    CJoystick *pjoystick = get_joystick(L,1);
    ASSERT(pjoystick);
    status_t ret0 = pjoystick->Close();
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t joystick_getnumberofaxes(lua_State *L)
{
    CJoystick *pjoystick = get_joystick(L,1);
    ASSERT(pjoystick);
    int ret0 = pjoystick->GetNumberOfAxes();
    lua_pushinteger(L,ret0);
    return 1;
}

static status_t joystick_getnumberofbuttons(lua_State *L)
{
    CJoystick *pjoystick = get_joystick(L,1);
    ASSERT(pjoystick);
    int ret0 = pjoystick->GetNumberOfButtons();
    lua_pushinteger(L,ret0);
    return 1;
}

static status_t joystick_getname(lua_State *L)
{
    CJoystick *pjoystick = get_joystick(L,1);
    ASSERT(pjoystick);  
    LOCAL_MEM(name);
    if(pjoystick->GetName(&name))
    {
        lua_push_local_string(L,name.CStr());
        return 1;
    }
    return 0;
}

static status_t joystick_isclosed(lua_State *L)
{
    CJoystick *pjoystick = get_joystick(L,1);
    ASSERT(pjoystick);
    bool ret0 = pjoystick->IsClosed();
    lua_pushboolean(L,ret0);
    return 1;
}

static status_t joystick_callback_onjsevent(lua_State *L, int _funcid, bool _once, int event, struct js_event *data)
{
    ASSERT(data);

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
    lua_pushinteger(L,event);

    lua_newtable(L);
    int top = lua_gettop(L);

    lua_pushstring(L,"number");
    lua_pushinteger(L,data->number);
    lua_settable(L,top);

    lua_pushstring(L,"time");
    lua_pushinteger(L,data->time);
    lua_settable(L,top);

    lua_pushstring(L,"type");
    lua_pushinteger(L,data->type);
    lua_settable(L,top);
    
    lua_pushstring(L,"value");
    lua_pushinteger(L,data->value);
    lua_settable(L,top);
    
    vm.Run(2,0);
    vm.ClearStack();
    return OK;
}

static status_t read_joystick(CClosure *closure)
{
    CLOSURE_PARAM_PTR(CTaskTimer*,ptask,1);

    CRawWeakPointer *wp = closure->GetParamWeakPointer(10);
    if(!wp || !wp->GetRawPtr())
    {
        ptask->Quit();
        return ERROR;
    }

    CLOSURE_PARAM_WEAKPTR(CJoystick*,pjoystick,10);
    CLOSURE_PARAM_INT(onjsevent,11);
    CLOSURE_PARAM_PTR(lua_State*,L,12);

    bool closed = pjoystick->IsClosed();

    struct js_event e;   
    if(pjoystick->Read(&e) > 0)
    {
        joystick_callback_onjsevent(L,onjsevent,false,1,&e);
    }
    else if(!closed && pjoystick->IsClosed())
    {
         joystick_callback_onjsevent(L,onjsevent,false,-1,&e);
    }
    return OK;
}

static status_t joystick_seteventhandler(lua_State *L)
{
    CJoystick *pjoystick = get_joystick(L,1);
    ASSERT(pjoystick);
    int onjsevent = CLuaVm::ToFunction(L,2);

    lua_userdata *ud = (lua_userdata*)lua_touserdata(L,1);
    ASSERT(ud);    
    if(ud->userdata != (void*)LUA_REFNIL)
    {
        CLuaVm::ReleaseFunction(L,(int)((int_ptr_t)ud->userdata));
    }
    ud->userdata = (void*)((int_ptr_t)onjsevent);

    ASSERT(how_to_get_global_taskmgr);
    CTaskMgr *taskmgr = how_to_get_global_taskmgr(L);
    ASSERT(taskmgr);

    CTaskTimer *pt = CTaskTimer::NewTimer(taskmgr,1,false);
    pt->Start();
    pt->Callback()->SetFunc(read_joystick);    

    CWeakPointer<CJoystick> wp(pjoystick);
    pt->Callback()->SetParamWeakPointer(10,&wp);
    pt->Callback()->SetParamInt(11,onjsevent);
    pt->Callback()->SetParamPointer(12,L);   
    lua_pushboolean(L,true);
    return 1;
}

/****************************************************/
static const luaL_Reg joystick_funcs_[] = {
    {"__gc",joystick_gc_},
    {"__tostring",joystick_tostring_},
    {"__is_same",joystick_issame_},
    {"new",joystick_new},
    {"Open",joystick_open},
    {"Close",joystick_close},
    {"GetNumberOfAxes",joystick_getnumberofaxes},
    {"GetNumberOfButtons",joystick_getnumberofbuttons},
    {"GetName",joystick_getname},
    {"IsClosed",joystick_isclosed},
    {"SetEventHandler",joystick_seteventhandler},    
    {NULL,NULL},
};

const luaL_Reg* get_joystick_funcs()
{
    return joystick_funcs_;
}

static int luaL_register_joystick(lua_State *L)
{	
    static luaL_Reg _joystick_funcs_[MAX_LUA_FUNCS];
    int _index = 0;        

    CLuaVm::CombineLuaFuncTable(_joystick_funcs_,&_index,get_joystick_funcs(),true);

    luaL_newmetatable(L, LUA_USERDATA_JOYSTICK);
    lua_pushvalue(L, -1);	
    lua_setfield(L, -2, "__index");	
    luaL_setfuncs(L,_joystick_funcs_,0);	
    lua_pop(L, 1);
    luaL_newlib(L,_joystick_funcs_);
    return 1;
}        

int luaopen_joystick(lua_State *L)
{
    luaL_requiref(L, "Joystick",luaL_register_joystick,1);
    lua_pop(L, 1);
    return 0;
}        

