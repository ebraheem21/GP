/*
 * Bootloader_Private.h
 *
 *  Created on: Feb 18, 2024
 *      Author: Amr
 */

#ifndef INC_BOOTLOADER_PRIVATE_H_
#define INC_BOOTLOADER_PRIVATE_H_

/* ----------------- Macro Declarations -----------------*/
#define BL_HOST_BUFFER_RX_LENGTH 200
/* ---------------------- COMMANDS --------------------- */
#define CBL_GET_VER_CMD          0x10
#define CBL_GET_HELP_CMD         0x11
#define CBL_GET_CID_CMD          0x12
#define CBL_GO_TO_ADDR_CMD       0x14
#define CBL_FLASH_ERASE_CMD      0x15
#define CBL_MEM_WRITE_CMD        0x16
/* ------------------ CRC_VERIFICATION ----------------- */
#define CRC_VERIFYING_FAILED     0x00
#define CRC_VERIFYING_SUCCESS    0x01

#define SEND_NACK                0xAB
#define SEND_ACK                 0xCD

#define CBL_VENDOR_ID            100
#define CBL_SW_MAJOR_VERSION     1
#define CBL_SW_MINOR_VERSION     1
#define CBL_SW_PATCH_VERSION     0

/* ------------------ CBL_FLASH_ERASE_CMD -------------- */
#define CBL_FLASH_MAX_SECTOR_NUMBER    6
#define CBL_APP_MAX_SECTOR_NUMBER      4
#define CBL_FLASH_MASS_ERASE           0xFF

#define HAL_SUCCESSFUL_ERASE           0xFFFFFFFFU

#define INVALID_SECTOR_NUMBER      0x00
#define VALID_SECTOR_NUMBER        0x01

#define UNSUCCESSFUL_ERASE         0x02
#define SUCCESSFUL_ERASE           0x03

#define ADDRESS_IS_INVALID         0x00
#define ADDRESS_IS_VALID           0x01

/* Start address of sector 2 */
#define FLASH_SECTOR2_BASE_ADDRESS   0x08008000U

#define STM32F401_SRAM_SIZE      (64 * 1024)
#define STM32F401_FLASH_SIZE     (256 * 1024)
#define STM32F401_SRAM_END       (SRAM_BASE + STM32F401_SRAM_SIZE)
#define STM32F401_FLASH_END      (FLASH_BASE + STM32F401_FLASH_SIZE)
/* ------------------ CBL_MEM_WRITE_CMD ----------------- */
#define FLASH_PAYLOAD_WRITE_FAILED    0x00
#define FLASH_PAYLOAD_WRITE_SUCCESS   0x01

#define FLASH_LOCK_WRITE_FAILED       0x00
#define FLASH_LOCK_WRITE_PASSED       0x01

/* ----------------- Data Type Declarations ---------------*/
typedef enum{
	BL_NACK = 0,
	BL_ACK
}BL_Status;

typedef void (*pMainApp)(void);
typedef void (*Jump_Ptr)(void);

/* ----------------- Functions Decleration ----------------*/
void BL_GetVersion(uint8_t *Host_Buffer);
void BL_GetHelp(uint8_t *Host_Buffer);
void BL_GetChipIdentificationNumber(uint8_t *Host_Buffer);
void BL_FlashErase(uint8_t *Host_Buffer);
void BL_FlashWrite(uint8_t *Host_Buffer);
void BL_JumpToAddress(uint8_t *Host_Buffer);
void bootloader_jump_to_user_app(uint8_t *Host_Buffer);

uint32_t BL_CRC_Verify(uint8_t *Data, uint32_t DataLength, uint32_t HostCRC);
uint8_t PerformFlashErase(uint8_t SectorNumber, uint8_t NumberOfSectors);
uint8_t BL_AddressVerification(uint32_t Address);
uint8_t FlashMemory_PayloadWrite(uint8_t *Data, uint32_t StartingAddress, uint16_t PayloadLength);
void Bootloader_Send_Data_To_Host(uint8_t *Host_Buffer, uint32_t Data_Len);
void BL_Send_ACK(uint8_t DataLength);
void BL_Send_NACK();

#endif /* INC_BOOTLOADER_PRIVATE_H_ */
