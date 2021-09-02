#ifndef NOS_MODBUS
#define NOS_MODBUS
#include "stdint.h"
#include "NOS_Typedefs.h"

uint16_t GetCRC16 (uint8_t *nData, uint8_t wLength);
void NOS_ModBus_ParseMasterCommand(ModBus_Master_Command* master,uint8_t* buff,uint8_t offset);
void NOS_ModBus_ParseSlaveCommand(ModBus_Slave_Command* slave,uint8_t* buff,uint8_t offset);

void NOS_ModBus_SetMasterCommand(ModBus_Master_Command* master,uint8_t addr,uint8_t command,uint16_t regAddr, uint16_t data);
void NOS_ModBus_SetSlaveCommand(ModBus_Slave_Command* slave,uint8_t addr,uint8_t command,uint8_t byteCount,uint8_t typ,NOS_Short sVal,NOS_Float fVal);

#endif 