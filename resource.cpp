#include <iostream>
#include "sys.hpp"
#include "rtos_api.hpp"

void InitPVS(TSemaphore S, const std::string& name)
{
  std::cout << "InitPVS " << name << "\n";
  ResourceQueue[S].name = name;
  ResourceQueue[S].block = 0;
}

void P(TSemaphore S)
{
  std::cout << "P " << ResourceQueue[S].name << "\n";
  while(ResourceQueue[S].block)
  { // ресурс заблокирован
    std::cout << "Resource is blocked\n";
    TaskQueue[RunningTask].task_state = TASK_WAITING;
    TaskQueue[RunningTask].waited_resource = S;
    Dispatch();
  }
  ResourceQueue[S].block = 1;
  std::cout << "End of P " <<  ResourceQueue[S].name << "\n";
}

void V(TSemaphore S)
{
  std::cout << "V " <<  ResourceQueue[S].name << "\n";
  ResourceQueue[S].block = 0;
  for(auto& task: TaskQueue)
  {
    if(task.task_state == TASK_WAITING && task.waited_resource == S)
    {
      task.task_state = TASK_READY;
      task.waited_resource = -1;
    }
  }
  Dispatch();
  std::cout << "End of V " << ResourceQueue[S].name << "\n";
}
