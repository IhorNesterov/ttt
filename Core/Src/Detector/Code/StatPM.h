
#ifndef __STATPM_H__
#define __STATPM_H__

///////////////////////////////////////////////////////////////////////////////
// STATPM V1.0 22/07/2021  ////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

//#include "GlobalData.h"
#include <stdio.h>
#include <stdint.h>


#define St_Alarm    0x01        // 1 - Превышение порога поискового алгоритма
                                // 0 - Норма
#define St_Call     0x02        // 1 - Режим Калибровка
                                // 0 - Режим Поиск
// Status_Stat
typedef struct {
  uint8_t Status;   // Flag Status
  uint8_t nSigm;    // Количество превышение над порогом ср.-их отклонений для управления сигнализацией
  float     CPS;    // Текущая скорость счета
  uint8_t Delta;    //  Статистическая погрешность [2..99] %
} TStatus_Stat; //CPS - значение


// Function
void Stat_Init(float nSigm, uint16_t tCall, float tau);
void Stat_GetStatus(TStatus_Stat *Status);
void Stat_AddData250ms(int32_t Ni);
void Stat_Reset(void);
void Stat_Start_Call(void);

#endif /* __STATPM_H__ */
 