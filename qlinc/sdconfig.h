/*==========================================================================
 |               Quectel OpenCPU --  User API
 |
 |              Copyright (c) 2010 Quectel Ltd.
 |
 |--------------------------------------------------------------------------
 | File Description
 | ----------------
 |      SPI Configuration (July 7, 2007)
 |
 |--------------------------------------------------------------------------
 |
 |  Designed by     :   
 |  Coded    by     :   
 |  Tested   by     :   
 |
 |--------------------------------------------------------------------------
 | Revision History
 | ----------------
 |
 \=========================================================================*/
#ifndef __QL_SPI_TEST_H__
#define __QL_SPI_TEST_H__
 
typedef unsigned char  INT8U;  
//typedef signed   char  INT8;  
typedef unsigned short INT16U; 
typedef signed   short INT16;  
typedef unsigned int   INT32U; 
//typedef signed   int   INT32; 
typedef float          FP32;   
typedef double         FP64;   

typedef unsigned short kal_uint16;

//#define MSDC_DEBUG
#ifdef MSDC_DEBUG
#define MASS
#define SD_ERASE
#endif

/*
 * CRC enable for transferring data
 */
#define SD_CRC_EN                0

/*
 * SPI clock frequency (unit: Hz)
 */
#define SPI_CLOCK                1000000//5529600

/*
 * Block length of SD card
 */
#define SD_BLOCKSIZE             512
#define SD_BLOCKSIZE_NBITS      9

/*
 * Enable for the corresponding operations
 *  1 indicates enable
 *  0 indicates disable
 */
#define SD_ReadMultiBlock_EN    1
#define SD_WriteMultiBlock_EN   1
#define SD_EraseBlock_EN        1
#define SD_ProgramCSD_EN        1
#define SD_ReadCID_EN           1
#define SD_ReadSD_Status_EN     1
#define SD_ReadSCR_EN           1

extern unsigned char tf_cs_level;
extern unsigned short line_file;


/*
 * The following configurations should be re-configured according to the real hardware environment
 */
#if 1 //defined(__CARD_SPI_MODE__)
extern char gpio_tf_clk;
extern char gpio_tf_do;
extern char gpio_tf_di;
extern char gpio_tf_cs;

#define  SPI_SCK_PIN               //gpio_tf_clk           //TF-clk        
#define  SPI_MISO_PIN              //gpio_tf_do            //TF-DAT0        
#define  SPI_MOSI_PIN              //gpio_tf_di            //TF-CMD
#define  SPI_CS_PIN                //gpio_tf_cs            //TF-DAT3     

//GPIO0 CLK
#define  SPI_SCK_GPIO()           (*(volatile kal_uint16 *)(0x80120150)) &= 0xfffc  //GPIO_ModeSetup(SPI_SCK_PIN,0);        
#define  SPI_SCK_OUT()            (*(volatile kal_uint16 *)(0x80120000)) |= 0x0001  //GPIO_InitIO(OUTPUT,SPI_SCK_PIN)
#define  SPI_SCK_IN()             (*(volatile kal_uint16 *)(0x80120000)) &= 0xfffe  //GPIO_InitIO(INPUT,SPI_SCK_PIN)
#define  SPI_SCK_SET()            (*(volatile kal_uint16 *)(0x801200C0)) |= 0x0001  //GPIO_WriteIO(1,SPI_SCK_PIN)
#define  SPI_SCK_CLR()            (*(volatile kal_uint16 *)(0x801200C0)) &= 0xfffe  //GPIO_WriteIO(0,SPI_SCK_PIN)

//M30 EVT
#if 0 
//IN   GPIO1
/* MISO  */    
#define  SPI_MISO_GPIO()           (*(volatile kal_uint16 *)(0x80120150)) &= 0xfff3  //GPIO_ModeSetup(SPI_MISO_PIN,0);   
#define  SPI_MISO_OUT()            (*(volatile kal_uint16 *)(0x80120000)) |= 0x0002//GPIO_InitIO(OUTPUT,SPI_MISO_PIN)
#define  SPI_MISO_IN()             (*(volatile kal_uint16 *)(0x80120000)) &= 0xfffd//GPIO_InitIO(INPUT,SPI_MISO_PIN)
#define  SPI_MISO_SET()            (*(volatile kal_uint16 *)(0x801200C0)) |= 0x0002//GPIO_WriteIO(1,SPI_MISO_PIN)
#define  SPI_MISO_CLR()            (*(volatile kal_uint16 *)(0x801200C0)) &= 0xfffd//GPIO_WriteIO(0,SPI_MISO_PIN)
#define  SPI_MISO_READ()           ((*(volatile kal_uint16 *)(0x80120100)) >> 1) & 0x0001 // GPIO_ReadIO(SPI_MISO_PIN)    


//OUT   GPIO2 
/* MOSI  */    
#define  SPI_MOSI_GPIO()           (*(volatile kal_uint16 *)(0x80120150)) &= 0xffcf //GPIO_ModeSetup(SPI_MOSI_PIN,0);
#define  SPI_MOSI_OUT()            (*(volatile kal_uint16 *)(0x80120000)) |= 0x0004//GPIO_InitIO(OUTPUT,SPI_MOSI_PIN)
#define  SPI_MOSI_IN()             (*(volatile kal_uint16 *)(0x80120000)) &= 0xfffb//GPIO_InitIO(INPUT,SPI_MOSI_PIN)
#define  SPI_MOSI_SET()            (*(volatile kal_uint16 *)(0x801200C0)) |= 0x0004//GPIO_WriteIO(1,SPI_MOSI_PIN)
#define  SPI_MOSI_CLR()            (*(volatile kal_uint16 *)(0x801200C0)) &= 0xfffb//GPIO_WriteIO(0,SPI_MOSI_PIN)
#endif

//M33 EVT
#if 1 
//IN   GPIO2
/* MISO  */    
#define  SPI_MISO_GPIO()          (*(volatile kal_uint16 *)(0x80120150)) &= 0xffcf //GPIO_ModeSetup(SPI_MOSI_PIN,0);
#define  SPI_MISO_OUT()           (*(volatile kal_uint16 *)(0x80120000)) |= 0x0004//GPIO_InitIO(OUTPUT,SPI_MOSI_PIN)
#define  SPI_MISO_IN()            (*(volatile kal_uint16 *)(0x80120000)) &= 0xfffb//GPIO_InitIO(INPUT,SPI_MOSI_PIN)
#define  SPI_MISO_SET()           (*(volatile kal_uint16 *)(0x801200C0)) |= 0x0004//GPIO_WriteIO(1,SPI_MOSI_PIN)
#define  SPI_MISO_CLR()           (*(volatile kal_uint16 *)(0x801200C0)) &= 0xfffb//GPIO_WriteIO(0,SPI_MOSI_PIN)
#define  SPI_MISO_READ()          ((*(volatile kal_uint16 *)(0x80120100)) >> 2) & 0x0001 // GPIO_ReadIO(SPI_MISO_PIN)    


//OUT   GPIO1
/* MOSI  */    
#define  SPI_MOSI_GPIO()          (*(volatile kal_uint16 *)(0x80120150)) &= 0xfff3  //GPIO_ModeSetup(SPI_MISO_PIN,0);   
#define  SPI_MOSI_OUT()           (*(volatile kal_uint16 *)(0x80120000)) |= 0x0002//GPIO_InitIO(OUTPUT,SPI_MISO_PIN)
#define  SPI_MOSI_IN()            (*(volatile kal_uint16 *)(0x80120000)) &= 0xfffd//GPIO_InitIO(INPUT,SPI_MISO_PIN)
#define  SPI_MOSI_SET()           (*(volatile kal_uint16 *)(0x801200C0)) |= 0x0002//GPIO_WriteIO(1,SPI_MISO_PIN)
#define  SPI_MOSI_CLR()           (*(volatile kal_uint16 *)(0x801200C0)) &= 0xfffd//GPIO_WriteIO(0,SPI_MISO_PIN)
#endif


//CS   GPIO13
/* CS  */        
#define  SPI_CS_GPIO()            (*(volatile kal_uint16 *)(0x80120160)) &= 0xf3ff //GPIO_InitIO(OUTPUT,SPI_CS_PIN)
#define  SPI_CS_OUT()             (*(volatile kal_uint16 *)(0x80120000)) |= 0x2000//GPIO_InitIO(OUTPUT,SPI_CS_PIN)
#define  SPI_CS_IN()              (*(volatile kal_uint16 *)(0x80120000)) &= 0xdfff//GPIO_InitIO(INPUT,SPI_CS_PIN)
#define  SPI_CS_SET()             (*(volatile kal_uint16 *)(0x801200C0)) |= 0x2000//GPIO_WriteIO(1,SPI_CS_PIN)
#define  SPI_CS_CLR()             (*(volatile kal_uint16 *)(0x801200C0)) &= 0xdfff//GPIO_WriteIO(0,SPI_CS_PIN)

/*
 * Initialize the IO port as a SPI port
 */
#define  SPI_INIT()         //PINSEL0 &= ~((0x03 << 8) + (0x03 << 10) + (0x03 << 12));
                             //PINSEL0 |= (0x01 << 8) + (0x01 << 10) + (0x01 << 12);
                             
/*
 * Pins for power control
 */
#define  SD_POWER                //(0x01 << 9)    
#define  SD_POWER_GPIO()        //PINSEL0 &= ~(0x03 << 18)
#define  SD_POWER_OUT()         //(*(volatile kal_uint16 *)(0x80120010)) |= 0x0008//IODIR |= SD_POWER
#define  SD_POWER_ON()          //(*(volatile kal_uint16 *)(0x801200D0)) |= 0x0008//IOCLR = SD_POWER
#define  SD_POWER_OFF()         //(*(volatile kal_uint16 *)(0x801200D0)) &= 0xFFF7//IOSET = SD_POWER

/*
 * Pins for dectecting inserting SD card
 */
#define  SD_INSERT                //(0x01 << 10)        
#define  SD_INSERT_GPIO()        //PINSEL0 &= ~(0x03 << 20)
#define  SD_INSERT_IN()          //IODIR &= ~SD_INSERT
#define  SD_INSERT_STATUS()      //(IOPIN & SD_INSERT)

/*
 * Pins for SD-card write-protect
 */
#define  SD_WP                    //(0x01 << 11)        
#define  SD_WP_GPIO()            //PINSEL0 &= ~(0x03 << 22)
#define  SD_WP_IN()              //IODIR &= ~SD_WP
#define  SD_WP_STATUS()          //(IOPIN & SD_WP)
#endif

#endif // end of '__QL_SPI_TEST_H__'

