#include "Detector.h"

static float Detector_Kg =_default_KG;
static float Detector_Bg =_default_BG;

// Setting Param Detector
void  Detector_Init_Param(float Set_Kg, float Set_Bg) {
 Detector_Kg = Set_Kg;
 Detector_Bg = Set_Bg;
}

// Get Value [uSv/h]
float Detector_GetuZvValue(TStatus_Stat* Stat) {
  float DER;
  DER  = (Stat->CPS>Detector_Bg) ? (Stat->CPS-Detector_Bg) : (Stat->CPS);
  DER *= Detector_Kg;
  return DER; // [uSv/h]
}





