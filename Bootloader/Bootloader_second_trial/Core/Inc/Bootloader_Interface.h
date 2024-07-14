

#ifndef INC_BOOTLOADER_INTERFACE_H_
#define INC_BOOTLOADER_INTERFACE_H_

#include "Bootloader_Private.h"

/* ------------------------ Bootloader Functions ----------------------*/
BL_Status BL_FeatchHostCommand();
/* ----------------- Software Interfaces Declarations -----------------*/
void BL_SendMsg(char *format,...);


#endif
