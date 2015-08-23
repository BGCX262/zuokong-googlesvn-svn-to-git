/*! \file  config.h
* \b 接口说明：  
*
*
* \b 功能描述:
*
*       工程配置
*         
* \b 历史记录:
*
*     <作者>        <时间>      <修改记录> <br>
*     zuokongxiao  2012-11-14   创建文件   <br>
*/

#ifndef __CONFIG_H
#define __CONFIG_H


/******************************************************************************/
/*                               头文件                                       */
/******************************************************************************/



/******************************************************************************/
/*                               宏定义                                       */
/******************************************************************************/

/*! PCB版本定义 */
#define PCB_VERSION                     (0x01000000)
/*! LPC1768软件版本 */
#define ARM_SOFTWARE_VERSION            (0x01000001)

/*! 配置是否输出调试信息 */
#define DEBUG_MSG_EN                    (1)
/*! 配置是否输出错误信息 */
#define ERROR_MSG_EN                    (0)
/*! 配置是否输出警告信息 */
#define WARNING_MSG_EN                  (0)
/*! 配置是否输出提示信息 */
#define NOTE_MSG_EN                     (0)

/*! 串口配置 */
#define CONFIG_UART0_EN                 (1)
#define CONFIG_UART1_EN                 (0)
#define CONFIG_UART2_EN                 (0)

/*! 调试输出串口定义 */
#define CONFIG_DEBUG_PORT               (UART0_PORT_NUM)
/*! 定义调试输出串口波特率 */
#define CONFIG_DEBUG_BAUDRATE           (38400)

/*! 配置是否使用OS */
#define CONFIG_OS_EN                    (1)

/*! 配置是否DA输出 1:DAC; 0:AD8400 */
#define CONFIG_DAC_EN                   (1)



#endif /*! end __CONFIG_H */

/********************************end of file***********************************/

