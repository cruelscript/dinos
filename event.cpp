#include <iostream>
#include "sys.hpp"
#include "rtos_api.hpp"

void SetSysEvent(TEventMask mask)
{
  std::cout << "SetSysEvent " << mask <<  "\n";
  WorkingEvents |= mask;
  for(auto& i: TaskQueue)
  {
    if(i.task_state == TASK_WAITING &&
       (WorkingEvents & i.waiting_events))
    {
      i.waiting_events &= !mask;
      i.task_state = TASK_READY;
      std::cout << "Task \"" << i.name << "\" is ready\n";
    }
  }
  WorkingEvents &= !mask;
  Dispatch();
  std::cout << "End of SetSysEvent " << mask << "\n";
}

void GetSysEvent(TEventMask* mask)
{
  *mask = WorkingEvents;
}

void WaitSysEvent(TEventMask mask)
{
  std::cout << "WaitSysEvent " << mask << "\n";
  TaskQueue[RunningTask].waiting_events = mask;
  if((WorkingEvents & mask) == 0)
  {
    TaskQueue[RunningTask].task_state = TASK_WAITING;
    Dispatch();
  }
  std::cout << "End of WaitSysEvent " << mask << "\n";
}
