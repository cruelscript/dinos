#ifndef SYS_HPP
#define SYS_HPP

#include "defs.hpp"
#include <csetjmp>
#include <string>

enum TTaskState
{
  TASK_RUNNING,
  TASK_READY,
  TASK_SUSPENDED,
  TASK_WAITING
};

typedef unsigned int TEventMask;
typedef int TSemaphore;

typedef struct Type_Task
{
  int next, prev;
  int priority;
  int ceiling_priority;

  void (* entry)();

  char* name;
  TTaskState task_state;
  int switch_count;
  jmp_buf context;
  TSemaphore waited_resource;
  TEventMask waiting_events;
} TTask;

typedef struct Type_resource
{
  volatile int block;
  std::string name;
} TResource;

extern TTask TaskQueue[MAX_TASK];
extern TResource ResourceQueue[MAX_RES];
// ������� ������������� ������, �������� ������ � ������� TaskSwitch
extern int RunningTask;
extern int HeadTasks[MAX_PRIORITY]; // ������ ������ � ������
extern int TaskCount;
extern int FreeTask;
extern TEventMask WorkingEvents;// ����������� �������
extern jmp_buf MainContext; // ������� ��������
void Schedule(int task);

void Dispatch();

#endif // SYS_HPP
