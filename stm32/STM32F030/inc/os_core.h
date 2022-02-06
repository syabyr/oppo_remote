/******************************************************************************
 Copyright (c) 2014/01/28 technology.
 All rights reserved.

 [Module Name]: OS_core.h
 [Date]:        28-Jan-2014
 [Comment]:
   kernel core header file.
 [Reversion History]:
*******************************************************************************/
#ifndef __OS_CORE_H__
#define __OS_CORE_H__

#include <stdio.h>
#include "include.h"


#ifdef _OS_CORE_C_
  #define _OS_CORE_DEC_
#else
  #define _OS_CORE_DEC_ extern
#endif


// ****** Public MACRO definition *********************************************
#define OS_TICKS_PER_SEC 	100u /* Set the number of ticks in one second */
#define OS_TICKS_PER_TIK 	1 /* Set the number of ticks in one second */
#define OS_TASK_STAT_EN		1

// ****** Public structure definition *****************************************
typedef  void (*PFN_PV)(void *);

typedef struct _tagOSTaskType
{
	PFN_PV pTask; 				// Pointer to the task (must be a 'void (void *)' function)
	unsigned int  Argument;		// the task input parameter
	unsigned int  Delay;		// Delay (ticks) until the function will (next) be run
	unsigned int  Period;		// Interval (ticks) between subsequent runs.
} OSTaskType;

typedef enum
{
	ID_USART = 0,
	ID_RF868,
	ID_RF433,
	ID_RF315,
	ID_RF24G,
	ID_CRASH,
	ID_TASK_MAX_NUMS
} EnuOSTaskIDType;

// ****** Public Variable type definition *************************************
#if ID_TASK_MAX_NUMS < 9
typedef unsigned char   OS_BYTE_TYPE; /* Unsigned  8 bit quantity */
#elif ID_TASK_MAX_NUMS < 16
typedef unsigned short	OS_BYTE_TYPE; /* Unsigned 16 bit quantity */
#else
typedef unsigned long   OS_BYTE_TYPE; /* Unsigned 32 bit quantity */
#endif


// ****** Private Variable prototypes ******************************************
_OS_CORE_DEC_ OSTaskType OS_tasks_SCH[ID_TASK_MAX_NUMS]; // The array of tasks

// ****** Extern function prototypes ******************************************
_OS_CORE_DEC_ void OSTaskIdleHook(void);
_OS_CORE_DEC_ void SysTick_Enable(void);
_OS_CORE_DEC_ void SysTick_Disable(void);


// ****** Public function prototypes ******************************************
_OS_CORE_DEC_ void  OS_Dispatch_Tasks(void);
_OS_CORE_DEC_ void  OS_Add_Task(unsigned char m_TaskID, PFN_PV Fn_p, unsigned int m_Arg, unsigned int m_Delay, unsigned int m_Period, unsigned char Co_op);
_OS_CORE_DEC_ void  OS_Delete_Task(unsigned char Task_index);
_OS_CORE_DEC_ void  OS_Init_Task(void);
_OS_CORE_DEC_ void  OS_Update_Task(void);


// ****** Public definition prototypes ****************************************
#define IsValidTask(TaskID)		(OS_tasks_SCH[TaskID].pTask != NULL)
#define IsInvalidTask(TaskID)	(OS_tasks_SCH[TaskID].pTask == NULL)
#define OS_Start_Task()		  do{OS_Dispatch_Tasks();}while(1)

// Enter 'idle mode' between clock ticks to save power
#define OS_Goto_Idle()			//OSTaskIdleHook(); /* Call user definable HOOK */

// Start or Stop update task
//#define OS_Start_Update()		TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE)
//#define OS_Stop_Update()		TIM_ITConfig(TIM3, TIM_IT_Update, DISABLE)
#define OS_Start_Update()
#define OS_Stop_Update()

#endif //__OS_CORE_H__
