#ifndef __ASN_TO_JS_TREE_H
#define __ASN_TO_JS_TREE_H

#include "asn_descriptor.h"
#include "asn_member.h"

#define ASN_TO_JS_TREE_PARAM_DEF \
CAsnDescriptor *descriptor,\
CAsnMember *member,\
const void *obj_ptr,\
int level, CFileBase *out\

int asn_to_js_tree_alloc_id();
status_t asn_to_js_tree(ASN_TO_JS_TREE_PARAM_DEF);

#endif

