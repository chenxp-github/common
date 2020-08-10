#ifndef __ASN_HELPER_H
#define __ASN_HELPER_H

#define ALLOC_OBJ(type) (type*)calloc(sizeof(type),1)
#define ALLOC_ARRAY(type,n) (type*)calloc(sizeof(type),n)

#define ALLOC_OCTET_STRING(var,n) do{\
    (var).buf = ALLOC_ARRAY(uint8_t,n);\
    (var).size = n;\
}while(0)\

#define ALLOC_OCTET_STRING_AND_SET(var,mem) do{\
    ALLOC_OCTET_STRING(var,(mem).StrLen());\
    memcpy((var).buf,(mem).CStr(),(mem).StrLen());\
}while(0)\

#define ALLOC_IA5_STRING(var,n) ALLOC_OCTET_STRING(var,n)
#define ALLOC_IA5_STRING_AND_SET(var,mem) ALLOC_OCTET_STRING_AND_SET(var,mem)

#define ALLOC_BIT_STRING(var,bits) do{\
    (var).buf = ALLOC_ARRAY(uint8_t,(bits+7)/8);\
    (var).size = (bits+7)/8;\
    (var).bits_unused = ((bits+7)/8)*8 - bits;\
}while(0)\

#define BIT_STRING_SET_BIT(var,n,v) do{\
    int _i = (n)/8, _o = 7-((n)%8);\
    if(v)\
        (var).buf[_i] |= (1<<_o);\
    else\
        (var).buf[_i] &= ~(1<<_o);\
}while(0)\

#define BIT_STRING_GET_BIT(var,n,v) do{\
    int _i = (n)/8, _o = 7-((n)%8);\
    v = ((var).buf[_i] >> _o)&0x01;\
}while(0)\

#endif

