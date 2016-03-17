#ifndef __QL_I2C_H__
#define __QL_I2C_H__


/* Interface */
typedef enum
{
   I2C_SW_8BIT=1,   //not support
   I2C_SW_16BIT,    //not support
   I2C_HW_8BIT,
   I2C_HW_16BIT
} I2C_MODE;


typedef enum
{
     HW_I2C_OWNER_1=6,
     HW_I2C_OWNER_2,
     HW_I2C_OWNER_3,
     HW_I2C_OWNER_4,
     HW_I2C_OWNER_5,
     HW_I2C_OWNER_6,
     HW_I2C_NUM_OF_OWNER
}I2C_OWNER;
typedef enum
{
	I2C_TRANSACTION_COMPLETE,
	I2C_TRANSACTION_FAIL
}I2C_TRANSACTION_RESULT;

/* Transaction mode for new I2C APIs */
typedef enum
{
	I2C_TRANSACTION_FAST_MODE,
	I2C_TRANSACTION_HIGH_SPEED_MODE
}I2C_TRANSACTION_MODE;

typedef struct
{
	I2C_MODE I2c_mode; // Transaction mode for existing I2c APIs

	bool get_handle_wait; //When get handle wait until the I2c is avaliable

	u8 slave_address;	//the address of the slave device

	u8 delay_len;	//number of half pulse between transfers in a trasaction

	I2C_TRANSACTION_MODE transaction_mode;	//I2c_TRANSACTION_FAST_MODE or I2c_TRANSACTION_HIGH_SPEED_MODE

	u32 Fast_Mode_Speed;	//The speed of I2c fast mode(Kb)

	u32 HS_Mode_Speed;	//The speed of I2c high speed mode(Kb)

}I2c_config_struct;


typedef struct
{
	I2c_config_struct  I2c_config;
	u8 fs_sample_cnt_div;     //these two parameters are used to specify I2c clock rate
	u8 fs_step_cnt_div;       //half pulse width=step_cnt_div*sample_cnt_div*(1/13Mhz)
	u8 hs_sample_cnt_div;     //these two parameters are used to specify I2c clock rate
	u8 hs_step_cnt_div;       //half pulse width=step_cnt_div*sample_cnt_div*(1/13Mhz)
	I2C_TRANSACTION_RESULT transaction_result; /* The result of the end of transaction   (I2c_TRANSACTION_COMPLETE|I2c_TRANSACTION_FAIL) */
}I2c_handle_struct;
    
/*****************************************************************
* Function:     Ql_I2C_Init 
* 
* Description:
*               Initialize hardware I2c device.
* Parameters:
*              None         
*                  
* Return:      None   
*               
*****************************************************************/    
void Ql_I2C_Init(void);
/*****************************************************************
* Function:     Ql_I2C_Config 
* 
* Description:
*               Configure the function of hardware I2c.
* Parameters:
*               owner:
*                  [in] the slave device
*                       Every slave device has its own enum
*
*               config:
*                  [in] A pointer to 'I2c_config_struct'
* Return:        
*               If the function suceeds, returns "QL_RET_OK"
*                or returns an Error Code
*****************************************************************/   

s32 Ql_I2C_Config(I2C_OWNER owner,I2c_config_struct* config);

/*****************************************************************
* Function:     Ql_I2C_Read 
* 
* Description:
*               This function read data form the specific slave device.               
* Parameters:
*               owner:
*                  [in] the slave device
*                       Every slave device has its own enum
*
*               para:
*                  [in] A pointer to the Read data buffer.
*
*              datalen:
*                  [in] the number of bytes read from the specific slave device ,  the range of datalen is 1<datalen<8  .
* Return:        
*               If the function suceeds, returns the number bytes of Read
*                or returns an Error Code
*****************************************************************/ 
s32 Ql_I2C_Read(I2C_OWNER owner,u8* para,u32 datalen);

/*****************************************************************
* Function:     Ql_I2C_Write 
* 
* Description:
*              This function write data to the specific slave device. 
* Parameters:
*               owner:
*                  [in] the slave device
*                       Every slave device has its own enum
*
*               para:
*                  [in] A pointer to the Write data buffer.
*
*              datalen:
*                  [in]  the number of bytes write to the specific slave device , the range of datalen is 1<datalen<8 .
* Return:        
*               If the function suceeds, returns the number bytes of Write
*                or returns an Error Code
*****************************************************************/
s32 Ql_I2C_Write(I2C_OWNER owner,u8* para,u32 datalen);

#endif // End-of __QL_I2C_H__