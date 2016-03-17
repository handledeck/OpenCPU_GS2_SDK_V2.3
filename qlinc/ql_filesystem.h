/*==========================================================================
 |               Quectel OpenCPU --  User API
 |
 |              Copyright (c) 2010 Quectel Ltd.
 |
 |--------------------------------------------------------------------------
 | File Description
 | ----------------
 |      File System
 |
 |--------------------------------------------------------------------------
 |
 |  Designed by     :   Jay XIN
 |  Coded    by     :   Jay XIN
 |  Tested   by     :   Stanley YONG
 |
 |--------------------------------------------------------------------------
 | Revision History
 | ----------------
 |  Sep. 25, 2010       Stanley Yong        Make the notes clear.
 |
 \=========================================================================*/

#ifndef __QL_FILESYSTEM_H__
#define __QL_FILESYSTEM_H__
#include "ql_type.h"


/****************************************************************************
 * Type of file access permitted
 ***************************************************************************/
#define QL_FS_READ_WRITE            0x00000000L
#define QL_FS_READ_ONLY             0x00000100L
#define QL_FS_CREATE                0x00010000L
#define QL_FS_CREATE_ALWAYS         0x00020000L
#define QL_FS_OPEN_SHARED           0x00000200L


/****************************************************************************
 * Type of file move permitted
 ***************************************************************************/
#define QL_FS_MOVE_COPY          0x00000001     // Move file|folder by copy
#define QL_FS_MOVE_KILL          0x00000002     // Delete the moved file|folder after moving
#define QL_FS_MOVE_OVERWRITE    0x00010000      // Overwrite the existed file in destination path when move file|folder


/****************************************************************************
 * 
 ***************************************************************************/
#define QL_FS_FILE_TYPE              0x00000004     
#define QL_FS_DIR_TYPE               0x00000008     
#define QL_FS_RECURSIVE_TYPE         0x00000010

/****************************************************************************
 * Constants for File Seek
 ***************************************************************************/
typedef enum QlFsSeekPosTag
{
   QL_FS_FILE_BEGIN,    // Beginning of file
   QL_FS_FILE_CURRENT,  // Current position of file pointer
   QL_FS_FILE_END       // End of file
}QlFsSeekPos;

typedef enum tagFileSys_Storage
{
    Ql_FS_UFS = 1,
    Ql_FS_RAM = 2,
    Ql_FS_SD  = 3
} FileSys_Storage;

#if defined(QL_MEMORY_64M)
typedef enum _EAPP_SPACE_SCHEME_ENUM   // support for winbond 64M flash at present 
{
/*-1*/  SCHEME_NONE = -1,
/* 0*/  SCHEME_0KB_1280KB,
/* 1*/  SCHEME_32KB_1248KB,
/* 2*/  SCHEME_64KB_1216KB,
/* 3*/  SCHEME_96KB_1184KB,
/* 4*/  SCHEME_128KB_1152KB,
/* 5*/  SCHEME_160KB_1120KB,
/* 6*/  SCHEME_192KB_1088KB,
/* 7*/  SCHEME_224KB_1056KB,
/* 8*/  SCHEME_256KB_1024KB,
/* 9*/  SCHEME_288KB_992KB,
/*10*/  SCHEME_320KB_960KB,
/*11*/  SCHEME_352KB_928KB,
/*12*/  SCHEME_384KB_896KB,
/*13*/  SCHEME_416KB_864KB,
/*14*/  SCHEME_448KB_832KB,
/*15*/  SCHEME_480KB_800KB,
/*16*/  SCHEME_512KB_768KB,
/*17*/  SCHEME_544KB_736KB,
/*18*/  SCHEME_576KB_704KB,
/*19*/  SCHEME_608KB_672KB,
/*20*/  SCHEME_640KB_640KB,
/*21*/  SCHEME_672KB_608KB,
/*22*/  SCHEME_704KB_576KB,
/*23*/  SCHEME_736KB_544KB,
/*24*/  SCHEME_768KB_512KB,
/*25*/  SCHEME_800KB_480KB,
/*26*/  SCHEME_832KB_448KB,
/*27*/  SCHEME_864KB_416KB,
/*28*/  SCHEME_896KB_384KB,
/*29*/  SCHEME_928KB_352KB,
/*30*/  SCHEME_960KB_320KB,
/*31*/  SCHEME_992KB_288KB,
/*32*/  SCHEME_1024KB_256KB,
/*33*/  SCHEME_1056KB_224KB,
/*34*/  SCHEME_1088KB_192KB,
/*35*/  SCHEME_1120KB_160KB,
/*36*/  SCHEME_1152KB_128KB,
/*37*/  SCHEME_1184KB_96KB,
/*38*/  SCHEME_1216KB_64KB,
/*39*/  SCHEME_1248KB_32KB,
/*39*/  SCHEME_MAX
}APP_SPACE_SCHEME_ENUM;
#elif defined(QL_MEMORY_128M)
typedef enum _EAPP_SPACE_SCHEME_ENUM   //support for winbond 128M flash at present 
{
/*-1*/ SCHEME_NONE = -1, 
/* 0*/ SCHEME_0KB_2816KB,
/* 1*/ SCHEME_64KB_2752KB,
/* 2*/ SCHEME_128KB_2688KB,
/* 3*/ SCHEME_192KB_2624KB,
/* 4*/ SCHEME_256KB_2560KB,
/* 5*/ SCHEME_320KB_2496KB,
/* 6*/ SCHEME_384KB_2432KB,
/* 7*/ SCHEME_448KB_2368KB,
/* 8*/ SCHEME_512KB_2304KB,
/* 9*/ SCHEME_576KB_2240KB,
/*10*/ SCHEME_640KB_2176KB,
/*11*/ SCHEME_704KB_2112KB,
/*12*/ SCHEME_768KB_2048KB,
/*13*/ SCHEME_832KB_1984KB,
/*14*/ SCHEME_896KB_1920KB,
/*15*/ SCHEME_960KB_1856KB,
/*16*/ SCHEME_1024KB_1792KB,
/*17*/ SCHEME_1088KB_1728KB,
/*18*/ SCHEME_1152KB_1664KB,
/*19*/ SCHEME_1216KB_1600KB,
/*20*/ SCHEME_1280KB_1536KB,
/*21*/ SCHEME_1344KB_1472KB,
/*22*/ SCHEME_1408KB_1408KB,
/*23*/ SCHEME_1472KB_1344KB,
/*24*/ SCHEME_1536KB_1280KB,
/*25*/ SCHEME_1600KB_1216KB,
/*26*/ SCHEME_1664KB_1152KB,
/*27*/ SCHEME_1728KB_1088KB,
/*28*/ SCHEME_1792KB_1024KB,
/*29*/ SCHEME_1856KB_960KB,
/*30*/ SCHEME_1920KB_896KB,
/*31*/ SCHEME_1984KB_832KB,
/*32*/ SCHEME_2048KB_768KB,
/*33*/ SCHEME_2112KB_704KB,
/*34*/ SCHEME_2176KB_640KB,
/*35*/ SCHEME_2240KB_576KB,
/*36*/ SCHEME_2304KB_512KB,
/*37*/ SCHEME_2368KB_448KB,
/*38*/ SCHEME_2432KB_384KB,
/*39*/ SCHEME_2496KB_320KB,
/*40*/ SCHEME_2560KB_256KB,
/*41*/ SCHEME_2624KB_192KB,
/*42*/ SCHEME_2688KB_128KB,
/*43*/ SCHEME_2752KB_64KB,
/*44*/ SCHEME_2816KB_0KB,
/*45*/  SCHEME_MAX
}APP_SPACE_SCHEME_ENUM;
#else
typedef enum _EAPP_SPACE_SCHEME_ENUM   //only support for winbond 64M flash at present 
{
/*-1*/  SCHEME_NONE = -1,
/*0*/   SCHEME_MAX
}APP_SPACE_SCHEME_ENUM;
#endif

/******************************************************************************
* Function:     Ql_FileGetFreeSize
*  
* Description:
*               This function obtains the amount of free space on Flash.
*
* Parameters:    
*               None
* Return:  
*               The total number of free bytes on Flash.
******************************************************************************/
s32  Ql_FileGetFreeSize(void);

/******************************************************************************
* Function:     Ql_FileGetFreeSizeEx
*  
* Description:
*               This function obtains the amount of free space on Flash or SD card.
*
* Parameters:    
*               storage:
*                    [in]Ql_FS_UFS, obtains the amount of free space on Flash
*                         Ql_FS_SD, obtains the amount of free space on SD card
*               freesize_ptr:
*                    [out]pointer to varibale to store the free size.
*
* Return:  
*               QL_RET_OK, suceess
*               QL_RET_ERR_INVALID_PARAMETER, invalid parameter.
******************************************************************************/

s32 Ql_FileGetFreeSizeEx(u8 storage, /*out*/ u64 *freesize_ptr);

/******************************************************************************
* Function:     Ql_FileOpen
*  
* Description:
*               The method opens and automatically creates a named file. 
*               This function is deprecated, and a new function is available 
*               'Ql_FileOpenEx'.
*
* Parameters:    
*               asciifilename:
*                   [in]The name of the file. 
*                        The name is limited to 252 characters. 
*
*               bcreate:
*                   [in]If TRUE, the function will create a file and open it;
*                        If FALSE, the function opens a file.
*
*               bonlyread:
*                   [in]TRUE or FALSE, Indicates whether this function open 
*                                  the file with read-only.
* Return:  
*               If the function succeeds, the return value specifies a file handle.
*               If the function fails, the return value is an error codes. 
*               QL_RET_ERR_FILE_NO_CARD indicates no sd card.
*               QL_RET_ERR_PARAM indicates parameter error. 
*               QL_RET_ERR_FILENAMETOOLENGTH indicates filename too length.
*               QL_RET_ERR_FILEOPENFAILED indicates open file failed.
******************************************************************************/
s32  Ql_FileOpen(u8* asciifilename, bool bcreate, bool bonlyread);


/******************************************************************************
* Function:     Ql_FileOpenEx
*  
* Description:
*               The method opens and automatically creates a named file.
*
* Parameters:    
*               asciifilename:
*                   [in]The name of the file. 
*                        The name is limited to 252 characters. 
*
*               Flag:
*                   [in]A u32 that defines the file's opening and access mode.
*                       The possible values are shown as follow:
*                       QL_FS_READ_WRITE, can read and write
*                       QL_FS_READ_ONLY, can only read
*                       QL_FS_CREATE, opens the file, if it exists. 
*                           If the file does not exist, the function creates the file
*                       QL_FS_CREATE_ALWAYS, creates a new file. 
*                           If the file exists, the function overwrites the file 
*                           and clears the existing attributes
* Return:  
*               If the function succeeds, the return value specifies a file handle.
*               If the function fails, the return value is an error codes. 
*               QL_RET_ERR_FILE_NO_CARD indicates no sd card.
*               QL_RET_ERR_PARAM indicates parameter error. 
*               QL_RET_ERR_FILENAMETOOLENGTH indicates filename too length.
*               QL_RET_ERR_FILEOPENFAILED indicates open file failed.
******************************************************************************/
s32 Ql_FileOpenEx(u8* asciifilename, u32 Flag);


/******************************************************************************
* Function:     Ql_FileRead
*  
* Description:
*               Reads data from the specified file, starting at the position 
*               indicated by the file pointer. After the read operation has been 
*               completed, the file pointer is adjusted by the number of bytes actually read.
*
* Parameters:    
*               filehandle:
*                   [in] A handle to the file to be read, which is the return value
*                        of the function Ql_FileOpen.
*
*               readbuffer:
*                   [in] Pointer to the buffer that receives the data read from the file.
*
*               readlength:
*                   [in] Number of bytes to be read from the file.
*
*               bonlyread:
*                   [out] Pointer to the number of bytes read. ReadFile sets this
*                         value to zero before doing taking action or checking errors.
* Return:  
*               QL_RET_OK, suceess
*               QL_RET_ERR_FILEREADFAILED, read file failed.
******************************************************************************/
s32  Ql_FileRead(s32 filehandle, u8* readbuffer, u32 readlength, u32* readedlen);


/******************************************************************************
* Function:     Ql_FileWrite
*  
* Description:
*               This function writes data to a file. Ql_FileWrite starts writing 
*               data to the file at the position indicated by the file pointer.
*               After the write operation has been completed, the file pointer 
*               is adjusted by the number of bytes actually written. 
*
* Parameters:    
*               filehandle:
*                   [in] A handle to the file to be read, which is the return value 
*                        of the function Ql_FileOpen.
*
*               writebuffer:
*                   [in] Pointer to the buffer that receives the data read from the file.
*
*               writelength:
*                   [in] Number of bytes to be write to the file.
*
*               written:
*                   [out] Pointer to the number of bytes read. ReadFile sets this
*                        value to zero before doing taking action or checking errors.
* Return:  
*               QL_RET_OK, suceess
*               QL_RET_ERR_FILEDISKFULL, file disk is full.
*               QL_RET_ERR_FILEWRITEFAILED, write file failed.
******************************************************************************/
s32  Ql_FileWrite(s32 filehandle, u8* writebuffer, u32 writelength, u32* written);


/******************************************************************************
* Function:     Ql_FileSeek
*  
* Description:
*               Repositions the pointer in the previously opened file. 
*
* Parameters:    
*               filehandle:
*                   [in] A handle to the file to be read, which is the return value 
*                        of the function Ql_FileOpen.
*
*               Offset:
*                   [in] Number of bytes to move the file pointer.
*
*               Whence:
*                   [in] Pointer movement mode. See QlFsSeekPos.
* Return:  
*               QL_RET_OK, suceess
*               QL_RET_ERR_FILESEEKFAILED, file seek failed
******************************************************************************/
s32  Ql_FileSeek(s32 filehandle, s32 Offset, s32 Whence);


/******************************************************************************
* Function:     Ql_FileGetFilePosition
*  
* Description:
*               Gets the current value of the file pointer.
*
* Parameters:    
*               filehandle:
*                   [in] A file handle, which was returned by calling 'Ql_FileOpen'.
*
*               Position:
*                   [out] Address of u32 that will receive the current offset 
*                         from the beginning of the file.
* Return:  
*               QL_RET_OK, success.
*               QL_RET_ERR_FILEFAILED, fail to operate file.
******************************************************************************/
s32  Ql_FileGetFilePosition(s32 filehandle, u32* Position);


/******************************************************************************
* Function:     Ql_FileTruncate
*  
* Description:
*               This function truncates a file to ZERO size.
*
* Parameters:    
*               filehandle: 
*                   [in] A file handle, which was returned by calling 'Ql_FileOpen'.
* Return:  
*               QL_RET_OK, success.
*               QL_RET_ERR_FILEFAILED, fail to operate file.
******************************************************************************/
s32  Ql_FileTruncate(s32 filehandle);


/******************************************************************************
* Function:     Ql_FileFlush
*  
* Description:
*               Forces any data remaining in the file buffer to be written to the file.
*
* Parameters:    
*               filehandle: 
*                   [in] A file handle, which was returned by calling 'Ql_FileOpen'.
* Return:  
*               None
******************************************************************************/
void Ql_FileFlush(s32 filehandle);


/******************************************************************************
* Function:     Ql_FileClose
*  
* Description:
*               Closes the file associated with the file handle and makes 
*               the file unavailable for reading or writing.
*
* Parameters:    
*               filehandle: 
*                   [in] A file handle, which was returned by calling 'Ql_FileOpen'.
* Return:  
*               None
******************************************************************************/
void Ql_FileClose(s32 filehandle);


/******************************************************************************
* Function:     Ql_FileGetSize
*  
* Description:
*               Retrieves the size, in bytes, of the specified file.
*
* Parameters:    
*               asciifilename:
*                   [in] The name of the file. 
*
*               filesize:
*                   [out] A pointer to the variable where the file size, 
*                         in bytes, is stored. 
* Return:  
*               QL_RET_OK, success.
*               QL_RET_ERR_FILE_NO_CARD, no sd card.
*               QL_RET_ERR_PARAM, parameter error. 
*               QL_RET_ERR_FILENAMETOOLENGTH , filename too length.
*               QL_RET_ERR_FILEFAILED, fail to operate file.
******************************************************************************/
s32  Ql_FileGetSize(u8* asciifilename, u32* filesize);


/******************************************************************************
* Function:     Ql_FileDelete
*  
* Description:
*               This function deletes an existing file from a file system.
*
* Parameters:    
*               asciifilename:
*                   [in] The name of the file. 
* Return:  
*               QL_RET_OK, success.
*               QL_RET_ERR_FILE_NO_CARD, no sd card.
*               QL_RET_ERR_PARAM, parameter error. 
*               QL_RET_ERR_FILENAMETOOLENGTH , filename too length.
*               QL_RET_ERR_FILEFAILED, fail to operate file.
******************************************************************************/
s32  Ql_FileDelete(u8* asciifilename);


/******************************************************************************
* Function:     Ql_FileCheck
*  
* Description:
*               Check whether the file exists or not.
*
* Parameters:    
*               asciifilename:
*                   [in] The name of the file. 
* Return:  
*               QL_RET_OK, success.
*               QL_RET_ERR_FILE_NO_CARD, no sd card.
*               QL_RET_ERR_PARAM, parameter error. 
*               QL_RET_ERR_FILENAMETOOLENGTH , filename too length.
*               QL_RET_ERR_FILEFAILED, fail to operate file.
*               QL_RET_ERR_FILENOTFOUND, file not found.
******************************************************************************/
s32  Ql_FileCheck(u8* asciifilename);


/******************************************************************************
* Function:     Ql_FileRename
*  
* Description:
*               Rename a file.
*
* Parameters:    
*               asciifilename:
*                   File to be renamed. 
*
*               newasciifilename:
*                   New name of file. The name is limited to 252 characters. 
* Return:  
*               QL_RET_OK, success.
*               QL_RET_ERR_FILE_NO_CARD, no sd card.
*               QL_RET_ERR_PARAM, parameter error. 
*               QL_RET_ERR_FILENAMETOOLENGTH , filename too length.
*               QL_RET_ERR_FILEFAILED, fail to operate file.
******************************************************************************/
s32  Ql_FileRename(u8* asciifilename, u8* newasciifilename);


/******************************************************************************
* Function:     Ql_FileCreateDir
*  
* Description:
*               Creates a directory.
*
* Parameters:    
*               asciifilename:
*                   [in] A pointer to a string containing the name of the directory to create. 
* Return:  
*               QL_RET_OK, success.
*               QL_RET_ERR_FILE_NO_CARD, no sd card.
*               QL_RET_ERR_PARAM, parameter error. 
*               QL_RET_ERR_FILENAMETOOLENGTH , filename too length.
*               QL_RET_ERR_FILEFAILED, fail to operate file.
******************************************************************************/
s32  Ql_FileCreateDir(u8* asciidirname);


/******************************************************************************
* Function:     Ql_FileRemoveDir
*  
* Description:
*               Removes a existing directory.
*
* Parameters:    
*               asciifilename:
*                   [in] A pointer to a string containing the name of the directory to remove. 
* Return:  
*               QL_RET_OK, success.
*               QL_RET_ERR_FILE_NO_CARD, no sd card.
*               QL_RET_ERR_PARAM, parameter error. 
*               QL_RET_ERR_FILENAMETOOLENGTH , filename too length.
*               QL_RET_ERR_FILEFAILED, fail to operate file.
******************************************************************************/
s32  Ql_FileRemoveDir(u8* asciidirname);


/******************************************************************************
* Function:     Ql_FileCheckDir
*  
* Description:
*               Check whether the file exists or not.
*
* Parameters:    
*               asciifilename:
*                   [in] The name of the directory. 
* Return:  
*               QL_RET_OK, success.
*               QL_RET_ERR_FILE_NO_CARD, no sd card.
*               QL_RET_ERR_PARAM, parameter error. 
*               QL_RET_ERR_FILENAMETOOLENGTH , filename too length.
*               QL_RET_ERR_FILEFAILED, fail to operate file.
*               QL_RET_ERR_FILENOTFOUND, file not found.
******************************************************************************/
s32  Ql_FileCheckDir(u8* asciidirname);


/******************************************************************************
* Function:     Ql_FileFindFirst
*  
* Description:
*               This function searches a directory for a file or subdirectory 
*               whose name matches the specified file name. 
*
* Parameters:    
*               asciipath:
*                   [in] Pointer to a null-terminated string that specifies a valid directory or path.
*
*               asciifilename:
*                   [in] Pointer to a null-terminated string that specifies a valid file name, 
*                        which can contain wildcard characters, such as * and ?.
*
*               filenamelength:
*                   [in] The maximum number of bytes to be received of the name.
*
*               filesize:
*                   [out] A pointer to the variable which represents the size specified by the file.
*
*               isdir:
*                   [out] A pointer to the variable which represents the type specified by the file.
* Return:  
*               If the function succeeds, the return value is a search handle 
*               that can be used in a subsequent call to the 
*               QL_RET_ERR_FILE_NO_CARD, no sd card.
*               QL_RET_ERR_PARAM, parameter error. 
*               QL_RET_ERR_FILENAMETOOLENGTH , filename too length.
*               QL_RET_ERR_FILEFAILED, fail to operate file.
*               QL_RET_ERR_FILENOMORE,  no more file.
******************************************************************************/
s32  Ql_FileFindFirst(u8* asciipath, u8* asciifilename, u32 filenamelength, u32* filesize, bool* isdir);


/******************************************************************************
* Function:     Ql_FileFindNext
*  
* Description:
*               Continues a file search from a previous call to the Ql_FileFindFirst 
*               function.
*
* Parameters:    
*               handle:
*                   [in] Search handle returned by a previous call to the Ql_FileFindFirst
*
*               asciifilename:
*                   [in] Pointer to a null-terminated string that specifies a valid file name, 
*                        which can contain wildcard characters, such as * and ?.
*
*               filenamelength:
*                   [in] The maximum number of bytes to be received of the name.
*
*               filesize:
*                   [out] A pointer to the variable which represents the size specified by the file.
*
*               isdir:
*                   [out] A pointer to the variable which represents the type specified by the file.
* Return:  
*               QL_RET_OK, success.
*               QL_RET_ERR_PARAM, parameter error. 
*               QL_RET_ERR_FILEFAILED, fail to operate file.
*               QL_RET_ERR_FILENOMORE, file not found.
******************************************************************************/
s32  Ql_FileFindNext(s32 handle, u8* asciifilename, u32 filenamelength, u32* filesize, bool* isdir);


/******************************************************************************
* Function:     Ql_FileFindClose
*  
* Description:
*               Closes the specified search handle.
*
* Parameters:    
*               handle:
*                   [in] Find handle, 
*                        returned by a previous call of the Ql_FileFindFirst function. 
* Return:  
*               None
******************************************************************************/
void Ql_FileFindClose(s32 handle);


/******************************************************************************
* Function:     Ql_FileXDelete
*                           
* Description:
*               Delete a file or directory.
*           
* Parameters:    
*               asciipath: 
*                       [in] File path to be deleted
*
*               flag: 
*                   [in] A u32 that defines the file's opening and access mode.
*                         The possible values are shown as follow:
*                         QL_FS_FILE_TYPE, 
*                         QL_FS_DIR_TYPE, 
*                         QL_FS_RECURSIVE_TYPE
*
* Return:     
*               QL_RET_OK, success.
*               QL_RET_ERR_PARAM, parameter error. 
*               QL_RET_ERR_FILENAMETOOLENGTH, filename too length.
*               QL_RET_ERR_FILENOTFOUND, file not found.
*               QL_RET_ERR_PATHNOTFOUND, path not found.
*               QL_RET_ERR_GET_MEM, fail to get memory.
*               QL_RET_ERR_GENERAL_FAILURE, general failture.
******************************************************************************/
s32 Ql_FileXDelete(u8* asciipath, u32 flag);


/******************************************************************************
* Function:     Ql_FileXMove  
*                           
* Description:
*               This function provides a facility to move/copy a file or folder
*           
* Parameters:    
*               asciisrcpath: 
*                       [in] Source path to be moved/copied
*
*               asciidestpath:
*                       [in] Destination path
*
*               flag:
*                   [in] A u32 that defines the file's opening and access mode.
*                         The possible values are shown as follow:
*                          QL_FS_MOVE_COPY, 
*                          QL_FS_MOVE_KILL, 
*                          QL_FS_MOVE_OVERWRITE
*
* Return:       
*               QL_RET_OK, success.
*               QL_RET_ERR_PARAM, parameter error. 
*               QL_RET_ERR_FILENAMETOOLENGTH, filename too length.
*               QL_RET_ERR_FILENOTFOUND, file not found.
*               QL_RET_ERR_PATHNOTFOUND, path not found.
*               QL_RET_ERR_GET_MEM, fail to get memory.
*               QL_RET_ERR_FILE_EXISTS, file existed.
*               QL_RET_ERR_GENERAL_FAILURE, general failture.
******************************************************************************/
s32 Ql_FileXMove(u8* asciisrcpath, u8* asciidestpath, u32 flag);


/******************************************************************************
* Function:     Ql_FileSys_GetSpaceInfo
*  
* Description:
*               Get the space information in file system area.
*
* Parameters:    
*               storage:
*                   [in] one value of 'FileSys_Storage'
*                        1 = UFS
*                        2 = RAM (only for M33)
*                        3 = SD Card (only for M33)
*
*               freeSpace:
*                   [out] Free space size, unit in bytes.
*
*               totalSpace:
*                   [out] Total space size, unit in bytes.
* Return:  
*               QL_RET_OK, success.
*               Ql_RET_ERR_UNKOWN, unkown error.
******************************************************************************/
s32 Ql_FileSys_GetSpaceInfo(u8 storage, u64* freeSpace, u64* totalSpace);

/******************************************************************************
* Function:     Ql_Fs_Format  
*                           
* Description:
*               This function format the SD card or UFS
*           
* Parameters:    
*               type: 
*                       Ql_FS_UFS,
*                       Ql_FS_SD,
*
* Return:       
*               QL_RET_OK, success.
*               QL_RET_ERR_PARAM, parameter error. 
*               QL_RET_ERR_FILENAMETOOLENGTH, filename too length.
*               QL_RET_ERR_FILENOTFOUND, file not found.
*               QL_RET_ERR_PATHNOTFOUND, path not found.
*               QL_RET_ERR_GET_MEM, fail to get memory.
*               QL_RET_ERR_GENERAL_FAILURE, general failture.
******************************************************************************/
s32 Ql_Fs_Format(u8 storage);

s32 Ql_WrtCodeRegion_Init(APP_SPACE_SCHEME_ENUM eAppSpaceScheme);
s32 Ql_WrtCodeRegion_Write(s8* buffer, u32 length);
s32 Ql_WrtCodeRegion_Finish(void);


#endif  // End-of __QL_FILESYSTEM_H__

