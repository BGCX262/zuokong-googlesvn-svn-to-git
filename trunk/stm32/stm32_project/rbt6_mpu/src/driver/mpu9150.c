/*! \file  i2c.c
* \b 接口说明：
*
*       1. <br>
*
* \b 功能描述:
*
*       I2C驱动
*
* \b 历史记录:
*
*     <作者>        <时间>      <修改记录> <br>
*     zuokongxiao  2013-11-03   创建文件   <br>
*/

/******************************************************************************/
/*                             头文件                                         */
/******************************************************************************/
#include "stm32f10x_gpio.h"
#include "stm32f10x_i2c.h"
#include "myGPIOType.h"
#include "deftype.h"
#include "i2c.h"
#include "debug.h"

/******************************************************************************/
/*                             宏定义                                         */
/******************************************************************************/
/*! MPU9150从地址 */
#define MPU9150_ADDRESS     0xd0

/******************************************************************************/
/*                            变量定义                                        */
/******************************************************************************/

/******************************************************************************/
/*                          内部接口声明                                      */
/******************************************************************************/


/******************************************************************************/
/*                          内部接口实现                                      */
/******************************************************************************/
/*! \fn       static int32 mpu9150_read_reg(uint8 ucReg, uint8 *pucBuf, uint8 ucSize)
*
*  \brief     读取单个寄存器内容
*
*  \param     ucReg  [in] 寄存器
*
*  \param     pucBuf [in] 指向数据缓冲区
*
*  \param     ucSize [in] 读取数
*
*  \exception 无
*
*  \return    EXIT_SUCCESS：成功; EXIT_FAILURE：失败
*/
static int32 mpu9150_read_reg(uint8 ucReg, uint8 *pucBuf, uint8 ucSize)
{
    int32 lRet = EXIT_FAILURE;
    
    do
    {
        /*! 输入参数检查 */
        if(INVALID_POINTER(pucBuf) || (1 > ucSize))
        {
            break;
        }
        /*! 写寄存器 */
        if(EXIT_FAILURE == i2c_write(I2C1, MPU9150_ADDRESS, &ucReg, 1))
        {
            break;
        }
        /*! 读取数据 */
        if(EXIT_FAILURE == i2c_read(I2C1, MPU9150_ADDRESS, pucBuf, ucSize))
        {
            break;
        }
        lRet = EXIT_SUCCESS;
    }while(0);
    
    return lRet;
}






/******************************************************************************/
/*                          外部接口实现                                      */
/******************************************************************************/
/*! \fn       void mpu9150_test(void)
*
*  \brief     测试MPU9150
*
*  \param     无
*
*  \exception 无
*
*  \return    无
*/
void mpu9150_test(void)
{
    uint8 ucTemp = 0;
    
    /*! 读取who_am_i寄存器 */
    if(EXIT_FAILURE == mpu9150_read_reg(0x75, &ucTemp, 1))
    {
        DEBUG_MSG("E:read mpu9150 who_am_i reg error!\r\n");
    }
    else
    {
        DEBUG_MSG("D:MPU9150 who_am_i=0x%x\r\n", ucTemp);
    }
}

/********************************end of file***********************************/

