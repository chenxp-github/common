#ifndef __ASN_TO_JS_H
#define __ASN_TO_JS_H

#include "asn_descriptor.h"
#include "asn_member.h"

#define ASN_TO_JS_PARAM_DEF \
CAsnDescriptor *descriptor,\
CAsnMember *member,\
const void *obj_ptr,\
int level, CFileBase *out\

int asn_to_js_alloc_id();
status_t asn_to_js(ASN_TO_JS_PARAM_DEF);

#endif



