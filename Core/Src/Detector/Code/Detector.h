#ifndef _DETECTOR_H_
#define _DETECTOR_H_

#include "StatPM.h"

// Param Detector

#define _default_KG    (float) (1.395E-3)       // Чувствительность [мкЗв/[ИМП/Сек]] по Cs137
#define _default_BG    0.0                     //  Фон детектора [ИМП/Сек] (CPS)

void  Detector_Init_Param(float Set_Kg, float Set_Bg);    // Setting Param Detector
float Detector_GetuZvValue(TStatus_Stat* Stat);           // Get Value [uS/h] ([мкЗв/ч])


/* Example Use */

// Инициализация
// Stat_Init(1.395E-3,0.0); // Kg = 1.395E-3 - Чувствительность детектора ; Bg=0.0 - Внутренний Фон детектора [CPS]

// Вывод
// DER = Detector_GetuZvValue(&Status); //   Вывод МЭД:  [uSv/h]

/* End Example Use */


#endif /* _DETECTOR_H_ */
