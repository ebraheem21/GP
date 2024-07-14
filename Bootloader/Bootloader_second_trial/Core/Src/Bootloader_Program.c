
/* ----------------- Includes -----------------*/

#include "string.h"
#include "stdarg.h"
#include "stdio.h"

#include "usart.h"
#include "crc.h"
#include "gpio.h"

#include "Bootloader_Interface.h"
#include "Bootloader_Config.h"
#include "Bootloader_Private.h"

/* ----------------- Global Variables Definitions -----------------*/
static uint8_t HostBuffer[BL_HOST_BUFFER_RX_LENGTH];
static uint8_t appExists = 0;

static uint8_t BL_Version[4] = {CBL_VENDOR_ID,
		CBL_SW_MAJOR_VERSION,
		CBL_SW_MINOR_VERSION,
		CBL_SW_PATCH_VERSION
};

static uint8_t BL_Supported_CMDs[6] = {CBL_GET_VER_CMD,
		CBL_GET_HELP_CMD,
		CBL_GET_CID_CMD,
		CBL_GO_TO_ADDR_CMD,
		CBL_FLASH_ERASE_CMD,
		CBL_MEM_WRITE_CMD
};

/* -----------------  Software Interfaces Definitions -----------------*/
BL_Status BL_FeatchHostCommand()
{
	BL_Status Status = BL_NACK;
	HAL_StatusTypeDef Hal_status = HAL_ERROR;
	uint8_t DataLength = 0;
	memset(HostBuffer,0,BL_HOST_BUFFER_RX_LENGTH);
	/*Receive the length of the packet from the host*/
	Hal_status = HAL_UART_Receive(BL_HOST_COMMUNICATION_UART_RECEIVE,HostBuffer,1,HAL_MAX_DELAY);

	if(Hal_status != HAL_OK)
	{
		Status = BL_NACK;
	}
	else
	{
		DataLength = HostBuffer[0];
		/*Receive the whole packet from the Host*/
		Hal_status = HAL_UART_Receive(BL_HOST_COMMUNICATION_UART_RECEIVE,&HostBuffer[1],DataLength,HAL_MAX_DELAY);

		if(Hal_status != HAL_OK)
		{
			Status = BL_NACK;
		}
		else
		{
			switch(HostBuffer[1])
			{
			case CBL_GET_VER_CMD:
				BL_GetVersion(HostBuffer);
				Status = BL_ACK;
				break;
			case CBL_GET_HELP_CMD:
				BL_GetHelp(HostBuffer);
				Status = BL_ACK;
				break;
			case CBL_GET_CID_CMD:
				BL_GetChipIdentificationNumber(HostBuffer);
				Status = BL_ACK;
				break;
			case CBL_GO_TO_ADDR_CMD:
				bootloader_jump_to_user_app(HostBuffer);
				Status = BL_ACK;
				break;
			case CBL_FLASH_ERASE_CMD:
				BL_FlashErase(HostBuffer);
				Status = BL_ACK;
				break;
			case CBL_MEM_WRITE_CMD:
				BL_FlashWrite(HostBuffer);
				Status = BL_ACK;
				break;
			}
		}
	}
	return Status;
}

void BL_SendMsg(char *format,...)
{
	char Msg[100] = {};
	va_list args;
	va_start(args,format);
	vsprintf(Msg,format,args);
#if BL_DEBUG_METHOD == BL_UART_DEBUG_MESSAGE
	HAL_UART_Transmit(BL_DEBUG,(uint8_t*)Msg,sizeof(Msg),HAL_MAX_DELAY);
#elif BL_DEBUG_METHOD == BL_USB_DEBUG_MESSAGE
	CDC_Transmit_FS((uint8_t*)&Msg,va_arg(arg, int));
#endif
	va_end(args);

}

/* ----------------- Static Functions Definitions -----------------*/
void BL_GetVersion(uint8_t *Host_Buffer)
{
	uint16_t HostPacketLength = 0;
	uint32_t CRC_Value = 0;
	HostPacketLength = Host_Buffer[0]+1;
	CRC_Value = *((uint32_t*)(Host_Buffer+HostPacketLength -4));
	if(CRC_VERIFYING_SUCCESS == BL_CRC_Verify((uint8_t*)&Host_Buffer[0],HostPacketLength-4,CRC_Value))
	{
		//BL_Send_ACK(4);
		Bootloader_Send_Data_To_Host((uint8_t *)(&BL_Version[0]), 4);
	}
	else
	{
		BL_Send_NACK();
	}
}

void BL_GetHelp(uint8_t *Host_Buffer)
{
	uint16_t HostPacketLength = 0;
	uint32_t CRC_Value = 0;
	HostPacketLength = Host_Buffer[0]+1;
	CRC_Value = *(uint32_t*)(Host_Buffer+HostPacketLength -4);
	if(CRC_VERIFYING_SUCCESS == BL_CRC_Verify((uint8_t*)&Host_Buffer[0],HostPacketLength-4,CRC_Value))
	{
		BL_Send_ACK(6);
		Bootloader_Send_Data_To_Host((uint8_t *)(&BL_Supported_CMDs[0]),6);
	}
	else
	{
		BL_Send_NACK();
	}
}

void BL_GetChipIdentificationNumber(uint8_t *Host_Buffer)
{
	uint16_t Chip_ID = 0;
	uint16_t HostPacketLength = 0;
	uint32_t CRC_Value = 0;
	HostPacketLength = Host_Buffer[0]+1;
	CRC_Value = *((uint32_t*)((Host_Buffer+HostPacketLength) -4));
	if(CRC_VERIFYING_SUCCESS == BL_CRC_Verify((uint8_t*)&Host_Buffer[0],HostPacketLength-4,CRC_Value))
	{
		Chip_ID = (uint16_t)(DBGMCU->IDCODE & 0x00000FFF);

		Bootloader_Send_Data_To_Host((uint8_t *)&Chip_ID, 2);
	}
	else
	{
		BL_Send_NACK();
	}
}

void BL_JumpToAddress(uint8_t *Host_Buffer){
	uint16_t Host_CMD_Packet_Len = 0;
	uint32_t Host_CRC32 = 0;
	uint32_t HOST_Jump_Address = 0;
	uint8_t Address_Verification = ADDRESS_IS_INVALID;

	/* Extract the CRC32 and packet length sent by the HOST */
	Host_CMD_Packet_Len = Host_Buffer[0] + 1;
	Host_CRC32 = *((uint32_t *)((Host_Buffer + Host_CMD_Packet_Len) - 4));
	/* CRC Verification */
	if(CRC_VERIFYING_SUCCESS == BL_CRC_Verify((uint8_t *)&Host_Buffer[0] , Host_CMD_Packet_Len - 4, Host_CRC32))
	{
		BL_Send_ACK(1);
		/* Extract the address form the HOST packet */
		HOST_Jump_Address = *((uint32_t *)&Host_Buffer[2]);
		/* Verify the Extracted address to be valid address */
		Address_Verification = BL_AddressVerification(HOST_Jump_Address);
		if(ADDRESS_IS_VALID == Address_Verification)
		{
			/* Report address verification succeeded */
			Bootloader_Send_Data_To_Host((uint8_t *)&Address_Verification, 1);
			/* Prepare the address to jump */
			Jump_Ptr Jump_Address = (Jump_Ptr)(HOST_Jump_Address + 1);

			Jump_Address();
		}
		else
		{
			/* Report address verification failed */
			Bootloader_Send_Data_To_Host((uint8_t *)&Address_Verification, 1);
		}
	}
	else
	{
		BL_Send_NACK();
	}
}


void BL_FlashErase(uint8_t *Host_Buffer)
{
	uint8_t Erase_Status = UNSUCCESSFUL_ERASE;
	uint16_t HostPacketLength = 0;
	uint32_t CRC_Value = 0;
	HostPacketLength = Host_Buffer[0]+1;
	CRC_Value = *(uint32_t*)(Host_Buffer+HostPacketLength -4);
	if(CRC_VERIFYING_SUCCESS == BL_CRC_Verify((uint8_t*)&Host_Buffer[0],HostPacketLength-4,CRC_Value))
	{
		Erase_Status = PerformFlashErase(2,4);
		if(SUCCESSFUL_ERASE == Erase_Status)
		{
			Bootloader_Send_Data_To_Host((uint8_t *)&Erase_Status, 1);
		}
		else
		{
			Bootloader_Send_Data_To_Host((uint8_t *)&Erase_Status, 1);
		}
	}
	else
	{
		BL_Send_NACK();
	}
}

uint16_t i = 0;
void BL_FlashWrite(uint8_t *Host_Buffer)
{
	uint8_t AddressStatus = ADDRESS_IS_INVALID;
	uint8_t Payload_Status = FLASH_PAYLOAD_WRITE_FAILED;
	uint16_t HostPacketLength = 0;
	static uint32_t HostAddress = 0;
	uint32_t HostDataLength = 0;
	uint32_t CRC_Value = 0;

	HostPacketLength = Host_Buffer[0]+1;
	CRC_Value = *(uint32_t*)(Host_Buffer+HostPacketLength -4);
	//if(CRC_VERIFYING_SUCCESS == BL_CRC_Verify((uint8_t*)&Host_Buffer[0],HostPacketLength-4,CRC_Value))
	//{
	//HostAddress = 0x08008000 + 64*i;
	HostAddress = *((uint32_t*)&Host_Buffer[2]) + 64*i;
	i++;
	HostDataLength = Host_Buffer[6];
	AddressStatus = BL_AddressVerification(HostAddress);
	if(AddressStatus == ADDRESS_IS_VALID)
			{
				Payload_Status = FlashMemory_PayloadWrite((uint8_t*)&Host_Buffer[7],HostAddress,HostDataLength);
				if(Payload_Status == FLASH_PAYLOAD_WRITE_SUCCESS)
				{
					Bootloader_Send_Data_To_Host((uint8_t *)&Payload_Status, 1);
				}
				else
				{
					Bootloader_Send_Data_To_Host((uint8_t *)&Payload_Status, 1);
				}
			}
			else
			{
				AddressStatus = ADDRESS_IS_INVALID;
				Bootloader_Send_Data_To_Host((uint8_t *)&AddressStatus, 1);
			}
		}
		/*else
		{
			BL_Send_NACK();
		}*/
	//}
uint32_t BL_CRC_Verify(uint8_t *Data, uint32_t DataLength, uint32_t HostCRC)
{
	uint8_t CRC_Status = CRC_VERIFYING_FAILED;
	uint32_t MCU_CRC = 0;
	uint32_t DataBuffer = 0;
	for(uint8_t Count = 0; Count < DataLength; Count++)
	{
		DataBuffer = (uint32_t)Data[Count];
		MCU_CRC = HAL_CRC_Accumulate(&hcrc,&DataBuffer,1);
	}
	__HAL_CRC_DR_RESET(&hcrc);
	if(HostCRC == MCU_CRC)
	{
		CRC_Status = CRC_VERIFYING_SUCCESS;
	}
	else
	{
		CRC_Status = CRC_VERIFYING_FAILED;
	}

	return CRC_Status;
}

uint8_t PerformFlashErase(uint8_t SectorNumber, uint8_t NumberOfSectors)
{
	FLASH_EraseInitTypeDef pEraseInit;
	uint32_t SectorError = 0;
	HAL_StatusTypeDef Hal_Status = HAL_ERROR;
	uint8_t SectorStatus = INVALID_SECTOR_NUMBER;
	uint8_t RemainingSectors = 0;
	if(NumberOfSectors > CBL_FLASH_MAX_SECTOR_NUMBER)
	{
		SectorStatus = INVALID_SECTOR_NUMBER;
	}
	else
	{
		SectorStatus = VALID_SECTOR_NUMBER;
		if((NumberOfSectors <= (CBL_FLASH_MAX_SECTOR_NUMBER - 1)) || (SectorNumber == CBL_FLASH_MASS_ERASE))
		{
			if(SectorNumber == CBL_FLASH_MASS_ERASE)
			{
				pEraseInit.TypeErase = FLASH_TYPEERASE_MASSERASE;
			}
			else
			{
				RemainingSectors = CBL_FLASH_MAX_SECTOR_NUMBER - SectorNumber;
				if(NumberOfSectors > RemainingSectors)
				{
					NumberOfSectors = RemainingSectors;
				}
				else
				{
					/* Do Nothing */
				}
				pEraseInit.TypeErase = FLASH_TYPEERASE_SECTORS;
				pEraseInit.Sector = SectorNumber;
				pEraseInit.NbSectors = NumberOfSectors;
			}
			pEraseInit.Banks = FLASH_BANK_1;
			pEraseInit.VoltageRange = FLASH_VOLTAGE_RANGE_3;

			Hal_Status = HAL_FLASH_Unlock();

			Hal_Status = HAL_FLASHEx_Erase(&pEraseInit, &SectorError);

			if(SectorError == HAL_SUCCESSFUL_ERASE)
			{
				SectorStatus = SUCCESSFUL_ERASE;
			}
			else
			{
				SectorStatus = UNSUCCESSFUL_ERASE;
			}
			Hal_Status = HAL_FLASH_Lock();
		}
		else
		{
			SectorStatus = UNSUCCESSFUL_ERASE;
		}
	}
	return SectorStatus;
}

uint8_t BL_AddressVerification(uint32_t Address)
{
	uint8_t Address_Status = ADDRESS_IS_INVALID;
	if(Address >= FLASH_BASE && Address <= STM32F401_FLASH_END)
	{
		Address_Status = ADDRESS_IS_VALID;
	}
	else if(Address >= SRAM_BASE && Address <= STM32F401_SRAM_END)
	{
		Address_Status = ADDRESS_IS_VALID;
	}
	else
	{
		Address_Status = ADDRESS_IS_INVALID;
	}
	return Address_Status;
}

void bootloader_jump_to_user_app(uint8_t *Host_Buffer)
{
	uint16_t HostPacketLength = 0;
	uint32_t CRC_Value = 0;
	HostPacketLength = Host_Buffer[0]+1;
	CRC_Value = *((uint32_t*)((Host_Buffer+HostPacketLength) -4));
	if(CRC_VERIFYING_SUCCESS == BL_CRC_Verify((uint8_t*)&Host_Buffer[0],HostPacketLength-4,CRC_Value))
	{
		if(0xFFFFFFFF != *((volatile uint32_t *)FLASH_SECTOR2_BASE_ADDRESS))
		{
			appExists = 1;
			Bootloader_Send_Data_To_Host((uint8_t *)&appExists, 1);
			/* Value of the main stack pointer of our main application */
			uint32_t MSP_Value = *((volatile uint32_t *)FLASH_SECTOR2_BASE_ADDRESS);

			/* Reset Handler definition function of our main application */
			uint32_t MainAppAddr = *((volatile uint32_t *)(FLASH_SECTOR2_BASE_ADDRESS + 4));

			/* Fetch the reset handler address of the user application */
			pMainApp ResetHandler_Address = (pMainApp)MainAppAddr;

			/* DeInitialize / Disable of modules */
			HAL_RCC_DeInit(); /* DeInitialize the RCC clock configuration to the default reset state. */

			/* Set Main Stack Pointer */
			__set_MSP(MSP_Value);

			/* Jump to Application Reset Handler */
			ResetHandler_Address();
		}
		else
		{
			appExists = 0;
			Bootloader_Send_Data_To_Host((uint8_t *)&appExists, 1);
		}
	}
	else
	{
		BL_Send_NACK();
	}
}

uint8_t FlashMemory_PayloadWrite(uint8_t *Data, uint32_t StartingAddress, uint16_t PayloadLength)
{
	HAL_StatusTypeDef HAL_Status = HAL_ERROR;
	uint8_t Payload_Status = FLASH_PAYLOAD_WRITE_FAILED;

	HAL_Status = HAL_FLASH_Unlock();
	if(HAL_Status != HAL_OK)
	{
		Payload_Status = FLASH_PAYLOAD_WRITE_FAILED;
	}
	else
	{
		for(uint16_t Counter = 0; Counter < PayloadLength; Counter++)
		{
			HAL_Status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE,StartingAddress + Counter ,Data[Counter]);
			if(HAL_Status != HAL_OK)
			{
				Payload_Status = FLASH_PAYLOAD_WRITE_FAILED;
				break;
			}
			else
			{
				Payload_Status = FLASH_PAYLOAD_WRITE_SUCCESS;
			}
		}
	}
	if((Payload_Status == FLASH_PAYLOAD_WRITE_SUCCESS) && (HAL_Status == HAL_OK))
	{
		HAL_Status = HAL_FLASH_Lock();
		if(HAL_Status != HAL_OK)
		{
			Payload_Status = FLASH_PAYLOAD_WRITE_FAILED;
		}
		else
		{
			Payload_Status = FLASH_PAYLOAD_WRITE_SUCCESS;
		}
	}
	else
	{
		Payload_Status = FLASH_PAYLOAD_WRITE_FAILED;
	}

	return Payload_Status;
}
void BL_Send_ACK(uint8_t DataLength)
{
	uint8_t ACK_Value[2] = {0};
	ACK_Value[0] = SEND_ACK;
	ACK_Value[1] = DataLength;
	HAL_UART_Transmit(BL_HOST_COMMUNICATION_UART_SEND,(uint8_t*)ACK_Value,2,HAL_MAX_DELAY);
}

void BL_Send_NACK()
{
	uint8_t ACK_Value = SEND_NACK;
	HAL_UART_Transmit(BL_HOST_COMMUNICATION_UART_SEND,&ACK_Value,1,HAL_MAX_DELAY);
}

void Bootloader_Send_Data_To_Host(uint8_t *Host_Buffer, uint32_t Data_Len)
{
	HAL_UART_Transmit(BL_HOST_COMMUNICATION_UART_SEND, Host_Buffer, Data_Len, HAL_MAX_DELAY);
}
