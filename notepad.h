#ifndef __NOTA_PAD_H
#define __NOTA_PAD_H

#include "cruntime.h"

class CNotePad{
public:
    HWND handle;
public:
        CNotePad();
    int FindNotePad();
    int Puts(const WCHAR *str);
    int Printf (const WCHAR * szFormat, ...);
};

extern CNotePad note_pad;

# endif
