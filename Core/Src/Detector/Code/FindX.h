#ifndef __FINDX_H__
#define __FINDX_H__

//#include "types.h"
#include <stdint.h>
#include <stdbool.h>
#include "stat.h"


#define TFW5_Nint        1              //  1 min
#define TFW5_NintSet     (TFW5_Nint-1)
#define TFW5_dt          (float)TimeBase_Sec
//0.25
#define TFW5_T           (float)(TFW5_dt*TFW5_Nint)

#define TFW5_SizeOfBuff  16 
#define TFW5_BuffClear   (uint16_t)(TFW5_SizeOfBuff-1)
#define TFW5_Time_Add    (float)(TFW5_SizeOfBuff)*TFW5_dt

#define TFW5_bCallibration        0x0000 
#define TFW5_bWork                0x0001
#define TFW5_bStartCallibration   0x0002




typedef struct {  // FIND WINDOW
  
  uint16_t  Work;
  
  uint32_t CountWindow; 
 
  float  Dt_nFona_cps;
  float  Dt_RF_PorogCPS[TFW5_SizeOfBuff];
  uint32_t  BuffCount[TFW5_SizeOfBuff];  // uint32_t
  
  uint16_t  Nt;
  uint8_t WriteIndex;
  uint8_t ReadIndex;
  uint16_t  Valid;
  
  float  CPS;
  float  T;
      
} TFW5Data;


void FW5_Write(uint32_t D);
void FW5_SetPorog(float nSigm, uint16_t Tcall);
void FW5_Clear(void);
void FW5_Read(void);
void FW5_Calibration(void);
bool FW5_Valid(void);
uint8_t FW5_G5Find(uint16_t *Out, float *CRM);



#endif // __FINDX_H__
