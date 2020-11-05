#include "joystick.h"
#include "syslog.h"
#include "mem_tool.h"

CJoystick::CJoystick()
{
    this->InitBasic();
}

CJoystick::~CJoystick()
{
    this->Destroy();
}

status_t CJoystick::InitBasic()
{
/*##Begin InitBasic##*/
/*##End InitBasic##*/
    m_fd = ERROR_FILE_HANDLE;
    return OK;
}

status_t CJoystick::Init()
{
    this->InitBasic();
/*##Begin Init##*/
/*##End Init##*/
    return OK;
}

status_t CJoystick::Destroy()
{
    this->Close();
/*##Begin Destroy##*/
/*##End Destroy##*/
    this->InitBasic();
    return OK;
}

status_t CJoystick::Copy(CJoystick *_p)
{
    ASSERT(_p);
    if(this == _p)return OK;

/*##Begin Copy##*/
/*##End Copy##*/
    return OK;
}

int CJoystick::Comp(CJoystick *_p)
{
    ASSERT(_p);
    if(this == _p)return 0;
    ASSERT(0);
    return 0;
}

status_t CJoystick::Print(CFileBase *_buf)
{
/*##Begin Print##*/
    ASSERT(_buf);
/*##End Print##*/
    return OK;
}

/*@@ Insert Function Here @@*/
status_t CJoystick::Open(const char *dev_name)
{
    ASSERT(dev_name);
    ASSERT(m_fd == ERROR_FILE_HANDLE);

    m_fd = open(dev_name,O_RDONLY|O_NONBLOCK);
    if(m_fd < 0)
    {
        return ERROR;
    }

    return OK;
}

status_t CJoystick::Close()
{
    if(m_fd >= 0)
    {
        close(m_fd);
        m_fd = ERROR_FILE_HANDLE;
    }
    return OK;
}

int CJoystick::GetNumberOfAxes()
{
    ASSERT(m_fd > 0);
    int8_t number_of_axes = 0;
    int rc = ioctl(m_fd, JSIOCGAXES, &number_of_axes);
    if (rc == 0) 
    {
        return number_of_axes;
    }
    return 0;
}

int CJoystick::GetNumberOfButtons()
{
    ASSERT(m_fd > 0);
    int8_t number_of_btns = 0;
    int rc = ioctl(m_fd, JSIOCGBUTTONS, &number_of_btns);
    if (rc == 0) 
    {
        return number_of_btns;
    }
    return 0;
}

status_t CJoystick::GetName(CMem *name)
{
    ASSERT(name);
    char js_name_str[128];
    
    int rc = ioctl(m_fd, JSIOCGNAME(sizeof(js_name_str)), js_name_str);
    if(rc >= 0) 
    {
        name->Puts(js_name_str);
        return OK;
    }
    return  ERROR;
}

bool CJoystick::IsClosed()
{
    return m_fd == ERROR_FILE_HANDLE;    
}

int CJoystick::Read(struct js_event *event)
{
    if(IsClosed())return -1;
    memset(event,0,sizeof(struct js_event));
    int i4_rd_bytes = read(m_fd, event, sizeof(struct js_event));  
    if (i4_rd_bytes == -1) 
    {
        if (errno == EAGAIN) 
        {
            return 0;
        }
        else 
        {
            this->Close();
            return -1;
        }
    }
    return i4_rd_bytes;
}