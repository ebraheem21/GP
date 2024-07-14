#ifndef DCM_H_
#define DCM_H_

#include "DCM_Types.h"

Dcm_NegativeResponseCodeType udsReadDtcInfoSub_0x04(const InfoType *RxData, InfoType *TxData);
Dcm_NegativeResponseCodeType udsReadDtcInfoSub_0x02_0x0A_0x0F_0x13_0x15(const InfoType *RxData, InfoType *TxData);
Dcm_NegativeResponseCodeType DspUdsReadDtcInformation(const InfoType *RxData, InfoType *TxData);

#endif /* DCM_H_ */
