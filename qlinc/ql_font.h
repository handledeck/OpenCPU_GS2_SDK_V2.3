#include "ql_type.h"

#define  WORD_WIDTH         16		// 16*16
#define  WORD_HEIGHT        16		// 16*16
#define  MAX_CHARS_NUMBER   8836
#define  BYTES_PER_WORD     (WORD_WIDTH) * (WORD_HEIGHT) / 8	// Number of bytes for presenting one word

typedef enum tagQL_FontType{
    FONT_NONE = 0,
    FONT_GB2132_12X12 = 1,
    FONT_GB2132_16X16 = 2,
    FONT_END
}QL_FontType;

/******************************************************************************
* Function:     Ql_GetFontData
*  
* Description:
*                Get bitmap font data.
*
* Parameters:    
*                word:
*                       Characters.
*
*                fontType:
*                       Font type, initialize to zero.
*
*                fontData:
*                       A buffer to store bitmap font data.
*
* Return:  
*               If this function succeeds, the length of bitmap data will be returned.
*               Negative indicates failure. please see Error Code Definition.
******************************************************************************/
s32 Ql_GetFontData(char word[2], u8 fontType, u8* fontData);

