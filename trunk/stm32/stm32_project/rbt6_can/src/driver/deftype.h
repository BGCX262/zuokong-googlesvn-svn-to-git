/*! \file  deftype.h
* \b �ӿ�˵����
*
*
* \b ��������:
*
*       �������͡��ṹ�塢�����塢ö�ٵȶ���
*
* \b ��ʷ��¼:
*
*     <����>        <ʱ��>      <�޸ļ�¼> <br>
*     zuokongxiao  2013-03-12   �����ļ�   <br>
*/
#ifndef __DEFTYPE_H
#define __DEFTYPE_H

/******************************************************************************/
/*                             ͷ�ļ�                                         */
/******************************************************************************/


/******************************************************************************/
/*                             �궨��                                         */
/******************************************************************************/
/*! exact-width signed integer types */
typedef   signed          char  int8;
typedef   signed short     int  int16;
typedef   signed           int  int32;
typedef   signed       __int64  int64;

/*! exact-width unsigned integer types */
typedef unsigned          char  uint8;
typedef unsigned short     int  uint16;
typedef unsigned           int  uint32;
typedef unsigned       __int64  uint64;

/*! ���سɹ� */
#ifndef EXIT_SUCCESS
 #define EXIT_SUCCESS           (0)
#endif

/*! ����ʧ�� */
#ifndef EXIT_FAILURE
 #define EXIT_FAILURE           (-1)
#endif

/*! NULLֵ */
#ifndef NULL
 #define NULL                   (0)
#endif

#ifndef FALSE
 //#define FALSE                  (0)
#endif

#ifndef TRUE
 //#define TRUE                   (1)
#endif

#ifndef ENABLE
 //#define ENABLE                 (1)
#endif

#ifndef DISABLE
 //#define DISABLE                (0)
#endif

/*! ��ȡ���鳤�� */
#ifndef ARRAY_SIZE
 #define __must_be_array(a)     0
 #define ARRAY_SIZE(x)          (sizeof(x)/sizeof((x)[0]) + __must_be_array(x))
#endif

/*! �ж�ָ���Ƿ���Ч */
#define INVALID_POINTER(x)      (NULL == (x))
#define VALID_POINTER(x)        (NULL != (x))

/*! λ���� */
#define BIT0                    0x01
#define BIT1                    0x02
#define BIT2                    0x04
#define BIT3                    0x08
#define BIT4                    0x10
#define BIT5                    0x20
#define BIT6                    0x40
#define BIT7                    0x80
#define BIT8                    0x0100
#define BIT9                    0x0200
#define BIT10                   0x0400
#define BIT11                   0x0800
#define BIT12                   0x1000
#define BIT13                   0x2000
#define BIT14                   0x4000
#define BIT15                   0x8000

/*! �ֽ�λ�����ĺ궨�� */
typedef union 
{
    unsigned char Byte;
    struct
    {
        unsigned char Bit0:1;
        unsigned char Bit1:1;
        unsigned char Bit2:1;
        unsigned char Bit3:1;
        unsigned char Bit4:1;
        unsigned char Bit5:1;
        unsigned char Bit6:1;
        unsigned char Bit7:1;
    } Bitfield;
} byte_bit_t;

/*! short��תΪ2�ֽ�(�˴�����Ĭ��ΪС��ģʽ) */
typedef union 
{
    unsigned short int Short;
    unsigned char Byte[2];
} short_byte_t;

/*! float��תΪ4�ֽ�(�˴�����Ĭ��ΪС��ģʽ) */
typedef union 
{
    float Float;
    unsigned char Byte[4];
} float_byte_t;

/*! ���庯��ָ������ */
typedef void (*pFunProc)(void);

/******************************************************************************/
/*                          �ⲿ�ӿں�������                                  */
/******************************************************************************/


#endif /*!< end __DEFTYPE_H */

/********************************end of file***********************************/
