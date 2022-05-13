#include <iostream>
#include "sys.hpp"
#include "rtos_api.hpp"

DeclareTask(Task1, 4);
DeclareTask(Task2, 2);
DeclareTask(Task3, 3);
DeclareTask(Task5, 2);
DeclareTask(Task6, 1);
DeclareTask(Task7, 2);
DeclareTask(Task8, 3);
DeclareTask(Task9, 11);
DeclareTask(Task10, 7);
DeclareTask(Task11, 4);
DeclareTask(Task12, 4);
DeclareSysEvent(1);
DeclareSysEvent(2);
DeclareSysEvent(3);

int main()
{
  std::cout << "DinOS!\n";
  InitPVS(1, "Res1");
  InitPVS(2, "Res2");
  StartOS(Task5, Task5prior, "Task5");
  ShutdownOS();
  return 0;
}

// +
// 4
TASK(Task1)
{
  std::cout << "Start Task 1\n";
  ActivateTask(Task2, Task2prior, "Task2");
  std::cout << "Task1\n";
  TerminateTask();
}
// 2
TASK(Task2)
{
  std::cout << "Start Task 2\n";
  ActivateTask(Task3, Task3prior, "Task3");
  Dispatch();
  std::cout << "Task2\n";
  TerminateTask();
}
// 3
TASK(Task3)
{
  std::cout << "Start Task3\n";
  Dispatch();
  std::cout << "Task3\n";
  TerminateTask();
}
//-------------------------------------

// 2
TASK(Task5)
{
  std::cout << "Start Task5\n";
  P(1);
  ActivateTask(Task6, Task6prior, "Task6");
  V(1);
  std::cout << "Task5\n";
  TerminateTask();
}
// 1
TASK(Task6)
{
  std::cout << "Start Task6\n";
  P(1);
  V(1);
  std::cout << "Task6\n";
  TerminateTask();
}
//-------------------------------------

// +
// 11
TASK(Task9)
{
  std::cout << "Start Task9\n";
  P(1);
  ActivateTask(Task10, Task10prior, "Task10");
  P(2);
  V(1);
  V(2);
  std::cout << "Task9\n";
  TerminateTask();
}
// 7
TASK(Task10)
{
  std::cout << "Start Task10\n";
  P(2);
  P(1);
  V(1);
  V(2);
  std::cout << "Task10\n";
  TerminateTask();
}
//-------------------------------------

// 2
int Task7_ID;

TASK(Task7)
{
  std::cout << "Start Task7\n";
  Task7_ID = RunningTask;
  ActivateTask(Task8, Task8prior, "Task8");
  WaitSysEvent(Event_1);
  TEventMask event;
  GetSysEvent(&event);
  std::cout << "GetEvent= " << event << "\n";
  std::cout << "Task7\n";
  TerminateTask();
}
// 3
TASK(Task8)
{
  std::cout << "Start Task8\n";
  SetSysEvent(Event_1);
  std::cout << "Task8\n";
  TerminateTask();
}
