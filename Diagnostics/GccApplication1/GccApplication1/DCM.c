#include <string.h>
#include "DCM_Types.h"
#include "DCM.h"
#include "Dem.h"
#include "stdio.h"




#define BYTES_TO_DTC(hb, mb, lb)	(((uint32)(hb) << 16) | ((uint32)(mb) << 8) | (uint32)(lb))
#define DTC_HIGH_BYTE(dtc)			(((uint32)(dtc) >> 16) & 0xFFu)
#define DTC_MID_BYTE(dtc)			(((uint32)(dtc) >> 8) & 0xFFu)
#define DTC_LOW_BYTE(dtc)			((uint32)(dtc) & 0xFFu)

#define SID_LEN 1
#define SF_LEN 1
#define DTC_LEN 3
#define FF_REC_NUM_LEN 1




static void udsReportDtc(uint32 dtc, uint8 *buffer)
{
	switch( DEM_ISO14229_1 )
	{
	case DEM_ISO14229_1: // UDS
		buffer[0] = DTC_HIGH_BYTE(dtc);
		buffer[1] = DTC_MID_BYTE(dtc);
		buffer[2] = DTC_LOW_BYTE(dtc);
		break;
	default:
		break;
	}
}



Dcm_NegativeResponseCodeType udsReadDtcInfoSub_0x02_0x0A_0x0F_0x13_0x15(const InfoType *RxData, InfoType *TxData)
{
	Dcm_NegativeResponseCodeType responseCode = DCM_E_POSITIVE_RESPONSE;
	Dem_ReturnSetDTCFilterType setDtcFilterResult;



	// Setup the DTC filter
	switch (RxData->DataPtr[1]) 	/** @req DCM378 */
	{
	case 0x02:	// reportDTCByStatusMask
		setDtcFilterResult = Dem_SetDTCFilter(RxData->DataPtr[2]);
		break;
	case 0x0A:	// reportSupportedDTC
		setDtcFilterResult = Dem_SetDTCFilter(DEM_DTC_STATUS_MASK_ALL);
		break;

	default:
		setDtcFilterResult = DEM_WRONG_FILTER;
		break;
	}

	if (setDtcFilterResult == DEM_FILTER_ACCEPTED) {
		uint8 dtcStatusMask;
		//lint --e(826)	PC-Lint exception - Suspicious pointer conversion
		//lint --e(927)	PC-Lint exception - Pointer to pointer cast
		TxDataType *txData = (TxDataType*)TxData->DataPtr;
		Dem_ReturnGetNextFilteredDTCType getNextFilteredDtcResult;
		uint32 dtc;
		Dem_EventStatusExtendedType dtcStatus;
		uint16 nrOfDtcs = 0;
		Std_ReturnType result;

		/** @req DCM377 */
		result = Dem_GetDTCStatusAvailabilityMask(&dtcStatusMask);
		if (result != E_OK) {
			dtcStatusMask = 0;
		}

		// Create positive response (ISO 14229-1 table 252)
		txData-> SID = 0x59;
		txData->reportType = RxData->DataPtr[1];
		txData->dtcStatusAvailabilityMask = dtcStatusMask;

		if (dtcStatusMask != 0x00) {	/** @req DCM008 */
			getNextFilteredDtcResult = Dem_GetNextFilteredDTC(&dtc, &dtcStatus);


			while (getNextFilteredDtcResult == DEM_FILTERED_OK) {
				udsReportDtc(dtc, (uint8*)&txData->dtcAndStatusRecord[nrOfDtcs]);
				txData->dtcAndStatusRecord[nrOfDtcs].statusOfDtc = dtcStatus;
				nrOfDtcs++;
				getNextFilteredDtcResult = Dem_GetNextFilteredDTC(&dtc, &dtcStatus);
			}

			if (getNextFilteredDtcResult != DEM_FILTERED_NO_MATCHING_DTC) {
				responseCode = DCM_E_REQUEST_OUT_OF_RANGE;
			}
		}
		TxData->DataLength = (LengthType)(3 + (nrOfDtcs * sizeof(dtcAndStatusRecordType)));
	}
	else {
		responseCode = DCM_E_REQUEST_OUT_OF_RANGE;
	}

	return responseCode;
}


static Dcm_NegativeResponseCodeType udsReadDtcInfoSub_0x03(const InfoType *RxData, InfoType *TxData)
{
	Dcm_NegativeResponseCodeType responseCode = DCM_E_POSITIVE_RESPONSE;

	uint16 numFilteredRecords = 0;
	uint32 dtc = 0;
	uint8 recordNumber = 0;
	uint16 nofBytesCopied = 0;
	(void)RxData;
	TxData->DataPtr[0] = ( (DEM_FILTER_ACCEPTED == Dem_SetDTCFilterForRecords(&numFilteredRecords)) &&
	( (SID_LEN + SF_LEN + (DTC_LEN + FF_REC_NUM_LEN)*numFilteredRecords) <= TxData->DataLength ));    // subid
	/* @req DCM298 */
	if( (DEM_FILTER_ACCEPTED == Dem_SetDTCFilterForRecords(&numFilteredRecords)) ) {
				
	   for( uint16 i = 0; (i < numFilteredRecords) && (DCM_E_POSITIVE_RESPONSE == responseCode); i++ ) {
	    	/* @req DCM299 */
			
	        if( DEM_FILTERED_OK == Dem_GetNextFilteredRecord(&dtc, &recordNumber) ) {
	        	/* @req DCM300 */
	            TxData->DataPtr[SID_LEN + SF_LEN + nofBytesCopied++] = DTC_HIGH_BYTE(dtc);
	            TxData->DataPtr[SID_LEN + SF_LEN + nofBytesCopied++] = DTC_MID_BYTE(dtc);
	            TxData->DataPtr[SID_LEN + SF_LEN + nofBytesCopied++] = DTC_LOW_BYTE(dtc);
	            TxData->DataPtr[SID_LEN + SF_LEN + nofBytesCopied++] = recordNumber;
	        } else {
	            responseCode = DCM_E_REQUEST_OUT_OF_RANGE;
	        }
	    }
	} else {
	responseCode = DCM_E_REQUEST_OUT_OF_RANGE;
	  }

	TxData->DataPtr[0] = 0x59;    // Positive response
    TxData->DataPtr[1] = 0x03;    // subid
    TxData->DataLength = SID_LEN + SF_LEN + nofBytesCopied;

	return responseCode;
}


// PC-Lint (715 etc): Remove errors until function is filled.
//lint -e{715, 838, 818}		Symbol not referenced, responseCode not used, txData should be const
Dcm_NegativeResponseCodeType udsReadDtcInfoSub_0x04(const InfoType *RxData, InfoType *TxData)
{
	// 1. Only consider Negative Response 0x10

	Dcm_NegativeResponseCodeType responseCode = DCM_E_POSITIVE_RESPONSE;
	uint32 DtcNumber = 0;
	uint8 RecordNumber = 0;
	uint8 SizeOfTxBuf = TxData->DataLength;
	uint8 AvailableBufSize = 0;
	uint8 RecNumOffset = 0;
	uint16 index = 0;
	uint16 EventIndex =0;
	uint16 FFIdNumber = 0;
	Dem_ReturnGetFreezeFrameDataByDTCType GetFFbyDtcReturnCode = DEM_GET_FFDATABYDTC_WRONG_DTC;
	Dem_ReturnGetStatusOfDTCType GetStatusOfDtc = DEM_STATUS_OK;
	Dem_EventStatusExtendedType DtcStatus = 0;
	Dem_EventParameterType *pEventParaTemp = NULL;

	// Now let's assume DTC has 3 bytes.
	DtcNumber = (((uint32)RxData->DataPtr[2])<<16) +
				(((uint32)RxData->DataPtr[3])<<8) +
				((uint32)RxData->DataPtr[4]);

	RecordNumber = RxData->DataPtr[5];

	for (EventIndex = 0; DEM_Config.ConfigSet->EventParameter[EventIndex].Arc_EOL != TRUE; EventIndex++){
		// search each event linked to this DTC
		if( (NULL != DEM_Config.ConfigSet->EventParameter[EventIndex].DTCClassRef) &&
			(DEM_Config.ConfigSet->EventParameter[EventIndex].DTCClassRef->DTC == DtcNumber)){
			pEventParaTemp = (Dem_EventParameterType *)(&DEM_Config.ConfigSet->EventParameter[EventIndex]);
		}
		else {
			pEventParaTemp = NULL;
		}

		if ((pEventParaTemp != NULL) && (NULL != pEventParaTemp->FreezeFrameClassRef)) {

			TxData->DataPtr[6 + RecNumOffset] = RecordNumber;

			// get Dids' number
			for (index = 0; pEventParaTemp->FreezeFrameClassRef[index] != NULL; index++){
				if (pEventParaTemp->FreezeFrameClassRef[index]->FFRecordNumber == RecordNumber) {
					// Calculate the Number of Dids in FF
					for (FFIdNumber = 0; pEventParaTemp->FreezeFrameClassRef[index]->FFIdClassRef[FFIdNumber] != NULL; FFIdNumber++) {
						;
					}
					break;
				}
			}
			TxData->DataPtr[7 + RecNumOffset] = FFIdNumber;

			// get FF data
			AvailableBufSize = SizeOfTxBuf - 7 - RecNumOffset;
			
			GetFFbyDtcReturnCode = Dem_GetFreezeFrameDataByDTC(DtcNumber, RecordNumber, &TxData->DataPtr[8 + RecNumOffset], &AvailableBufSize);
			
			if (GetFFbyDtcReturnCode != DEM_GET_FFDATABYDTC_OK){
				break;
			}
			RecNumOffset = RecNumOffset + AvailableBufSize;

			if( AvailableBufSize > 0 ) {
				TxData->DataLength = 8 + RecNumOffset;
			}
			else {
				TxData->DataLength = 6 + RecNumOffset;
			}
		}
	}

	// Negative response
	switch (GetFFbyDtcReturnCode) {
		case DEM_GET_FFDATABYDTC_OK:
			break;
		default:
			return DCM_E_REQUEST_OUT_OF_RANGE;
	}

	GetStatusOfDtc = Dem_GetStatusOfDTC(DtcNumber, &DtcStatus); /** @req DEM212 */
	switch (GetStatusOfDtc) {
		case DEM_STATUS_OK:
			break;
		default:
			return DCM_E_GENERAL_REJECT;
	}


	// Positive response
	// See ISO 14229(2006) Table 254
	TxData->DataPtr[0] = 0x59;	// positive response
	TxData->DataPtr[1] = 0x04;	// subid
	TxData->DataPtr[2] = RxData->DataPtr[2];	// DTC
	TxData->DataPtr[3] = RxData->DataPtr[3];
	TxData->DataPtr[4] = RxData->DataPtr[4];
	TxData->DataPtr[5] = (uint8)DtcStatus;	//status
	return responseCode;
}


static Dcm_NegativeResponseCodeType udsReadDtcInfoSub_0x06_0x10(const InfoType *RxData, InfoType *TxData)
{
	Dcm_NegativeResponseCodeType responseCode = DCM_E_POSITIVE_RESPONSE;
	Dem_DTCOriginType dtcOrigin;
	uint8 startRecNum;
	uint8 endRecNum;

	// Switch on sub function
	switch (RxData->DataPtr[1]) 	/** @req DCM378 */
	{
		case 0x06:	// reportDTCExtendedDataRecordByDTCNumber
		dtcOrigin = DEM_DTC_ORIGIN_PRIMARY_MEMORY;
		break;

		case 0x10:	// reportMirrorMemoryDTCExtendedDataRecordByDTCNumber
		dtcOrigin = DEM_DTC_ORIGIN_MIRROR_MEMORY;
		break;

		default:
		responseCode = DCM_E_SUB_FUNCTION_NOT_SUPPORTED;
		dtcOrigin = 0;
		break;
	}

	// Switch on record number
	switch (RxData->DataPtr[5])
	{
		case 0xFF:	// Report all Extended Data Records for a particular DTC
		startRecNum = 0x00;
		endRecNum = 0xEF;
		break;

		default:	// Report one specific Extended Data Records for a particular DTC
		startRecNum = RxData->DataPtr[5];
		endRecNum = startRecNum;
		break;
	}

	if (responseCode == DCM_E_POSITIVE_RESPONSE) {
		Dem_ReturnGetStatusOfDTCType getStatusOfDtcResult;
		uint32 dtc;
		Dem_EventStatusExtendedType statusOfDtc;

		dtc = BYTES_TO_DTC(RxData->DataPtr[2], RxData->DataPtr[3], RxData->DataPtr[4]);
		getStatusOfDtcResult = Dem_GetStatusOfDTC(dtc, &statusOfDtc); /** @req DCM295 */ /** @req DCM475 */
		if (getStatusOfDtcResult == DEM_STATUS_OK) {
			Dem_ReturnGetExtendedDataRecordByDTCType getExtendedDataRecordByDtcResult;
			uint8 recNum;
			uint16 recLength;
			uint16 txIndex = 6;

			/** @req DCM297 */ /** @req DCM474 */ /** @req DCM386 */
			TxData->DataPtr[1] = RxData->DataPtr[1];			// Sub function
			udsReportDtc(dtc, &TxData->DataPtr[2]);
			TxData->DataPtr[5] = statusOfDtc;							// DTC status
			for (recNum = startRecNum; recNum <= endRecNum; recNum++) {
				recLength = TxData->DataLength - (txIndex + 1);	// Calculate what's left in buffer
				/** @req DCM296 */ /** @req DCM476 */ /** @req DCM382 */
				getExtendedDataRecordByDtcResult = Dem_GetExtendedDataRecordByDTC(dtc, DEM_DTC_KIND_ALL_DTCS, dtcOrigin, recNum, &TxData->DataPtr[txIndex+1], &recLength);
				if (getExtendedDataRecordByDtcResult == DEM_RECORD_OK && recLength > 0) {
					TxData->DataPtr[txIndex++] = recNum;
					/* Instead of calling Dem_GetSizeOfExtendedDataRecordByDTC() the result from Dem_GetExtendedDataRecordByDTC() is used */
					/** @req DCM478 */ /** @req DCM479 */ /** @req DCM480 */
					txIndex += recLength;
				}
				else {
					// TODO: What to do here?
				}
			}
			TxData->DataLength = txIndex;
		}
		else {
			responseCode = DCM_E_REQUEST_OUT_OF_RANGE;
		}
	}

	return responseCode;
}

Dcm_NegativeResponseCodeType DspUdsReadDtcInformation(const InfoType *RxData, InfoType *TxData)
{
	/** @req DCM248 */
	// Sub function number                0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F 10 11 12 13 14 15
	static const uint8 DataLength[0x16] = {0, 3, 3, 2, 6, 3, 6, 4, 4, 5, 2, 2, 2, 2, 2, 3, 6, 3, 3, 3, 2, 2};

	Dcm_NegativeResponseCodeType responseCode = DCM_E_POSITIVE_RESPONSE;

	uint8 subFunctionNumber = RxData->DataPtr[1];

	// Check length
	if (subFunctionNumber <= 0x15) {
		if (RxData->DataLength == DataLength[subFunctionNumber]) {
			switch (subFunctionNumber)
			{
			case 0x01:	// reportNumberOfDTCByStatusMask
			case 0x07:	// reportNumberOfDTCBySeverityMaskRecord
			case 0x11:	// reportNumberOfMirrorMemoryDTCByStatusMask
			case 0x12:	// reportNumberOfEmissionRelatedOBDDTCByStatusMask
				break;

			case 0x02:	// reportDTCByStatusMask
			case 0x0A:	// reportSupportedDTC
			case 0x0F:	// reportMirrorMemoryDTCByStatusMask
			case 0x13:	// reportEmissionRelatedOBDDTCByStatusMask
			case 0x15:	// reportDTCWithPermanentStatus
							responseCode = udsReadDtcInfoSub_0x02_0x0A_0x0F_0x13_0x15(RxData, TxData);
				break;

			case 0x08:	// reportDTCBySeverityMaskRecord
				break;

			case 0x09:	// reportSeverityInformationOfDTC
				break;

			case 0x06:	// reportDTCExtendedDataRecordByDTCNumber
			case 0x10:	// reportMirrorMemoryDTCExtendedDataRecordByDTCNumber
						responseCode = udsReadDtcInfoSub_0x06_0x10(RxData, TxData);
				break;

			case 0x03:	// reportDTCSnapshotIdentidication
				responseCode = udsReadDtcInfoSub_0x03(RxData, TxData);

				break;

			case 0x04:	// reportDTCSnapshotByDtcNumber
				responseCode = udsReadDtcInfoSub_0x04(RxData, TxData);

				break;

			case 0x05:	// reportDTCSnapshotRecordNumber
				break;

			case 0x0B:	// reportFirstTestFailedDTC
			case 0x0C:	// reportFirstConfirmedDTC
			case 0x0D:	// reportMostRecentTestFailedDTC
			case 0x0E:	// reportMostRecentConfirmedDTC

				break;

			case 0x14:	// reportDTCFaultDetectionCounter

				break;

			default:
				// Unknown sub function
				responseCode = DCM_E_REQUEST_OUT_OF_RANGE;
				break;
			}
		}
		else {
			// Wrong length
			responseCode = DCM_E_INCORRECT_MESSAGE_LENGTH_OR_INVALID_FORMAT;
		}
	}
	else {
		// Sub function out of range
		responseCode = DCM_E_REQUEST_OUT_OF_RANGE;
	}
	return responseCode;
}


