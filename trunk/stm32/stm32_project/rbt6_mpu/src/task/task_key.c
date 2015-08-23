/*! \file  task_key.c
* \b 接口说明：
*       
*
* \b 功能描述:
*
*       扫描按键任务
*
* \b 历史记录:
*
*     <作者>        <时间>      <修改记录> <br>
*     zuokongxiao  2013-05-11   创建文件   <br>
*/

/******************************************************************************/
/*                             头文件                                         */
/******************************************************************************/
#include "ucos_ii.h"
#include "os_cpu.h"
#include "deftype.h"
#include "task_app.h"
#include "bsp.h"
#include "led.h"
#include "uart.h"
#include "debug.h"
#include "key.h"


/******************************************************************************/
/*                             宏定义                                         */
/******************************************************************************/


/******************************************************************************/
/*                            变量定义                                        */
/******************************************************************************/


/******************************************************************************/
/*                          内部接口声明                                      */
/******************************************************************************/




/******************************************************************************/
/*                          内部接口实现                                      */
/******************************************************************************/
/*! \fn       void app_task_key(void *pdata)
*
*  \brief     扫描按键
*
*  \param     pdata [in] 
*
*  \exception 注意：长按按键时第一个输出与第二个输出的时间为600ms
*
*  \return    按键
*/
uint8 scan_key(void)
{
    static uint8 s_ucKeyS1State = 0x00, s_ucKeyS2State = 0x00;
    uint8 ucKeyValue = KEY_NONE, ucKeyTemp = KEY_NONE;
    
    /*! 读取按键值 */
    ucKeyTemp = key_read();
    /*! s1 */
    if(CHECK_KEY_DOWN(ucKeyTemp, KEY_S1))
    {
        if(s_ucKeyS1State >= 8)
        {
            ucKeyValue |= KEY_S1;
        }
        else
        {
            s_ucKeyS1State++;
            if(2 == s_ucKeyS1State)
            {
                ucKeyValue |= KEY_S1;
            }
        }
    }
    else
    {
        s_ucKeyS1State = 0x00;
    }
    
    /*! s2 */
    if(CHECK_KEY_DOWN(ucKeyTemp, KEY_S2))
    {
        if(s_ucKeyS2State >= 8)
        {
            ucKeyValue |= KEY_S2;
        }
        else
        {
            s_ucKeyS2State++;
            if(2 == s_ucKeyS2State)
            {
                ucKeyValue |= KEY_S2;
            }
        }
    }
    else
    {
        s_ucKeyS2State = 0x00;
    }

    return ucKeyValue;
}

/******************************************************************************/
/*                          外部接口实现                                      */
/******************************************************************************/
/*! \fn       void app_task_key(void *pdata)
*
*  \brief     扫描按键
*
*  \param     pdata [in] 
*
*  \exception 无
*
*  \return    无
*/
void app_task_key(void *pdata)
{
    uint8 ucKeyValue = 0;
    
    /* 防止编译器告警 */
    pdata = pdata;
    /* 开机延时 */
    //OSTimeDlyHMSM(0, 0, 2, 0);
    /*! 初始化按键端口 */
    key_port_init();
    
    /*! 任务循环 */
    for( ; ; )
    {
        /*! 扫描按键 */
        ucKeyValue = scan_key();
        if(KEY_NONE != ucKeyValue)
        {
            DEBUG_MSG("D: KeyValue=0x%02x\r\n", ucKeyValue);
        }
        /*! 延时 */
        OSTimeDlyHMSM(0, 0, 0, 100);
    }
}


/********************************end of file***********************************/

