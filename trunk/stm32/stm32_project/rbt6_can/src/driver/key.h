/*! \file  key.h
* \b �ӿ�˵����
*       
*
* \b ��������:
*
*       ɨ�谴��
*
* \b ��ʷ��¼:
*
*     <����>        <ʱ��>      <�޸ļ�¼> <br>
*     zuokongxiao  2013-03-10   �����ļ�   <br>
*/
#ifndef __KEY_H
#define __KEY_H

/******************************************************************************/
/*                             ͷ�ļ�                                         */
/******************************************************************************/


/******************************************************************************/
/*                             �궨��                                         */
/******************************************************************************/
/*! �������� */
#define KEY_NONE            (0x00)
#define KEY_S1              (0x01)
#define KEY_S2              (0x02)
/*! �����ж� */
#define CHECK_KEY_DOWN(keytemp, key)    ((key) != ((keytemp)&(key)))
#define CHECK_KEY_UP(keytemp, key)      ((key) == ((keytemp)&(key)))

/******************************************************************************/
/*                          �ⲿ�ӿں�������                                  */
/******************************************************************************/
void key_port_init(void);
uint8_t key_read(void);


#endif /*!< end __KEY_H */

/********************************end of file***********************************/
