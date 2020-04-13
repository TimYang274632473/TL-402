#include "includes.h"

/*	PC USART1	*/
void App_Task4(void const * argument)
{
	while(1)
	{
		//PC下发指令解析
		if(Semap_PcCmd != NULL)
		{
			if(osSemaphoreWait(Semap_PcCmd,portMAX_DELAY) == osOK)
			{
				PC_Cmd_Check();
			}
		}
		else
		{
			vTaskDelay(1000); 
		}
	}
}



