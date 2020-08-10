#include "ctask.h"
#include "syslog.h"
#include "mem_tool.h"
#include "ctaskmgr.h"

#define TASK_IS_RUNNING     0x00000001
#define TASK_IS_DELETED     0x00000002
#define TASK_IS_SLEEPING    0x00000004

status_t task_init_basic(struct task *self)
{
    self->_user_data = NULL;
    self->_run = NULL;  
    self->_destroy = NULL;
    self->_id = 0;
    self->_type = 0;
    self->_last_sleep_time = 0;
    self->_sleep_time = 0;
    self->_total_sleep_time = 0;
    self->_last_run_time = 0;
    self->_flags = 0;
    self->_taskmgr = NULL;

    return OK;
}
status_t task_init(struct task *self,struct taskmgr *mgr)
{
    task_init_basic(self);

    self->_last_run_time = crt_get_sys_timer();
    self->_last_sleep_time = self->_last_run_time;

    if(mgr)
    {
        self->_id = taskmgr_alloc_id(mgr);
        taskmgr_add_task(mgr,self);
    }

    return OK;
}
status_t task_destroy(struct task *self)
{
    task_init_basic(self);
    return OK;
}
void* task_get_user_data(struct task *self)
{
    return self->_user_data;
}
status_t task_set_user_data(struct task *self,void* i_user_data)
{
    self->_user_data = i_user_data;
    return OK;
}

status_t task_set_run_func(struct task *self, TASK_RUN_FUNC func)
{
    self->_run = func;
    return OK;
}
status_t task_quit(struct task *self)
{
    self->_flags |= TASK_IS_DELETED;
    return OK;
}
status_t task_suspend(struct task *self)
{
    self->_flags &= (~TASK_IS_RUNNING);
    return OK;
}
status_t task_resume(struct task *self)
{
    self->_flags |= TASK_IS_RUNNING;
    return OK;
}
bool_t task_is_running(struct task *self)
{
    if(task_is_dead(self))return FALSE;
    return (self->_flags&TASK_IS_RUNNING) != 0;
}
int task_get_type(struct task *self)
{
    return self->_type;
}
status_t task_set_type(struct task *self,int type)
{
    self->_type = type;
    return OK;
}
int task_get_id(struct task *self)
{
    return self->_id;
}
bool_t task_is_dead(struct task *self)
{
    return (self->_flags & TASK_IS_DELETED) != 0;
}

bool_t task_is_sleeping(struct task *self,uint32_t interval)
{
    if(self->_flags & TASK_IS_SLEEPING)
    {
        self->_sleep_time += interval;
        if(self->_sleep_time >= self->_total_sleep_time)
        {
            self->_flags &= (~TASK_IS_SLEEPING);
            self->_sleep_time = 0;
            self->_total_sleep_time = 0;
        }
        return TRUE;
    }
    return FALSE;
}

status_t task_sleep(struct task *self,uint32_t ms)
{
    if(ms > 0)
    {
        self->_flags |= TASK_IS_SLEEPING;
        self->_sleep_time = 0;
        self->_total_sleep_time = ms;
    }
    return OK;
}
struct taskmgr* task_get_taskmgr(struct task *self)
{
    return self->_taskmgr;
}
status_t task_set_taskmgr(struct task *self,struct taskmgr* _taskmgr)
{
    self->_taskmgr = _taskmgr;
    return OK;
}

status_t task_run(struct task *self,int interval)
{
    ASSERT(self->_run);
    return self->_run(self->_user_data,interval);
}

status_t task_set_destroy_func(struct task *self,TASK_DESTROY_FUNC func)
{
    self->_destroy = func;
    return OK;
}

status_t task_run_destroy_func(struct task *self)
{
    if(self->_destroy)
    {
        return self->_destroy(self->_user_data);
    }
    return ERROR;
}
