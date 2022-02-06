/********************************************************************
* /// SY.LIN SCHEDULER CORE ///                                     *
*                                                                   *
* *** THESE ARE THE CORE SCHEDULER FUNCTIONS ***                    *
* --- These functions may be used with all devices ---              *
*                                                                   *
* *** ID_TASK_MAX_NUMS must be set by the user ***                  *
* --- see "OS_CORE.H" ---                                           *
*********************************************************************/
#define _OS_CORE_C_

#include "os_core.h"

// Set bit to 0 if task is idlesse, Set bit to 1 if task is active
OS_BYTE_TYPE OSTaskRuning = 0;

// Set bit to 0 if task is cooperate, Set bit to 1 if task is anticipate
OS_BYTE_TYPE OSTaskOccupy = 0;

/******************************************************************************
* This is the 'dispatcher' function. When a task (function) is due to run,    *
* OS_Dispatch_Tasks() will run it.                                            *
* This function must be called (repeatedly) from the main loop.               *
*******************************************************************************/
void OS_Dispatch_Tasks(void)
{
	static unsigned char Index; // for task ID index
	if (OSTaskRuning)
	{
		// Dispatches (runs) the next task (if one is ready)
		for (Index = 0; Index < ID_TASK_MAX_NUMS; Index++)
		{
			// Only dispatching cooperate tasks
			if ((OSTaskRuning & (1 << Index)) && (!(OSTaskOccupy & (1 << Index))))
			{	
				(*OS_tasks_SCH[Index].pTask)(&OS_tasks_SCH[Index].Argument); // Run the task
				OSTaskRuning &= ~(1 << Index); // Reset/reduce corresponding task ID flag
				// Periodic tasks will automatically run again
				// if this is a 'one shot' task, remove it from the array
				if (OS_tasks_SCH[Index].Period == 0)
				{
					OS_tasks_SCH[Index].pTask = NULL; // Faster than call to delete task
				}
			}
		}
	}
	else
	{
		OS_Goto_Idle();
	}
}

/******************************************************************************
* Causes a task (function) to be executed at regular intervals                *
* or after a user-defined delay                                               *
*                                                                             *
* m_TaskID:If the value is >= ID_TASK_MAX_NUMS then the task could not be     *
*          added to the array.                                                *
*          If the value is < ID_TASK_MAX_NUMS, then the task was added        *
*          successfully.                                                      *
* Fn_P:    The name of the function which is to be scheduled.                 *
* m_Delay: The interval (TICKS) before the task is first executed             *
* Period:  If Period is 0, the function is only called once,                  *
*          If Period is non-zero, then the function is called repeatedly      *
*          at an interval determined by the vakue of Rep.                     *
*                                                                             *
* 注意: 当该函数对已经存在的任务, 根据该有效任务号(ID)进行自重新追加调用,     *
*       并且把该追加任务函数放在本任务函数中进行自追加调用时,                 *
*       应将(Fn_p)设为NULL, 且m_Period不能设为零, 从而可以避免递归调用问题    *
*******************************************************************************/
void OS_Add_Task(unsigned char m_TaskID, PFN_PV Fn_p, unsigned int m_Arg, unsigned int m_Delay, unsigned int m_Period, unsigned char Co_op)
{
	if (m_TaskID < ID_TASK_MAX_NUMS)
	{
		OS_Stop_Update();

		if (m_Delay == 0)
		{
			OSTaskRuning |= (1 << m_TaskID);
			m_Delay = m_Period;
		}
		else
		{
			OSTaskRuning &= ~(1 << m_TaskID);
		}

		if (Co_op)
			OSTaskOccupy |= (1 << m_TaskID);
		else
			OSTaskOccupy &= ~(1 << m_TaskID);

		OS_tasks_SCH[m_TaskID].Argument = m_Arg;
		OS_tasks_SCH[m_TaskID].Delay  = m_Delay;
		OS_tasks_SCH[m_TaskID].Period = m_Period;

		if (Fn_p != NULL)
		{
			OS_tasks_SCH[m_TaskID].pTask = Fn_p;
		}

		OS_Start_Update();
	}
}

/********************************************************************
* Removes a task from the scheduler.  Note that this does           *
* not delete the associated function from memory:                   *
* it simply means that it is no longer called by the scheduler.     *
* PARAMS: Task_index - The task index. Provided by OS_Add_Task().   *
*********************************************************************/
void OS_Delete_Task(unsigned char Task_index)
{
	if (Task_index < ID_TASK_MAX_NUMS)
	{
		if (OS_tasks_SCH[Task_index].pTask)
		{
   			OS_tasks_SCH[Task_index].pTask = NULL;
			OS_tasks_SCH[Task_index].Argument = 0;
			OS_tasks_SCH[Task_index].Period = 0;
			OS_tasks_SCH[Task_index].Delay  = 0;

			OSTaskRuning &= ~(1 << Task_index);
			OSTaskOccupy &= ~(1 << Task_index);
		}
	}
}

/********************************************************************
* Scheduler initialisation function.  Prepares scheduler            *
* data structures and sets up timer interrupts at required rate.    *
* You must call this function before using the scheduler.           *
*                                                                   *
* NOTE: THE SCHEDULER INTERRUPT SHOULD BE DISABLED!!!               *
*********************************************************************/
void OS_Init_Task(void)
{
	int i;

	/************************ Timer Stop ***********************/
	OS_Stop_Update();

	/********************** All Tasks delete *******************/
	for (i = 0; i < ID_TASK_MAX_NUMS; i++)
	{
		OS_Delete_Task(i);
	}
}

/********************************************************************
* Function    : Uses SysTick Handler for timing ticks               *
* Description : It is called, interrupt happen when SysTick_Handler *
* (precise) tick interval = 20 ms                                   *
*********************************************************************/
void OS_Update_Task(void)
{
	int Index;

	// NOTE: calculations are in *TICKS* (not milliseconds)
	for (Index = 0; Index < ID_TASK_MAX_NUMS; Index++)
	{
		if (OS_tasks_SCH[Index].pTask)
       	{
			if (OS_tasks_SCH[Index].Delay == 0)
			{
				if (OSTaskOccupy & (1 << Index))
				{
					// If it is a anticipate task, run it IMMEDIATELY
					(*OS_tasks_SCH[Index].pTask)(&OS_tasks_SCH[Index].Argument); // Run the task

					OSTaskRuning &= ~(1 << Index); // Reset/reduce corresponding task ID flag

					// Periodic tasks will be scheduled again (see below)
					// if this is a 'one shot' task, remove it from the array
					if (OS_tasks_SCH[Index].Period == 0)
					{
						OS_tasks_SCH[Index].pTask = NULL;
					}
				}
				else
				{
					// If it is a cooperate task, set corresponding task ID flag
					OSTaskRuning |= (1 << Index);
				}

				if (OS_tasks_SCH[Index].Period > 0)
				{
					// Schedule this periodic task to run again
					OS_tasks_SCH[Index].Delay = OS_tasks_SCH[Index].Period - 1;
				}
			}
			else
			{
				// Not yet ready to run: just decrement the delay
				OS_tasks_SCH[Index].Delay -= 1;
			}
		}
	}
}

/********************************************************************
* -------------------------- END OF FILE -------------------------- *
*********************************************************************/
