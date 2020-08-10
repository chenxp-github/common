#include "ctaskmgr.h"
#include "mem_tool.h"
#include "syslog.h"

status_t taskmgr_del_node(struct taskmgr *self,struct task *node);

status_t taskmgr_init_basic(struct taskmgr *self)
{
    self->_index = NULL;
    self->_top = 0;
    self->_size = 0;
    self->_unique_id = 0;
    return OK;
}
status_t taskmgr_init(struct taskmgr *self,int init_size)
{
    int i;
    taskmgr_init_basic(self);  
    self->_size = init_size;
    MALLOC(self->_index,struct task * ,self->_size);
    for(i = 0; i < self->_size; i++)
        self->_index[i] = NULL;
    return OK;
}
status_t taskmgr_destroy(struct taskmgr *self)
{
    int i;
    if(self == NULL)return ERROR;
    if(self->_index == NULL)
        return ERROR;
    for(i = 0; i < self->_top; i++)
    {
        taskmgr_del_node(self,self->_index[i]);
    }
    FREE(self->_index);
    taskmgr_init_basic(self);   
    return OK;
}
bool_t taskmgr_is_empty(struct taskmgr *self)
{
    return self->_top <= 0;
}
bool_t taskmgr_is_full(struct taskmgr *self)
{
    return self->_top >= self->_size;
}

status_t taskmgr_auto_resize(struct taskmgr *self)
{
    return OK;
}

status_t taskmgr_push_ptr(struct taskmgr *self,struct task *node)
{
    ASSERT(node);   
    taskmgr_auto_resize(self);
    ASSERT(!taskmgr_is_full(self));
    self->_index[self->_top] = node;
    self->_top++;
    return OK;
}

struct task * taskmgr_pop_ptr(struct taskmgr *self)
{
    if(taskmgr_is_empty(self))
        return NULL;
    self->_top--;
    return self->_index[self->_top];
}

int taskmgr_get_len(struct taskmgr *self)
{
    return self->_top;
}

struct task * taskmgr_get_top_ptr(struct taskmgr *self)
{
    if(taskmgr_is_empty(self))
        return NULL;
    return self->_index[self->_top - 1];
}
status_t taskmgr_del_top(struct taskmgr *self)
{
    if(taskmgr_is_empty(self))
        return ERROR;
    self->_top--;
    taskmgr_del_node(self,self->_index[self->_top]);
    return OK;
}
status_t taskmgr_clear(struct taskmgr *self)
{
    while(taskmgr_del_top(self));
    return OK;
}

struct task * taskmgr_get_elem(struct taskmgr *self,int index)
{
    if(index < 0 || index >= self->_top)
        return NULL;
    return self->_index[index];
}

struct task * taskmgr_bsearch_node(struct taskmgr *self,struct task *node,int order)
{
    return taskmgr_get_elem(self,taskmgr_bsearch(self,node,order));
}
int taskmgr_bsearch(struct taskmgr *self,struct task *node,int order)
{
    int find,pos;
    pos = taskmgr_bsearch_pos(self,node,order,&find);
    if(find) return pos;
    return -1;
}

status_t taskmgr_ins_elem_ptr(struct taskmgr *self,int i, struct task *node)
{
    int k;

    ASSERT(node);
    ASSERT(i >= 0 && i <= self->_top);

    taskmgr_auto_resize(self);
    ASSERT(!taskmgr_is_full(self));
    for(k = self->_top; k > i; k--)
    {
        self->_index[k] = self->_index[k - 1];
    }
    self->_index[i] = node;
    self->_top++;
    return OK;
}

struct task * taskmgr_remove_elem(struct taskmgr *self,int index)
{
    int k;
    struct task *p;
    
    ASSERT(index >= 0 && index < self->_top);
    p = taskmgr_get_elem(self,index);

    for(k = index; k < self->_top-1; k++)
    {
        self->_index[k] = self->_index[k + 1];
    }
    self->_top --;
    self->_index[self->_top] = NULL;
    return p;
}

status_t taskmgr_del_elem(struct taskmgr *self,int i)
{
    struct task *p = taskmgr_remove_elem(self,i);
    if(p != NULL)
    {
        taskmgr_del_node(self,p);
        return OK;
    }
    return ERROR;
}

status_t taskmgr_ins_ordered_ptr(struct taskmgr *self,struct task *node, int order,int unique)
{
    int pos,find;
    pos = taskmgr_bsearch_pos(self,node,order,&find);
    if(find && unique)
        return ERROR;
    return taskmgr_ins_elem_ptr(self,pos,node);
}

int taskmgr_bsearch_pos(struct taskmgr *self,struct task *node, int order, int *find_flag)
{
    int low,high,mid,comp;
    
    low = 0; 
    high = taskmgr_get_len(self) - 1;

    while(low<=high)
    {
        mid = (low+high) >> 1;

        comp = self->_index[mid]->_id - node->_id;
        if(comp == 0)
        {
            *find_flag = TRUE;
            return mid;
        }
    
        if(order != 0) comp = -comp;
        if(comp>0)high=mid-1;else low=mid+1;
    }
    *find_flag = FALSE;
    return low;
}
/////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
status_t taskmgr_del_node(struct taskmgr *self,struct task *node)
{
    task_run_destroy_func(node);
    task_destroy(node);
    FREE(node);
    return OK;
}

int taskmgr_id_to_index(struct taskmgr *self, int id)
{
    struct task tmp;
    int ff,pos;

    tmp._id = id;

    if(id == 0)return -1;
    if(self->_index == NULL)
        return -1;
    
    pos = taskmgr_bsearch_pos(self,&tmp,0,&ff);
    if(ff)return pos;
    return -1;
}

struct task *taskmgr_get_task(struct taskmgr *self,int id)
{
    int index = taskmgr_id_to_index(self,id);
    if(index < 0 || index >= taskmgr_get_len(self))
        return NULL;
    return taskmgr_get_elem(self,index);
}
status_t taskmgr_add_task(struct taskmgr *self,struct task *task)
{
    struct task *top;

    ASSERT(task);

    if(taskmgr_id_to_index(self,task_get_id(task)) >= 0)
    {
        LOG("task(%d) already exist,add failed.",task_get_id(task));
        return ERROR;
    }
    top = taskmgr_get_top_ptr(self);
    if(top)
    {
        ASSERT(task_get_id(task) > task_get_id(top));
    }
    task_set_taskmgr(task,self);
    return taskmgr_push_ptr(self,task);
}

int taskmgr_alloc_id(struct taskmgr *self)
{
    self->_unique_id++;
    if(self->_unique_id == 0)
        self->_unique_id++;
    return self->_unique_id;
}

status_t taskmgr_schedule_once(struct taskmgr *self)
{
    uint32_t now,interval;  
    int i,len = taskmgr_get_len(self);
    for(i = 0; i < len; i++)
    {
        struct task *pt = taskmgr_get_elem(self,i);
        if(pt == NULL)continue;
        if(!task_is_running(pt))continue;
        
        now = crt_get_sys_timer();

        interval = now - pt->_last_sleep_time;
        pt->_last_sleep_time = now;
        if(task_is_sleeping(pt,interval))continue;

        interval = now - pt->_last_run_time;
        pt->_last_run_time = now;

        task_run(pt,interval);
    }
    return OK;
}

status_t taskmgr_check_delete(struct taskmgr *self)
{
    int i;

    for(i = 0; i < taskmgr_get_len(self); i++)
    {
        struct task *pt = taskmgr_get_elem(self,i);
        ASSERT(pt);

        if(task_is_dead(pt))
        {
            taskmgr_del_elem(self,i);
            i--;
        }
    }       
    return OK;
}

status_t taskmgr_schedule(struct taskmgr *self)
{
    taskmgr_check_delete(self);
    taskmgr_schedule_once(self);
    return OK;
}

status_t taskmgr_suspend_task(struct taskmgr *self,int task_id)
{
    struct task *pt = taskmgr_get_task(self,task_id);
    if(pt == NULL)return ERROR;
    task_suspend(pt);
    return OK;
}
status_t taskmgr_resume_task(struct taskmgr *self,int task_id)
{
    struct task *pt = taskmgr_get_task(self,task_id);
    if(pt == NULL)return ERROR;
    task_resume(pt);
    return OK;
}
bool_t taskmgr_is_task_running(struct taskmgr *self,int task_id)
{
    struct task *pt = taskmgr_get_task(self,task_id);
    if(pt == NULL)return ERROR;
    return task_is_running(pt);
}
status_t taskmgr_del_task(struct taskmgr *self,int task_id)
{
    int i = taskmgr_id_to_index(self,task_id);
    if(i >= 0  && i < taskmgr_get_len(self))
        return taskmgr_del_elem(self,i);
    return ERROR;
}

bool_t taskmgr_is_task(struct taskmgr *self,int tid)
{
    struct task *pt = taskmgr_get_task(self,tid);
    if(pt == NULL)return FALSE;
    return !task_is_dead(pt);
}

status_t taskmgr_quit_task(struct taskmgr *self,int *task_id)
{
    struct task *pt;

    ASSERT(task_id);
    pt = taskmgr_get_task(self,*task_id);
    if(pt)
    {
        task_quit(pt);
        *task_id = 0;
        return OK;
    }

    return ERROR;
}

