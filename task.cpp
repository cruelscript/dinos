#include <iostream>
#include "sys.hpp"
#include "rtos_api.hpp"

// добавить задачу перед item, head указывает на голову списка
void InsertTaskBefore(int task, int item, int* head)
{
  if(TaskQueue[task].next != -1)
  {
    return;
  }
  if(item == *head)
  {
    *head = task;
  }
  TaskQueue[task].next = item;
  TaskQueue[task].prev = TaskQueue[item].prev;
  TaskQueue[TaskQueue[item].prev].next = task;
  TaskQueue[item].prev = task;
}

void InsertTaskAfter(int task, int item)
{
  if(TaskQueue[task].next != -1)
  {
    return;
  }
  TaskQueue[task].next = TaskQueue[item].next;
  TaskQueue[task].prev = item;
  TaskQueue[TaskQueue[item].next].prev = task;
  TaskQueue[item].next = task;
}

void RemoveTask(int task, int* head)
{
  if(TaskQueue[task].next == -1)
  {
    return;
  }
  if(*head == task)
  {
    if(TaskQueue[task].next == task)
    {
      *head = -1;
    }
    else
    {
      *head = TaskQueue[task].next;
    }
  }
  TaskQueue[TaskQueue[task].prev].next = TaskQueue[task].next;
  TaskQueue[TaskQueue[task].next].prev = TaskQueue[task].prev;
  TaskQueue[task].next = -1;
  TaskQueue[task].prev = -1;
}

int ActivateTask(TTaskCall entry, int priority, char* name)
{
  int task, occupy;
  std::cout << "ActivateTask " << name << "\n";
  task = HeadTasks[priority];
  occupy = FreeTask;
//изменяем список свободных задач
  RemoveTask(occupy, &FreeTask);
  TaskQueue[occupy].priority = priority;
  TaskQueue[occupy].ceiling_priority = priority;
  TaskQueue[occupy].name = name;
  TaskQueue[occupy].entry = entry;
  TaskQueue[occupy].switch_count = 0;
  TaskQueue[occupy].task_state = TASK_READY;
  TaskCount++;
  std::cout << "End of ActivateTask " << name << "\n";
  Schedule(occupy);
  if(task != HeadTasks[priority])
  {
    Dispatch();
  }
  return occupy;
}

void TerminateTask()
{
  int task;
  TaskCount--;
  task = RunningTask;
  std::cout << "TerminateTask " << TaskQueue[task].name << "\n";
  TaskQueue[task].task_state = TASK_SUSPENDED;
  RemoveTask(task, &(HeadTasks[TaskQueue[task].ceiling_priority]));
// добавляем задачу в список свободных
  InsertTaskBefore(task, FreeTask, &FreeTask);
  std::cout << "End of TerminateTask " << TaskQueue[task].name << "\n";
// задач не осталось, возвращаемся
  if(TaskCount == 0)
  {
    longjmp(MainContext, 1);
  }
  Dispatch();
}

void Schedule(int task)
{
  int priority;
  if(TaskQueue[task].task_state == TASK_SUSPENDED)
  {
    return;
  }
  std::cout << "Schedule " << TaskQueue[task].name << "\n";
  priority = TaskQueue[task].ceiling_priority;
  RemoveTask(task, &(HeadTasks[priority]));
  if(HeadTasks[priority] == -1)
  {
    HeadTasks[priority] = task;
    TaskQueue[task].next = task;
    TaskQueue[task].prev = task;
  }
  else
  {
    InsertTaskAfter(task, TaskQueue[HeadTasks[priority]].prev);
  }
  std::cout << "End of Schedule " << TaskQueue[task].name << "\n";
}

void TaskSwitch(int nextTask)
{
  if(nextTask == -1)
  {
    return;
  }
  if(RunningTask != -1 && TaskQueue[RunningTask].task_state == TASK_RUNNING)
  {
    TaskQueue[RunningTask].task_state = TASK_READY;
  }
  TaskQueue[nextTask].task_state = TASK_RUNNING;
  RunningTask = nextTask;
  TaskQueue[nextTask].switch_count++;
  if(TaskQueue[nextTask].switch_count == 1)
  {
    std::cout << "Dispatch - run task " << TaskQueue[nextTask].name << "\n";
//в первый раз
    TaskQueue[nextTask].entry();
  }
  else
  {
    std::cout << "Dispatch - task switch to " << TaskQueue[nextTask].name << "\n";
//контекст задачи уже существует
    longjmp(TaskQueue[nextTask].context, 1);
  }
}

// резервируем область стека для задачи
void TaskSwitchWithCushion(int nextTask)
{
  char space[100000];
  space[99999] = 1; // отключаем оптимизацию массивов
  TaskSwitch(nextTask);
}

void Dispatch()
{
  if(RunningTask != -1)
  {
    std::cout << "Dispatch - " << TaskQueue[RunningTask].name << "\n";
  }
  else
  {
    std::cout << "Dispatch\n";
  }
  int nextTask = HeadTasks[0];
  int priority = 0;
  while(TaskCount)
  {
    if(nextTask != -1)
    {
      if(TaskCount == 1 && TaskQueue[nextTask].task_state != TASK_READY)
      {
        break;
      }
      if(TaskQueue[nextTask].task_state == TASK_READY || TaskQueue[nextTask].task_state ==
                                                         TASK_RUNNING)
      {
// переключаемся на следующую задачу
        if(RunningTask == -1 || TaskQueue[RunningTask].task_state ==
                                TASK_SUSPENDED)
        {
          TaskSwitch(nextTask);
        }
        else if(RunningTask != nextTask)
        {
          if(!setjmp(TaskQueue[RunningTask].context))
          {
            if(TaskQueue[RunningTask].switch_count == 1)
            {
              TaskSwitchWithCushion(nextTask);
            }
            else
            {
              TaskSwitch(nextTask);
            }
          }
        }
        break;
      }
      nextTask = TaskQueue[nextTask].next;
    }
    if(nextTask == -1 || nextTask == HeadTasks[priority])
    { //переключаемся на следующий приоритет
      priority++;
      if(priority < MAX_PRIORITY)
      {
        nextTask = HeadTasks[priority];
      }
      else
      {
        std::cout << "Error - dont have ready tasks\n";
        longjmp(MainContext, 1);
      }
    }
  }
// добавляем задачу в конец списка
  RemoveTask(nextTask, &(HeadTasks[priority]));
  if(HeadTasks[priority] == -1)
  {
    HeadTasks[priority] = TaskQueue[nextTask].next = TaskQueue[nextTask].prev = nextTask;
  }
  else
  {
    InsertTaskAfter(nextTask, TaskQueue[HeadTasks[priority]].prev);
  }
  std::cout << "End of Dispatch - " << TaskQueue[RunningTask].name << "\n";
}
