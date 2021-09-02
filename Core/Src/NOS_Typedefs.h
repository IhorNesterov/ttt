#ifndef NOS_TYPEDEFS
#define NOS_TYPEDEFS
#include <stdint.h>
/* Unions begin */

typedef union Short_t //uint16_t data wrap
{ 
    uint8_t bytes[2];
    uint16_t data;
} NOS_Short;

typedef union Float_t //float data wrap
{
    uint8_t bytes[4];
    float data;
} NOS_Float;



/* Unions end */

/* Enums begin */

typedef enum ModBusState_t{Free,ReceiveFromMaster,ReceiveFromSlave,TransmitToMaster,TransmitToSlave} ModBusState;

/* Enums end */

/* Structures begin */

/*WS2812B Matrix*/
typedef struct PixelColor_t
{
   uint8_t R;
   uint8_t G;
   uint8_t B;
} PixelColor;

typedef struct MatrixSize_t
{
  uint8_t col;
  uint8_t row;
} MatrixSize;

typedef struct  Symvol_t
{
  uint8_t data[5];
} Symvol;


typedef struct WS2812B_Matrix_t
{
    uint8_t* buffer;
    NOS_Short ledsCount;
    MatrixSize* size;
    PixelColor* textColor;
    PixelColor* foneColor;
    Symvol* symvols;
} WS2812B_Matrix;

/*WS2812B Matrix*/

/*ModBus*/

typedef struct ModBus_Master_Command_t
{
    uint8_t type; 
    uint8_t Addr;
    uint8_t Command;
    uint8_t Byte_Count;
    uint16_t Reg_Addr;
    uint16_t Reg_Count;
    NOS_Short ShortValue;
    NOS_Float FloatValue;
} ModBus_Master_Command;

typedef struct ModBus_Slave_Command_t
{
    uint8_t type;
    uint8_t Addr;
    uint8_t Command;
    uint8_t Byte_Count;
    NOS_Short ShortValue;
    NOS_Float FloatValue;
} ModBus_Slave_Command;


/*ModBus*/

/* Structures end */
#endif