#ifndef _CHARGE_H_
#define _CHARGE_H_

#include "common.h"

#define CHARGE_START        1
#define CHARGE_STOP         0

#define CHARGE_MODE_FAST    1
#define CHARGE_MODE_NOMAL   0

#define BATTERY_START       1
#define BATTERY_STOP        0


/** ISL6251A  Driver structure */
typedef struct
{  
  int    AdapterStatus;
  uint8_t u8VoltLevel;
  uint16_t u16ReptVolt;
  void   (*Init)(void);
  int    (*Detect_Adapter)(void);
  void   (*Set_Status)(int);
  void   (*Set_Mode)(int);
  void   (*Supply_Ctrl)(int); 
}T_DrvChargeFunc, *PT_DrvChargeFunc;


void OnAdapter_Handler(void);
void OffAdapter_Handler(void);
void ChargeProtect(void);
void Get_Battery_Preference(void);
uint16_t Get_Battery_Volt(void);
uint8_t Get_Battery_Level(void);

#endif //_CHARGE_H_
