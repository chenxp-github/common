#ifndef __ASN_TO_JSON_H
#define __ASN_TO_JSON_H

#include "asn_descriptor.h"
#include "asn_member.h"

#define ASN_TO_JSON_PARAM_DEF \
CAsnDescriptor *descriptor,\
CAsnMember *member,\
const void *obj_ptr,\
int level, CFileBase *out, int *pending_comma\

int asn_to_json_alloc_id();
status_t asn_to_json(ASN_TO_JSON_PARAM_DEF);

#endif




