/*! \file  parser_hex.h
* \b �ӿ�˵����
*
*
* \b ��������:
*
*       ����hex��¼
*
* \b ��ʷ��¼:
*
*       <����>          <ʱ��>       <�޸ļ�¼> <br>
*       zuokongxiao    2012-10-17    �������ļ� <br>
*/

#ifndef __PARSER_HEX_H
#define __PARSER_HEX_H

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
/*                            ͷ�ļ�                                          */
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
/*                           �궨��                                           */
/******************************************************************************/
/*! ���سɹ� */
#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS            (0)
#endif
/*! ����ʧ�� */
#ifndef EXIT_FAILURE
#define EXIT_FAILURE            (-1)
#endif
/*! NULLֵ */
#ifndef    NULL
#define    NULL                 (0)
#endif

/*! hex��¼���� */
enum _record_type
{
    RECORD_TYPE_DATA                    = 0x00,
    RECORD_TYPE_END_OF_FILE             = 0x01,
    RECORD_TYPE_EXT_SEG_ADDR            = 0x02,
    RECORD_TYPE_START_SEG_ADDR          = 0x03,
    RECORD_TYPE_EXT_LIN_ADDR            = 0X04,
    RECORD_TYPE_START_LIN_ADRR          = 0X05,
};
/*! hex��¼�ṹ�嶨�� */
typedef struct
{
    uint8 ucRecLen; /*!< ���ݳ��� */
    uint16 unOffset; /*!< ��Ե�ַ */
    uint8 ucRecTyp; /*!< ��¼������ */
    uint8 aucData[32]; /*!< ���� */
} hex_record_t;

/*! ��������С */
#define BOOT_BUF_SIZE               (128u)
/*! �û�������ʼ��ַ */
#define USR_CODE_ADR_START          0x00010000
/*! ״̬ */
enum _boot_state
{
    BOOT_STATE_IDLE                 = 0x00,
    BOOT_STATE_START                = 0x01,
    BOOT_STATE_RECEIVE_DATA         = 0x02,
    BOOT_STATE_WRITE_FLASH          = 0x04,
    BOOT_STATE_END                  = 0x08,
};
/*! ���ݻ������ṹ�� */
typedef struct
{
    uint8 ucHexEndFlag;         /*!< ������־λ */
    uint16 unWriteIndex;        /*!< д��ʶ */
    uint8 aucBuf[BOOT_BUF_SIZE];
    /*! */
    uint8 ucPreState;           /*!< ��¼״̬ */
    Data32_16_t ulStaAddr;      /*!< ��д��256�ֽڵ���ʼ��ַ */
    Data32_16_t ulPreAddr;      /*!< �ϴ�����֡��ʼ��ַ+����֡���Ⱥ�ĵ�ַ */
    Data32_16_t ulCurAddr;      /*!< ��ǰ����֡�ĵ�ַ(����hex��¼�к��ַ) */
}boot_buf_t;

/*! �������ݽṹ�� */
typedef struct
{
    uint8 ucSendFlag; /*!< ���ͱ�־λ */
    uint8 ucDataLen; /*!< ָʾ���ݳ��� */
    uint8 aucData[BOOT_BUF_SIZE+6]; /*!< */
} send_buf_t;

/******************************************************************************/
/*                          �ⲿ�ӿں�������                                  */
/******************************************************************************/
int32 parser_hex_record_type(boot_buf_t *pstBootBuf, hex_record_t *pstHexRecord);
void init_boot_buf(boot_buf_t *pstBootBuf);
int32 parser_hex_record(hex_record_t *pstHexRecord, uint8 *pucDataBuf, uint8 ucDataLen);
void fill_send_buf(send_buf_t *pstSendbuf, boot_buf_t *pstBootBuf);

#ifdef __cplusplus
}
#endif

#endif /*!< end __PARSER_HEX_H */
