#include "paxosstorage.h"
#include "syslog.h"

CPaxosStorage::CPaxosStorage()
{
    this->InitBasic();
}
CPaxosStorage::~CPaxosStorage()
{
    this->Destroy();
}
status_t CPaxosStorage::InitBasic()
{
    //add your code
    return OK;
}
status_t CPaxosStorage::Init()
{
    this->Destroy();
    //add your code
    return OK;
}
status_t CPaxosStorage::Destroy()
{
    //add your code
    this->InitBasic();
    return OK;
}
