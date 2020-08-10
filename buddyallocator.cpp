#include "buddyallocator.h"
#include "syslog.h"
#include "mem_tool.h"

#define NODE_UNUSED 0
#define NODE_USED 1
#define NODE_SPLIT 2
#define NODE_FULL 3

static inline int is_pow_of_2(uint32_t x)
{
    return !(x & (x-1));
}

static inline uint32_t next_pow_of_2(uint32_t x)
{
    if (is_pow_of_2(x))
        return x;
    x |= x>>1;
    x |= x>>2;
    x |= x>>4;
    x |= x>>8;
    x |= x>>16;
    return x+1;
}

static inline int _index_offset(int index, int level, int max_level)
{
    return ((index + 1) - (1 << level)) << (max_level - level);
}
////////////////////////////////////////////////////////////////
CBuddyAllocator::CBuddyAllocator()
{
    this->InitBasic();
}
CBuddyAllocator::~CBuddyAllocator()
{
    this->Destroy();
}
status_t CBuddyAllocator::InitBasic()
{
    m_Level = NULL;
    m_Tree = NULL;
    return OK;
}

status_t CBuddyAllocator::Attach(void *data)
{
    m_Level = (int32_t*)data;
    m_Tree =  ((uint8_t*)data) + sizeof(int32_t);
    return OK;
}
status_t CBuddyAllocator::Init(int level,void *data)
{
    this->InitBasic();
    int size = ShmSize(level);
    memset(data,NODE_UNUSED,size);
    this->Attach(data);
    *m_Level = level;
    return OK;
}
status_t CBuddyAllocator::Destroy()
{
    this->InitBasic();
    return OK;
}

int CBuddyAllocator::Alloc(int s)
{
    int size;

    ASSERT(m_Tree);

    if(s==0)
    {
        size = 1;
    }

    else
    {
        size = (int)next_pow_of_2(s);
    }
    int length = 1 << this->GetLevel();
    if (size > length)
        return -1;

    int index = 0;
    int level = 0;
    while (index >= 0)
    {
        if (size == length)
        {
            if (this->m_Tree[index] == NODE_UNUSED)
            {
                this->m_Tree[index] = NODE_USED;
                MarkParent(index);
                return _index_offset(index, level, this->GetLevel());
            }
        }

        else
        {
            // size < length
            switch (this->m_Tree[index])
            {
            case NODE_USED:
            case NODE_FULL:
                break;
            case NODE_UNUSED:
                // split first
                this->m_Tree[index] = NODE_SPLIT;
                this->m_Tree[index*2+1] = NODE_UNUSED;
                this->m_Tree[index*2+2] = NODE_UNUSED;
            default:
                index = index * 2 + 1;
                length /= 2;
                level++;
                continue;
            }

        }

        if (index & 1)
        {
            ++index;
            continue;
        }

        for (;;)
        {
            level--;
            length *= 2;
            index = (index+1)/2 -1;
            if (index < 0)
                return -1;
            if (index & 1)
            {
                ++index;
                break;
            }

        }

    }

    return -1;

}

void CBuddyAllocator::Combine(int index)
{
    for (;;)
    {
        int buddy = index - 1 + (index & 1) * 2;
        if (buddy < 0 || this->m_Tree[buddy] != NODE_UNUSED)
        {
            this->m_Tree[index] = NODE_UNUSED;
            while (((index = (index + 1) / 2 - 1) >= 0) &&  this->m_Tree[index] == NODE_FULL)
            {
                this->m_Tree[index] = NODE_SPLIT;
            }

            return;
        }

        index = (index + 1) / 2 - 1;
    }
}

void CBuddyAllocator::Free(int offset)
{
    VASSERT(m_Tree);
    VASSERT(offset >= 0);
    VASSERT( offset < (1<< this->GetLevel()));
    int left = 0;
    int length = 1 << this->GetLevel();
    int index = 0;
    for (;;)
    {
        switch (this->m_Tree[index])
        {
            case NODE_USED:
                VASSERT(offset == left);
                Combine(index);
                return;
            case NODE_UNUSED:
                VASSERT(0);
                return;

            default:
                length /= 2;
                if (offset < left + length)
                {
                    index = index * 2 + 1;
                }

                else
                {
                    left += length;
                    index = index * 2 + 2;
                }

                break;
        }
    }
}

int CBuddyAllocator::Size(int offset)
{
    ASSERT(m_Tree);
    ASSERT( offset < (1<< this->GetLevel()));
    int left = 0;
    int length = 1 << this->GetLevel();
    int index = 0;
    for (;;)
    {
        switch (this->m_Tree[index])
        {
        case NODE_USED:
            ASSERT(offset == left);
            return length;
        case NODE_UNUSED:
            ASSERT(0);
            return length;
        default:
            length /= 2;
            if (offset < left + length)
            {
                index = index * 2 + 1;
            }
            else
            {
                left += length;
                index = index * 2 + 2;
            }

            break;
        }
    }

    return 0;
}

void CBuddyAllocator::MarkParent(int index)
{
    for (;;)
    {
        int buddy = index - 1 + (index & 1) * 2;
        if (buddy > 0 && (this->m_Tree[buddy] == NODE_USED ||   this->m_Tree[buddy] == NODE_FULL))
        {
            index = (index + 1) / 2 - 1;
            this->m_Tree[index] = NODE_FULL;
        }

        else
        {
            return;
        }

    }
}

void CBuddyAllocator::Dump(int index, int level)
{
    VASSERT(m_Tree);
    switch (this->m_Tree[index])
    {
    case NODE_UNUSED:
        syslog_printf("(%d:%d)", _index_offset(index, level, this->GetLevel()) , 1 << (this->GetLevel() - level));
        break;
    case NODE_USED:
        syslog_printf("[%d:%d]", _index_offset(index, level, this->GetLevel()) , 1 << (this->GetLevel() - level));
        break;
    case NODE_FULL:
        syslog_printf("{");
        Dump(index * 2 + 1 , level+1);
        Dump(index * 2 + 2 , level+1);
        syslog_printf("}");
        break;
    default:
        syslog_printf("(");
        Dump(index * 2 + 1 , level+1);
        Dump(index * 2 + 2 , level+1);
        syslog_printf(")");
        break;
    }
}

void CBuddyAllocator::Dump()
{
    this->Dump(0,0);
}

int CBuddyAllocator::GetLevel()
{
    ASSERT(m_Level);
    return *m_Level;
}

int CBuddyAllocator::ShmSize(int level)
{
    return (1<<level)*sizeof(uint8_t)*2 + sizeof(int32_t);
}

int CBuddyAllocator::ShmSize()
{
    return ShmSize(GetLevel());
}
