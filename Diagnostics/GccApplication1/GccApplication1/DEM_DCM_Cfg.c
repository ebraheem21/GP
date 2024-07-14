#include "DEM.h"
#include "DCM.h"
#include "string.h"
#include "lm_sensor.h"
///////////////////DEM////////////////

// 10.2.21 DemPreDebounceCounterBased
const Dem_PreDebounceCounterBasedType preDebounceCounter = {
	.JumpUp=TRUE,
	.JumpDown=TRUE,
	.CountInStepSize=10,
	.CountOutStepSize=10
	}; /** @req DEM144 */



	// 10.2.20
const Dem_PreDebounceAlgorithmClassType DebounceAlgorithm = {
		.PreDebounceName = DEM_PRE_DEBOUNCE_COUNTER_BASED,				// (1)
		.PreDebounceCounterBased = &preDebounceCounter		// (0..1)

	};

//Event 1
static const Dem_EventClassType EventClassTypes[] =
{
		{
				.ConsiderPtoStatus = FALSE,
				.EventDestination[0] = DEM_DTC_ORIGIN_PRIMARY_MEMORY,
				.EventPriority = 1,
				.FFPrestorageSupported = FALSE,
				.OperationCycleRef = DEM_POWER,
				.ConfirmationCycleCounterThreshold = 3,
				.PreDebounceAlgorithmClass = &DebounceAlgorithm,
				.IndicatorAttribute = NULL
		},
		{
				.ConsiderPtoStatus = FALSE,
				.EventDestination[0] = DEM_DTC_ORIGIN_PRIMARY_MEMORY,
				.EventPriority = 1,
				.FFPrestorageSupported = FALSE,
				.OperationCycleRef = DEM_POWER,
				.ConfirmationCycleCounterThreshold = 3,
				.PreDebounceAlgorithmClass = &DebounceAlgorithm,
				.IndicatorAttribute = NULL
		},
		{
				.ConsiderPtoStatus = FALSE,
				.EventDestination[0] = DEM_DTC_ORIGIN_PRIMARY_MEMORY,
				.EventPriority = 1,
				.FFPrestorageSupported = FALSE,
				.OperationCycleRef = DEM_POWER,
				.ConfirmationCycleCounterThreshold = 3,
				.PreDebounceAlgorithmClass = &DebounceAlgorithm,
				.IndicatorAttribute = NULL
		}
};

static const Dem_ExtendedDataRecordClassType ExtendedDataRecordClasses[] =
{
		{
				.RecordNumber = 1,
				.DataSize = 4,
				.UpdateRule = DEM_UPDATE_RECORD_YES,
				.CallbackGetExtDataRecord = NULL,
				.InternalDataElement = DEM_OCCCTR
		},
		{
				.RecordNumber = 2,
				.DataSize = 4,
				.UpdateRule = DEM_UPDATE_RECORD_YES,
				.CallbackGetExtDataRecord = NULL,
				.InternalDataElement = DEM_OCCCTR
		},
		{
				.RecordNumber = 3,
				.DataSize = 4,
				.UpdateRule = DEM_UPDATE_RECORD_YES,
				.CallbackGetExtDataRecord = NULL,
				.InternalDataElement = DEM_OCCCTR
		}
};

const Dem_ExtendedDataRecordClassType* ExtendedData_Engine[] =
{
	&ExtendedDataRecordClasses[0],
};
const Dem_ExtendedDataRecordClassType* ExtendedData_Oil[] =
{
	&ExtendedDataRecordClasses[1],
};
const Dem_ExtendedDataRecordClassType* ExtendedData_Brakes[] =
{
	&ExtendedDataRecordClasses[2],
};


Std_ReturnType ReadTMP(uint8 *Data) {
	uint8 tmp = LM35_getTemperature();
	*Data = (uint8)(tmp);
	return E_OK;
}

Std_ReturnType ReadRPM(uint8 *Data) {
	uint8 rpm = 0x41;
	*Data = (uint8)(rpm);
	return E_OK;
}

Std_ReturnType ReadAllParameters(uint8 *Data) {
	uint8 offset = 0;
	uint8 parameterData;  // Changed from pointer to uint8 variable

	// Read TMP
	ReadTMP(&parameterData);
	Data[offset++] = parameterData;
	
	// Read RPM
	ReadRPM(&parameterData);
	Data[offset++] = parameterData;

	return E_OK;
}


Std_ReturnType GetReadDataLength(uint16 *DidLength) {
	*DidLength = 2; // Total size of tmp 1 byte , rpm 1  byte
	return E_OK;
}



Std_ReturnType MyDidConditionCheckRead(Dcm_NegativeResponseCodeType *Nrc)
{
	// Example check: replace this with your actual condition
		if (1) {
		*Nrc = DCM_E_POSITIVE_RESPONSE;
		return E_OK;
		} 
		else {
			if (Nrc != NULL) {
			*Nrc = DCM_E_CONDITIONS_NOT_CORRECT;
			}
			return E_NOT_OK;
		}
}


static const Dem_DidType DidTypes[] =
{
		{
				.DidConditionCheckReadFnc = MyDidConditionCheckRead,
				.DidIdentifier = 0x1234,
				.DidReadDataLengthFnc = GetReadDataLength,
				.DidReadFnc =  ReadAllParameters,
				.PidOrDidSize = 2,
				.PidOrDidUsePort = FALSE
		},
		{
				.DidConditionCheckReadFnc = MyDidConditionCheckRead,
				.DidIdentifier = 0x5678,
				.DidReadDataLengthFnc = GetReadDataLength,
				.DidReadFnc =  ReadAllParameters,
				.PidOrDidSize = 2,
				.PidOrDidUsePort = FALSE
		},
		{
				.DidConditionCheckReadFnc = MyDidConditionCheckRead,
				.DidIdentifier = 0x2653,
				.DidReadDataLengthFnc = GetReadDataLength,
				.DidReadFnc =  ReadAllParameters,
				.PidOrDidSize = 2,
				.PidOrDidUsePort = FALSE
		}
};

const Dem_DidType* Did_Engine[] =
{
	&DidTypes[0],
	NULL,
};
const Dem_DidType* Did_Oil[] =
{
	&DidTypes[1],
	NULL,
};
const Dem_DidType* Did_Brakes[] =
{
	&DidTypes[2],
	NULL,
};


static const Dem_FreezeFrameClassType FreezeFrameClasses[] =
{
		{
				.FFKind = DEM_FREEZE_FRAME_NON_OBD,
				.FFRecordNumber = 1,
				.FFStorageCondition = DEM_FF_STORAGE_FAILED,
				.FFIdClassRef = Did_Engine
		},
		{
				.FFKind = DEM_FREEZE_FRAME_NON_OBD,
				.FFRecordNumber = 2,
				.FFStorageCondition = DEM_FF_STORAGE_FAILED,
				.FFIdClassRef = Did_Oil
		},
		{
				.FFKind = DEM_FREEZE_FRAME_NON_OBD,
				.FFRecordNumber = 3,
				.FFStorageCondition = DEM_FF_STORAGE_FAILED,
				.FFIdClassRef = Did_Brakes
		}
};

const Dem_FreezeFrameClassType* FreezeFrame_Engine[] =
{
	&FreezeFrameClasses[0],
	&FreezeFrameClasses[1],
	&FreezeFrameClasses[2],
};
const Dem_FreezeFrameClassType* FreezeFrame_Oil[] =
{
	&FreezeFrameClasses[0],
	&FreezeFrameClasses[1],
	&FreezeFrameClasses[2],
};




static const Dem_DTCClassType DTCClassTypes[] =
{
		{
				.DTC = 0x016E10,
				.DTCFunctionalUnit = 1,
				.DTCKind = DEM_DTC_KIND_ALL_DTCS
		},
		{
				.DTC = 0x016F11,
				.DTCFunctionalUnit = 1,
				.DTCKind = DEM_DTC_KIND_ALL_DTCS
		},
};





// Define event parameters
const Dem_EventParameterType DemEventParameters[]= {
	{
        .EventID = DEM_EVENT_ID_ENGINE_OVERHEAT,
        .EventKind = DEM_EVENT_KIND_BSW,
        .EventClass = &EventClassTypes[0], // Add proper reference if needed
        .ExtendedDataClassRef = ExtendedData_Engine,
        .FreezeFrameClassRef = FreezeFrame_Engine,
        .CallbackInitMForEFnc = NULL, // Add proper callback if needed
        .DTCClassRef = &DTCClassTypes[0],
        .Arc_EOL = FALSE
    },
    {
        .EventID = DEM_EVENT_ID_LOW_OIL_PRESSURE,
        .EventKind = DEM_EVENT_KIND_BSW,
        .EventClass = &EventClassTypes[1], // Add proper reference if needed
        .ExtendedDataClassRef = ExtendedData_Oil,
        .FreezeFrameClassRef = FreezeFrame_Oil,
        .CallbackInitMForEFnc = NULL, // Add proper callback if needed
        .DTCClassRef = &DTCClassTypes[1],
        .Arc_EOL = FALSE
    },

};

// Define configuration set
const Dem_ConfigSetType DemConfigSet = {
    .EventParameter = DemEventParameters,
    };

// Define global configuration
const Dem_ConfigType DEM_Config = {
    .ConfigSet = &DemConfigSet
};



///////////////////DCM//////////////////////


/*uint8 arr[]={0x7f,0x02,0x01};

const InfoType DATA[] =
{
		{
		.DataPtr = arr,
		.DataLength = 6
		}
};*/
