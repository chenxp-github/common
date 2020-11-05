#ifndef __LUALIB_JOYSTICK_H
#define __LUALIB_JOYSTICK_H

#include "joystick.h"
#include "luavm.h"

#define LUA_USERDATA_JOYSTICK "lua_userdata_joystick"

CJoystick* get_joystick(lua_State *L, int idx);
lua_userdata *joystick_new_userdata(lua_State *L,CJoystick *pobj,int is_weak);
int luaopen_joystick(lua_State *L);
bool is_joystick(lua_State *L, int idx);
const luaL_Reg* get_joystick_funcs();

#endif
