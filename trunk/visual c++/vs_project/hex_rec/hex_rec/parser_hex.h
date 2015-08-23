/*! \file  parser_hex.h
* \b 接口说明：
*
*
* \b 功能描述:
*
*       解析hex记录
*
* \b 历史记录:
*
*       <作者>          <时间>       <修改记录> <br>
*       zuokongxiao    2012-10-17    创建该文件 <br>
*/

#ifndef __PARSER_HEX_H
#define __PARSER_HEX_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/*                            头文件                                          */
/******************************************************************************/
/*! exact-width signed integer types */
typedef   signed          char  int8;
typedef   signed short     int  int16;
typedef   signed           int  int32;
/*! exact-width unsigned integer types */
typedef unsigned          char  uint8;
typedef unsigned short     int  uint16;
typedef unsigned           int  uint32;

typedef union
{
    unsigned long d32;
    struct
    {
       unsigned short l:16;
       unsigned short h:16;
    } d16;
} Data32_16_t;

typedef union
{
    unsigned long d32;
    struct
    {
       unsigned char s0;
       unsigned char s1;
       unsigned char s2;
       unsigned char s3;
    } d8;
} Data32_8_t;

typedef union
{
    unsigned char d8;
    struct
    {
       unsigned char l:4;
       unsigned char h:4;
    } d4;
} Data8_4_t;

typedef union
{
    unsigned short d16;
    struct
    {
       unsigned char l;
       unsigned char h;
    } d8;
} Data16_8_t;

typedef union
{
    unsigned short d16;
    unsigned char d8[2];
    struct
    {
       unsigned char s0:4;
       unsigned char s1:4;
       unsigned char s2:4;
       unsigned char s3:4;
    } d4;
} Data16_4_t;

/******************************************************************************/
/*                           宏定义                                           */
/******************************************************************************/
/*! 返回成功 */
#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS            (0)
#endif
/*! 返回失败 */
#ifndef EXIT_FAILURE
#define EXIT_FAILURE            (-1)
#endif
/*! NULL值 */
#ifndef    NULL
#define    NULL                 (0)
#endif

/*! hex记录类型 */
enum _record_type
{
    RECORD_TYPE_DATA                    = 0x00,
    RECORD_TYPE_END_OF_FILE             = 0x01,
    RECORD_TYPE_EXT_SEG_ADDR            = 0x02,
    RECORD_TYPE_START_SEG_ADDR          = 0x03,
    RECORD_TYPE_EXT_LIN_ADDR            = 0X04,
    RECORD_TYPE_START_LIN_ADRR          = 0X05,
};
/*! hex记录结构体定义 */
typedef struct
{
    uint8 ucRecLen; /*!< 数据长度 */
    uint16 unOffset; /*!< 相对地址 */
    uint8 ucRecTyp; /*!< 记录的类型 */
    uint8 aucData[32]; /*!< 数据 */
} hex_record_t;

/*! 缓冲区大小 */
#define BOOT_BUF_SIZE               (128u)
/*! 用户代码起始地址 */
#define USR_CODE_ADR_START          0x00010000
/*! 状态 */
enum _boot_state
{
    BOOT_STATE_IDLE                 = 0x00,
    BOOT_STATE_START                = 0x01,
    BOOT_STATE_RECEIVE_DATA         = 0x02,
    BOOT_STATE_WRITE_FLASH          = 0x04,
    BOOT_STATE_END                  = 0x08,
};
/*! 数据缓冲区结构体 */
typedef struct
{
    uint8 ucHexEndFlag;         /*!< 结束标志位 */
    uint16 unWriteIndex;        /*!< 写标识 */
    uint8 aucBuf[BOOT_BUF_SIZE];
    /*! */
    uint8 ucPreState;           /*!< 记录状态 */
    Data32_16_t ulStaAddr;      /*!< 需写入256字节的起始地址 */
    Data32_16_t ulPreAddr;      /*!< 上次数据帧起始地址+数据帧长度后的地址 */
    Data32_16_t ulCurAddr;      /*!< 当前数据帧的地址(解析hex记录行后地址) */
}boot_buf_t;

/*! 发送数据结构体 */
typedef struct
{
    uint8 ucSendFlag; /*!< 发送标志位 */
    uint8 ucDataLen; /*!< 指示数据长度 */
    uint8 aucData[BOOT_BUF_SIZE+6]; /*!< */
} send_buf_t;

/******************************************************************************/
/*                          外部接口函数声明                                  */
/******************************************************************************/
int32 parser_hex_record_type(boot_buf_t *pstBootBuf, hex_record_t *pstHexRecord);
void init_boot_buf(boot_buf_t *pstBootBuf);
int32 parser_hex_record(hex_record_t *pstHexRecord, uint8 *pucDataBuf, uint8 ucDataLen);
void fill_send_buf(send_buf_t *pstSendbuf, boot_buf_t *pstBootBuf);

#ifdef __cplusplus
}
#endif

#endif /*!< end __PARSER_HEX_H */

