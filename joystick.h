#ifndef __JOYSTICK_H
#define __JOYSTICK_H

/*##Begin Inlcudes##*/
/*##End Inlcudes##*/
#include "mem.h"
#include "memfile.h"
#include <linux/joystick.h>

class CJoystick
/*##Begin Bases##*/
/*##End Bases##*/
{
public:
/*##Begin Members##*/
/*##End Members##*/
    int m_fd;
public:
    CJoystick();
    virtual ~CJoystick();

    status_t InitBasic();
    status_t Init();
    status_t Destroy();

    status_t Copy(CJoystick *_p);
    int Comp(CJoystick *_p);
    status_t Print(CFileBase *_buf);

/*##Begin Getter_H##*/
/*##End Getter_H##*/
/*##Begin Setter_H##*/
/*##End Setter_H##*/

    status_t Open(const char *dev_name);
    status_t Close();
    int GetNumberOfAxes();
    int GetNumberOfButtons();
    status_t GetName(CMem *name);

    int Read(struct js_event *event);
    bool IsClosed();
};

#endif
