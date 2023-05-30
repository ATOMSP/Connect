#ifndef __CON_PORT_H__
#define __CON_PORT_H__



#ifdef __cplusplus
extern "C"{
#endif

#if defined(USE_CONNECT)

/* HAL lib */
#include "Platform.h"
#if defined(USE_HAL)
/* HAL Receive dat Callback */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){
  if(huart->Instance == LOG_USART){
    RecProcess(GetConInfo());
    if(!GetConInfo()->Isclose){HAL_UART_Receive_IT(huart,(uint8_t*)&(GetConInfo()->rec),0x01);}
  }
}
/* HAL Receive dat Init */
#define HAVEN_HAL_CONNECT_INIT
void HAL_Connect_Init(){
  HAL_UART_Receive_IT(LOG_HUARTX,(uint8_t*)&(GetConInfo()->rec),0x01);
}
/* Log Macro func */
#define ConnectLog(format,...) printf(format,##__VA_ARGS__)
#else
/* other platform */

/*
  Interrupt(void){
    RecProcess(GetConInfo());
    ...
  }
  #define HAVEN_HAL_CONNECT_INIT
  platform usart enable func
  void HAL_Connect_Init(){
    ...
  }
*/
#endif

#endif


#ifdef __cplusplus
}
#endif












#endif



