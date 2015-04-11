#ifndef  __BOOT_H__
#define  __BOOT_H__



#define STM32F042C6
#if  !defined (STM32F042C4)
#define   APP_ADDREND             ((uint32_t)0x08008000)       //STM32F042C6      APPÙÀåa÷œÏ„»A≥u
#else
#define   APP_ADDREND             ((uint32_t)0x08004000)       //STM32F042C4      APPÙÀåa÷œÏ„»A≥u
#endif


#define   IAP_ADDR                ((uint32_t)0x08000000)
#define   APP_FLAH_ADDR           ((uint32_t)0x08002000)
#define   APP_ADDR                ((uint32_t)0x08002400)
#define   APP_AVAILABLE_FLAG_ADDR ((uint32_t)0x08002600)
#define   FLASH_PAGE_SIZE         ((uint32_t)0x00000400)
#define   IAPLICATION_ADDRESS     ((uint32_t)0x08000000)
#define   APPLICATION_ADDRESS     ((uint32_t)APP_ADDR)
#define   PROENDMARK              ((uint32_t)(APP_ADDREND-4))


#define   POWERUP_APPTOIAP  0x55aa5aa5aa55a55aULL
#define   POWERUP_IAPTOAPP  0xaa55a55a55aa5aa5ULL


#define   APP_OK            0x55555555AAAAAAAAULL


#define BOOT_FLAG_APPTOIAP    (*(volatile unsigned long long *)(0x200000f0))

#if defined(APP)
extern const unsigned long long appAvailableFlag;
extern void  appAvailableFlagNotOptimiz();
#endif

extern unsigned char isAppAvailable();
extern void bootApp();
extern void bootAppToIap();

#endif