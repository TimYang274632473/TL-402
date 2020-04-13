#include "includes.h"

osThreadId Task1Handle;
osThreadId Task2Handle;
osThreadId Task3Handle;
osThreadId Task4Handle;
osThreadId Task5Handle;

osTimerId				AutoReloadTimerHandle_50mS;

//二值信号量：用于任务之间同步、中断与任务同步
SemaphoreHandle_t		Semap_Gps;
SemaphoreHandle_t		Semap_PcCmd;								// 互斥二值信号量句柄

QueueHandle_t			Queue_GSM;									// 队列句柄
QueueHandle_t			Queue_SPI;									// 队列句柄

EventGroupHandle_t		EventGroupHandler_MCU;						// 事件标志组句柄
EventGroupHandle_t		EventGroupHandler_OTH;						// 事件标志组句柄

void vApplicationIdleHook(void);

void vApplicationIdleHook( void )
{
	__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
	HAL_PWR_EnterSLEEPMode(PWR_LOWPOWERREGULATOR_ON,PWR_SLEEPENTRY_WFI);
}


void MX_FREERTOS_Init(void) 
{
	taskENTER_CRITICAL();           								// 进入临界区

	Semap_Gps	  = xSemaphoreCreateBinary();							// 创造互斥二值信号量,用于串口中断接收标志
	Semap_PcCmd	= xSemaphoreCreateBinary();							// 创造互斥二值信号量,用于串口中断接收标志

	Queue_GSM	= xQueueCreate(Queue_GSM_Num, Queue_GSM_Len ); 		// 创建消息队列
	Queue_SPI	= xQueueCreate(Queue_SPI_Num, Queue_SPI_Len ); 		// 创建消息队列
	
	EventGroupHandler_MCU = xEventGroupCreate();	 				// 创建事件标志组	
	EventGroupHandler_OTH = xEventGroupCreate();	 				// 创建事件标志组	
	
	osTimerDef(AutoReloadTimer_50mS, app_timer_50ms_callback);		// 创建定时器任务
	AutoReloadTimerHandle_50mS = osTimerCreate(osTimer(AutoReloadTimer_50mS), osTimerPeriodic, NULL);
	
	osThreadDef(Task1, App_Task1, osPriorityHigh, 0, 256);
	Task1Handle = osThreadCreate(osThread(Task1), NULL);

	osThreadDef(Task2, App_Task2, osPriorityAboveNormal, 0, 256);
	Task2Handle = osThreadCreate(osThread(Task2), NULL);

	osThreadDef(Task3, App_Task3, osPriorityAboveNormal, 0, 256);
	Task3Handle = osThreadCreate(osThread(Task3), NULL);

	osThreadDef(Task4, App_Task4, osPriorityNormal, 0, 256);
	Task4Handle = osThreadCreate(osThread(Task4), NULL);

	osThreadDef(Task5, App_Task5, osPriorityNormal, 0, 256);
	Task5Handle = osThreadCreate(osThread(Task5), NULL);

	osTimerStart(AutoReloadTimerHandle_50mS,50);
	
	taskEXIT_CRITICAL();            								// 退出临界区
	
	HAL_NVIC_SetPriority(EXTI0_1_IRQn, 7, 0);
	HAL_NVIC_EnableIRQ(EXTI0_1_IRQn);

	HAL_NVIC_SetPriority(EXTI2_3_IRQn, 7, 0);
	HAL_NVIC_EnableIRQ(EXTI2_3_IRQn);
	
	HAL_NVIC_SetPriority(EXTI4_15_IRQn, 7, 0);
	HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);
	
	__HAL_UART_ENABLE_IT(&huart1,  UART_IT_RXNE);
	__HAL_UART_ENABLE_IT(&huart2,  UART_IT_RXNE);
	__HAL_UART_ENABLE_IT(&hlpuart1,UART_IT_RXNE);
	__HAL_UART_ENABLE_IT(&huart4,  UART_IT_RXNE);
}


