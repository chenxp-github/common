#ifndef __COMMON_UTILS_H
#define __COMMON_UTILS_H

#include "mem.h"

status_t get_self_exe_path(CMem *fullpath);
status_t make_full_path_with_exe(const char *rpath, CMem *out);


#endif
