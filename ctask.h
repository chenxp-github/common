#ifndef __C_TASK_H
#define __C_TASK_H

#include "cruntime.h"

#define BEGIN_FSM() switch(self->_step){
#define END_FSM() case 0:default:task_suspend(&self->_task);break;}

#define BEGIN_STATE(state) case state:{
#define END_STATE() }break

#define GOTO_STATE(state) self->_step = state
#define CONTINUE(ms) do{task_sleep(&self->_task,ms);return ERROR;}while(0)

typedef status_t (*TASK_RUN_FUNC)(void *user_data,uint32_t interval);
typedef status_t (*TASK_DESTROY_FUNC)(void *p);

struct taskmgr;

struct task{
    void* _user_data;
    TASK_RUN_FUNC _run;
    TASK_DESTROY_FUNC _destroy;

    struct taskmgr *_taskmgr;
    int _id;
    int _type;
    uint32_t _last_sleep_time;
    uint32_t _sleep_time;
    uint32_t _total_sleep_time;
    uint32_t _last_run_time;
    uint32_t _flags;
};

status_t task_init_basic(struct task *self);
status_t task_init(struct task *self,struct taskmgr *mgr);
status_t task_destroy(struct task *self);
void* task_get_user_data(struct task *self);
status_t task_set_user_data(struct task *self,void* i_user_data);
status_t task_set_run_func(struct task *self, TASK_RUN_FUNC func);
status_t task_quit(struct task *self);
status_t task_suspend(struct task *self);
status_t task_resume(struct task *self);
bool_t task_is_running(struct task *self);
int task_get_type(struct task *self);
status_t task_set_type(struct task *self,int type);
int task_get_id(struct task *self);
bool_t task_is_dead(struct task *self);
bool_t task_is_sleeping(struct task *self,uint32_t interval);
status_t task_sleep(struct task *self,uint32_t ms);
struct taskmgr* task_get_taskmgr(struct task *self);
status_t task_set_taskmgr(struct task *self,struct taskmgr* _taskmgr);
status_t task_run_callback(struct task *self,int event);
status_t task_run(struct task *self,int interval);
status_t task_set_destroy_func(struct task *self,TASK_DESTROY_FUNC func);
status_t task_run_destroy_func(struct task *self);

#endif
