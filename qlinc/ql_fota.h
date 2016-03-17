#ifndef __QL_FOTA_H__
#define __QL_FOTA_H__

#include "ql_type.h"


/*=============================================================================
 | <<< BEGIN: 
 |     The following definitions are for some extension development with 
 |     Ql_Fota_App_InitEx() API function.
 |
 |     Usually developer calls Ql_Fota_App_Init() to initialize FOTA function.
 |     If any developer has some special demands that need to use Ql_Fota_App_InitEx(), 
 |     please contact Quectel for the use assistance.
 \=============================================================================*/
/************************************************************************
* IMPORTANT NOTE:
*   If use OpenCPU module with 128Mb memory, please open this definition 
*   of QL_MEMORY_128M; or if use OpenCPU module with 64Mb memory, please 
*   open this definition of QL_MEMORY_64M.
*************************************************************************/
//#define QL_MEMORY_64M
//#define QL_MEMORY_128M


/***************************************************
*  PLEASE DON'T MODIFY THE DEFINITION BELOW.
*  WRONG DEFINITION WILL CAUSE UPDATING FAILS.
****************************************************/

#if (defined  (QL_MEMORY_128M))

#define QL_APP_REGION_START_ADDR        0x08540000
#define QL_APP_UPDATE_CACHE_START_ADDR  0x08640000

#elif (defined (QL_MEMORY_64M))

#define QL_APP_REGION_START_ADDR        0x08540000
#define QL_APP_UPDATE_CACHE_START_ADDR  0x085E0000

#else
#define QL_APP_REGION_START_ADDR        0xFFFFFFFF
#define QL_APP_UPDATE_CACHE_START_ADDR  0xFFFFFFFF

#endif

typedef struct _TFeedDog    //Define Watchdog
{
    s16 Q_gpio_pin1;        //Watchdog gpio pin 1, If only use one gpio,you can set other to -1,it means invalid.
    s16 Q_feed_interval1;   //gpio1 time interval for feed dog.
    s16 Q_gpio_pin2;        //Watchdog gpio pin 2, If only use one gpio,you can set other to -1,it means invalid.
    s16 Q_feed_interval2;   //gpio2 time interval for feed dog.
    s16 reserved1;          //reserve 1
    s16 reserved2;          //reserve 2
}FEED_DOG;

typedef enum _EFOTA_UPDATE_TYPE
{
    FOTA_UPDATE_TYPE_BEGIN = -1,
    FOTA_UPDATE_TYPE_APP = 0 ,               //App cover upgrade
    FOTA_UPDATE_TYPE_CORE = 1 ,            //CORE cover upgrade
    FOTA_UPDATE_TYPE_CORE_DELTA ,     //CORE delta upgrade
    FOTA_UPDATE_TYPE_END
}FOTA_UPDATE_TYPE;

typedef struct _TCustomerUpdate
{
    FOTA_UPDATE_TYPE  Q_update_type;	             //Upgrade type£¬0 :App cover upgrade;1:CORE cover upgrade;2:CORE delta upgrade
    u32  Q_function_flag;		                     // if(Q_function_flag&0x01),  it means after the upgrade, clear nvram
    u32  Q_packet_size;                              //Size of bin,when after using upgrade packaging tools

    u32  Q_app_area_start;                           //Start address of  app area, app bin will be write to this area when upgrade successed.
    u32  Q_app_area_size;                            //Size of  app area,upgrade package must be small than Q_app_cache_area_size
    u32  Q_app_cache_area_start;                     //Start address of  cache area, it is used to store app bin when upgrade.
    u32  Q_app_cache_area_size;                      //Size of  cache area,upgrade package must be small than Q_app_cache_area_size

    u32  Q_core_area_start;                          // When App upgrade,this value is invalid , it can fill any value or not to fill
    u32  Q_core_area_size;                           // When App upgrade,this value is invalid , it can fill any value or not to fill
    u32  Q_core_cache_area_start;                    // When App upgrade,this value is invalid , it can fill any value or not to fill
    u32  Q_core_cache_area_size;                     // When App upgrade,this value is invalid , it can fill any value or not to fill

    FEED_DOG    Q_t_Watch_dog;                       //Set watchdog when upgrade
    s32  reserved[20];                               //reserve
}CUSTOMER_UPDATE_STRUCT;
/*=============================================================================
 |     The definitions are for some extension development with 
 |     Ql_Fota_App_InitEx() API function.
 |     
 |     :END >>>
 \=============================================================================*/



/*****************************************************************
* Function:     Ql_Fota_Core_Init 
* 
* Description:
*               Initializes the FOTA-Core related functions.
*               Applications must initialize the FOTA function before they call
*               other FOTA-Core related functions.
* Parameters:
*               None
* Return:        
*               None
*****************************************************************/
extern void Ql_Fota_Core_Init(void);


/*****************************************************************
* Function:     Ql_Fota_Core_Write_Data 
* 
* Description:
*               FOTA write data API.
*                1. This function is used to write data to spare image pool
*                2. This API only allow sequentially writing mechanism
*                3. Authentication mechanism is executed during writing
* Parameters:
*               length: the length of writing (Unit: Bytes)
*               buffer: pointer to the start address of buffer
* Return:        
*               0:   means pass write success
*               < 0: writing action is fail
*****************************************************************/
extern s32  Ql_Fota_Core_Write_Data(s32 length, s8* buffer);


/*****************************************************************
* Function:     Ql_Fota_Core_Finish 
* 
* Description:
*               FOTA finalization API.
*                1. compare calculated checksum with image checksum in the header after
*                   whole image is written
*                2. mark the status to UPDATE_NEEDED 
* Parameters:
*               None
* Return:        
*               None
*****************************************************************/
extern void Ql_Fota_Core_Finish(void);


/*****************************************************************
* Function:     Ql_Fota_App_InitEx 
* 
* Description:
*               Initializes the FOTA-Application related functions.
*                1. Initialize data structure and progress initial step
*                2. Register customized authentication function or encryption function
*
*               NOTE:
*                   Developer should contact Quectel for use assistance 
*                   on this API function.
* Parameters:
*               pcs: 
*                   one pointer;it point a struct of upgrade
* Return:        
*               QL_RET_OK indicates this function successes.
*               QL_RET_ERR_PARAM indicates parameter error.
*               Ql_RET_NOT_SUPPORT indicates not support this function.
*               Ql_RET_ERR_RAWFLASH_UNKNOW indicates unkown error.
*****************************************************************/
extern s32 Ql_Fota_App_InitEx(CUSTOMER_UPDATE_STRUCT *pcs);


/*****************************************************************
* Function:     Ql_Fota_App_Init 
* 
* Description:  Initialise FOTA_Application related functions.
*               It a simple API.Programer only need to pass the
*               simple parameters to this API.
*
* Parameters:
*               pDog -> A pointer to watch dog 
* Return:        
*               QL_RET_OK indicates this function successes.
*               QL_RET_ERR_PARAM indicates parameter error.
*               Ql_RET_NOT_SUPPORT indicates not support this function.
*               Ql_RET_ERR_RAWFLASH_UNKNOW indicates unkown error.
*****************************************************************/

extern s32 Ql_Fota_App_Init(FEED_DOG *pDog);

/*****************************************************************
* Function:     Ql_Fota_App_Write_Data 
* 
* Description:
*               FOTA write data API.
*                1. This function is used to write data to spare image pool
*                2. This API only allow sequentially writing mechanism
*                3. Authentication mechanism is executed during writing
* Parameters:
*               length: the length of writing (Unit: Bytes).recommend 512 bytes
*               buffer: pointer to the start address of buffer
* Return:        
*               QL_RET_OK indicates this function successes.
*               QL_RET_ERR_PARAM indicates parameter error.
*               Ql_RET_NOT_SUPPORT indicates not support this function.
*               Ql_RET_ERR_UNKOWN indicates unkown error.
*               Ql_RET_ERR_RAWFLASH_OVERRANGE indicates over flash range.
*               Ql_RET_ERR_RAWFLASH_UNIITIALIZED indicates uninitialized before write or read flash.
*               Ql_RET_ERR_RAWFLASH_UNKNOW indicates unkown error.
*               Ql_RET_ERR_RAWFLASH_INVLIDBLOCKID indicates block id invalid.
*               Ql_RET_ERR_RAWFLASH_PARAMETER indicates parameter error.
*               Ql_RET_ERR_RAWFLASH_ERASEFlASH indicates erasen flash failure.
*               Ql_RET_ERR_RAWFLASH_WRITEFLASH indicates writen flash failure.
*               Ql_RET_ERR_RAWFLASH_READFLASH indicates readen flash failure.
*               Ql_RET_ERR_RAWFLASH_MAXLENGATH indicates the data length too large.
*****************************************************************/
extern s32  Ql_Fota_App_Write_Data(s32 length, s8* buffer);


/*****************************************************************
* Function:     Ql_Fota_App_Finish 
* 
* Description:
*               FOTA finalization API.
*                1. compare calculated checksum with image checksum in the header after
*                   whole image is written
*                2. mark the status to UPDATE_NEEDED 
* Parameters:
*               None
* Return:        
*               QL_RET_OK indicates this function successes.
*               Ql_RET_NOT_SUPPORT indicates not support this function.
*               Ql_RET_ERR_UNKOWN indicates unkown error.
*               Ql_RET_ERR_RAWFLASH_OVERRANGE indicates over flash range.
*               Ql_RET_ERR_RAWFLASH_UNIITIALIZED indicates uninitialized before write or read flash.
*               Ql_RET_ERR_RAWFLASH_UNKNOW indicates unkown error.
*               Ql_RET_ERR_RAWFLASH_INVLIDBLOCKID indicates block id invalid.
*               Ql_RET_ERR_RAWFLASH_PARAMETER indicates parameter error.
*               Ql_RET_ERR_RAWFLASH_ERASEFlASH indicates erasen flash failure.
*               Ql_RET_ERR_RAWFLASH_WRITEFLASH indicates writen flash failure.
*               Ql_RET_ERR_RAWFLASH_READFLASH indicates readen flash failure.
*               Ql_RET_ERR_RAWFLASH_MAXLENGATH indicates the data length too large.
*****************************************************************/
extern s32 Ql_Fota_App_Finish(void);



/*****************************************************************
* Function:     Ql_Fota_Update 
* 
* Description:
*               Starts FOTA Update.
* Parameters:
*               None.
* Return:        
*               QL_RET_OK indicates this function successes.
*               QL_RET_ERR_INVALID_OP indicates invalid operation.
*               Ql_RET_NOT_SUPPORT indicates not support this function.
*               Ql_RET_ERR_RAWFLASH_PARAMETER indicates parameter error.
*               Ql_RET_ERR_RAWFLASH_ERASEFlASH indicates erasen flash failure.
*               Ql_RET_ERR_RAWFLASH_WRITEFLASH indicates writen flash failure.
*****************************************************************/
extern s32 Ql_Fota_Update(void);

#endif  // End-of __QL_FOTA_H__
