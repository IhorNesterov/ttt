#include "FindX.h"

//****************************************************************************//
//  22/07/21 Find Window5
//  Putyrski Mikhail
//****************************************************************************//

static float Find_Sigma;
static uint16_t TFW5_Tcall;

// Time Mask
//const unsigned int iMaskFind = 0x0088;
const unsigned int iMaskFind = 0xFFFF;

const float Kst[121] = {
              12.7062	,
              4.3027	,
              3.1824	,
              2.7764	,
              2.5706	,
              2.4469	,
              2.3646	,
              2.306	,
              2.2622	,
              2.2281	,
              2.201	,
              2.1788	,
              2.1604	,
              2.1448	,
              2.1314	,
              2.1199	,
              2.1098	,
              2.1009	,
              2.093	,
              2.086	,
              2.0796	,
              2.0739	,
              2.0687	,
              2.0639	,
              2.0595	,
              2.0555	,
              2.0518	,
              2.0484	,
              2.0452	,
              2.0423	,
              2.0301	,
              2.0301	,
              2.0301	,
              2.0301	,
              2.0301	,
              2.0301	,
              2.0301	,
              2.0301	,
              2.0301	,
              2.0211	,
              2.0211	,
              2.0211	,
              2.0211	,
              2.0211	,
              2.0141	,
              2.0141	,
              2.0141	,
              2.0141	,
              2.0141	,
              2.0086	,
              2.0086	,
              2.0086	,
              2.0086	,
              2.0086	,
              2.004	,
              2.004	,
              2.004	,
              2.004	,
              2.004	,
              2.0003	,
              2.0003	,
              2.0003	,
              2.0003	,
              2.0003	,
              2.0003	,
              2.0003	,
              2.0003	,
              2.0003	,
              2.0003	,
              1.9944	,
              1.9944	,
              1.9944	,
              1.9944	,
              1.9944	,
              1.9944	,
              1.9944	,
              1.9944	,
              1.9944	,
              1.9944	,
              1.9901	,
              1.9901	,
              1.9901	,
              1.9901	,
              1.9901	,
              1.9901	,
              1.9901	,
              1.9901	,
              1.9901	,
              1.9901	,
              1.9867	,
              1.9867	,
              1.9867	,
              1.9867	,
              1.9867	,
              1.9867	,
              1.9867	,
              1.9867	,
              1.9867	,
              1.9867	,
              1.984	,
              1.984	,
              1.984	,
              1.984	,
              1.984	,
              1.984	,
              1.984	,
              1.984	,
              1.984	,
              1.984	,
              1.9818	,
              1.9818	,
              1.9818	,
              1.9818	,
              1.9818	,
              1.9818	,
              1.9818	,
              1.9818	,
              1.9818	,
              1.9818	,
              1.9799	,
              1	
};



//****************************************************************************//

TFW5Data FW5Data;
uint16_t  _out;
void FW5_CalculatePorog_i(float dt, float nSigma, float nFona_cps, float TCAL_sec);
void FW5_SetPorog(float nSigm, uint16_t Tcall);

//****************************************************************************//

float ssqrt(float z) {
 if (z==0) return 0;
 union {float f; uint32_t i; } val = {z};
 val.i -= 8388608;
 val.i >>= 1;
 val.i += 536870912;
 return val.f;
}

// Write Buff
void FW5_Write(uint32_t D) {
  FW5Data.CountWindow+=D;
}

void FW5_SetPorog(float nSigm, uint16_t Tcall) {
     TFW5_Tcall = Tcall;
     Find_Sigma = nSigm;
}


// Callibration
void FW5_Calibration(void) {
  
   FW5Data.Valid=0;
   FW5Data.Nt = 0;
   FW5Data.Work = TFW5_bStartCallibration;
   FW5_Clear( );
   FW5Data.Valid=0;
   
}

// Read
void FW5_Read(void) {  
  
  static uint16_t i;
 
  switch (FW5Data.Work) {
    
    case TFW5_bWork :
     
      if (++i>TFW5_NintSet) { 
        
        //
        i=0;
        
        // Read Cyclic Buffer
        FW5Data.BuffCount[FW5Data.WriteIndex] = FW5Data.CountWindow; 

        FW5_Clear( );
     
        // Inc WriteIndex
        ++FW5Data.WriteIndex;
        FW5Data.WriteIndex&=TFW5_BuffClear;
        
        // Valid
        if (FW5Data.Valid==0) {    
          if (FW5Data.Nt++>TFW5_SizeOfBuff) FW5Data.Valid=1;
        }
        
       }
        
       break;
        
    case TFW5_bCallibration :
      
        ++FW5Data.Nt;
        
        if (FW5Data.Nt>TFW5_Tcall) {
          
            // Time
            FW5Data.T = (float)FW5Data.Nt;
            FW5Data.T *=(float)TFW5_dt;
            
            // Window
            FW5Data.CPS = (float)FW5Data.CountWindow;
            FW5Data.CPS /= FW5Data.T;
            FW5_CalculatePorog_i(TFW5_T,Find_Sigma,FW5Data.CPS,FW5Data.T);
               
            // Clear
            if (FW5Data.Work == TFW5_bCallibration) {
                  FW5_Clear( ); 
                  FW5Data.Work = TFW5_bWork;
                  FW5Data.Nt = 0;
                  FW5Data.Valid = 0;
                  FW5Data.ReadIndex  = 0;
                  FW5Data.WriteIndex =  FW5Data.ReadIndex;
                  i=0;
            }
            
        } 
        break;
    default :
        FW5Data.Valid = 0;
        FW5Data.Work = TFW5_bCallibration;
        FW5Data.Nt = 0;
        i=0;
        FW5_Clear( );
    break;       
  }     
}


// Clear Buff
void FW5_Clear(void) {
  // Clear Buffer
  FW5Data.CountWindow=0;
}

// Bool Find Mode 
bool FW5_Valid(void) {
 return FW5Data.Valid;   
}


// Calibration 
void FW5_CalculatePorog_i(float dt, float nSigma, float nFona_cps, float TCAL_sec) {
  
  uint8_t i;
  float Time;
  float K1,K2;

  // Fon j-win; i-step.
  FW5Data.Dt_nFona_cps = nFona_cps*dt;
 
  // K2
  K2 = (float)nFona_cps;
  K2/= TCAL_sec;
  
  // Время
  Time=dt;
  
  for(i=0;i<TFW5_SizeOfBuff;i++) {
    
      K1    = (float)nFona_cps;
      K1   /= Time; 
      K1   += K2;
      K1    = ssqrt(K1);
      K1   *= (nSigma+0.1);
      K1   *= dt;
      
      FW5Data.Dt_RF_PorogCPS[i]=K1;
      
      // Next Step
      Time += dt;
  }
  
}


// Поисковый алгоритм       
void FW5_Find(uint16_t *Out, float *CRM) {
  
 uint8_t i;
 uint8_t ReadIndex; 
 uint32_t Ni;
 uint32_t SNi;
 uint32_t Result;
 uint16_t  TimeMask;
 uint16_t  IndexResult;
 
 static uint16_t  SDTi;
 static float  Sours_cpsi;
 static float  Porog_cpsi;
 static float  K, Kmax;
 
 
 // Чтение данных 
 ReadIndex  = FW5Data.ReadIndex;
  
 // Инициализация алгоритма
 Result=0; SNi=0; SDTi=1; K=0; Kmax=0;
 
 // Анализ первой ячейки интегрирования
 IndexResult=1; 
 TimeMask=iMaskFind; 
 
 // Расчет
 for (i=0;i<TFW5_SizeOfBuff;i++) {
   
      // Расчет индекса 
      --ReadIndex; 
      ReadIndex &=TFW5_BuffClear;
      
      // Ni
      Ni=FW5Data.BuffCount[i];
      
      // SNi
      SNi+=Ni;
      
      // Time Mask 
      if (TimeMask&0x0001) { // Maska
        
      // Сравнение с порогом
      if (SNi>4) {
              // Расчет Sours_cpsi  
              Sours_cpsi = (float)SNi;
              Sours_cpsi /= SDTi; 
              if (Sours_cpsi > FW5Data.Dt_nFona_cps) {
                  // Вычитание фона
                  Sours_cpsi -= FW5Data.Dt_nFona_cps;
                  // Porog
                  Porog_cpsi = FW5Data.Dt_RF_PorogCPS[i];
                  Porog_cpsi *= (SNi>120) ? Kst[120] : Kst[SNi];
                  //  Сравнение с порогом   
                  if (Sours_cpsi>Porog_cpsi) { 
                    // Данные по превышению  
                    Result |=IndexResult;
                    // Максимальное значение превышения скорости счета
                    if ((Result&8)==8) {
                          K  = (float) Sours_cpsi;
                          K /= Porog_cpsi;
                          if (K>Kmax) Kmax = K;   
                    } // Buzer
                  }  // >Porog 
              } // > Fon[j], j-окно  
          } // SNi>4
      } // Maska
      
      // Следующая ячейка интегрирования
      SDTi +=1;
      TimeMask>>=1;
      IndexResult<<=1; 
 }
 
 // Вывод
 *CRM  = Kmax;
 *Out=Result;
}


// Поисковый алгоритм       
uint8_t FW5_G5Find(uint16_t *Out, float *CRM) {
  
  float _crm,crm_max;
  uint16_t out_max; 
  
  crm_max = 1; out_max  = 0;
  *Out = out_max; *CRM = crm_max;
   
  if (FW5Data.Valid==0) return 0; 
 
  // Если есть данные  
  if (FW5Data.ReadIndex == FW5Data.WriteIndex) return 0; 
  
  // Чтение данных   
  FW5Data.ReadIndex  = FW5Data.WriteIndex;  
  

  FW5_Find(&_out,&_crm);
  out_max |= _out; 
  if (_crm>crm_max) crm_max =_crm;   
  

  // Out
  *Out = out_max;
  *CRM = crm_max;
  
  return 1;

}


