#ifndef __ASN_TO_LUA_H
#define __ASN_TO_LUA_H

#include "asn_descriptor.h"
#include "asn_member.h"

#define ASN_TO_LUA_PARAM_DEF \
CAsnDescriptor *descriptor,\
CAsnMember *member,\
const void *obj_ptr,\
int level, CFileBase *out,int *pending_comma\

int asn_to_lua_alloc_id();
status_t asn_to_lua(ASN_TO_LUA_PARAM_DEF);

#endif


