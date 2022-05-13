#ifndef RTOS_API_HPP
#define RTOS_API_HPP

#include "sys.hpp"
#include <string>

#define DeclareTask(TaskID, priority)\
TASK(TaskID); \
enum {TaskID##prior=(priority)}
#define TASK(TaskID) void TaskID(void)

typedef void TTaskCall();

int ActivateTask(TTaskCall entry, int priority, char* name);

void TerminateTask();

int StartOS(TTaskCall entry, int priority, char* name);

void ShutdownOS();

void InitPVS(TSemaphore S, const std::string& name);

void P(TSemaphore S);

void V(TSemaphore S);

#define DeclareSysEvent(ID) \
const int Event_##ID = (ID)*(ID);

void SetSysEvent(TEventMask mask);

void GetSysEvent(TEventMask* mask);

void WaitSysEvent(TEventMask mask);

#endif // RTOS_API_HPP
