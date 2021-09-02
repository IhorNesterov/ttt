#include "StatPM.h"
#include "stat.h"
#include "FindX.h"

////////////////////////////////////////////////////////////////////////////////
// Function                                                                   //
////////////////////////////////////////////////////////////////////////////////

static float  CPS;
static int8_t Delta;
uint16_t FindChange=0;

// Stat
void Stat_Init(float nSigm, uint16_t tCall, float tau) {
  float timen = (float) tCall/TimeBase_Sec;
  uint16_t Time = (uint16_t)timen;
  FW5_SetPorog(nSigm,Time);
  stat_SetDeatTime(tau);
  FW5_Calibration( );
}

// Reset Stat
void Stat_Reset(void) {
 stat_reset ( );
}

// Start Callibration
void Stat_Start_Call(void) {
 FW5_Calibration( );
}

// Add Data
void Stat_AddData250ms(int32_t Ni) {
  //  Add Data
  stat_add((stat_pulses_t)Ni,(stat_time_t)TimeBase);
  stat_check(0x01);
  // Find
  FW5_Write(Ni);
  FW5_Read( );
  FindChange = 1;
}

// Get Status
void Stat_GetStatus(TStatus_Stat *Status) {

   float Kmax;
   uint16_t Temp=0;

   // Measure
   stat_calc_cps(&CPS,&Delta);

   Status->CPS   = CPS;
   Status->Delta = Delta;

   // Find
   if  (FindChange==1) {
          FindChange=0;
          if (FW5_G5Find(&Temp,&Kmax)) {
             Status->Status &=~St_Call;
             if (Temp>0) {
                  Status->Status |= St_Alarm;
                  Status->nSigm   = Kmax;
             } else {
                 Status->Status &=  ~St_Alarm;
                 Status->nSigm = 0;
               }
          } else Status->Status |=St_Call;;
   }

}
 