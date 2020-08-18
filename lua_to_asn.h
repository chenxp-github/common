#ifndef __LUA_TO_ASN_H
#define __LUA_TO_ASN_H

#include "asn_descriptor.h"
#include "asn_member.h"
#include "lua.hpp"

#define LUA_TO_ASN_PARAM_DEF \
CAsnDescriptor *descriptor, lua_State *L, int idx,uint8_t **out\

status_t lua_to_asn(LUA_TO_ASN_PARAM_DEF);

#endif
