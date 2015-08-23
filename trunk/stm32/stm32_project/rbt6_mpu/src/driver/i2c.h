/*! \file  i2c.h
* \b 接口说明：
*       
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
#ifndef __I2C_H
#define __I2C_H

/******************************************************************************/
/*                             头文件                                         */
/******************************************************************************/

/******************************************************************************/
/*                             宏定义                                         */
/******************************************************************************/
/*! I2C数据缓冲区 */
typedef struct _i2c_buf
{
    uint8 ucSalveAddr; /*!< 从机地址 */
    uint16 unBufSize; /*!< 缓冲区大小 */
    uint8 *pucBuf; /*!< 数据缓冲区 */
} i2c_buf_t;

/******************************************************************************/
/*                          外部接口函数声明                                  */
/******************************************************************************/
void i2c_init(I2C_TypeDef *I2Cx);
int32_t i2c_write(I2C_TypeDef *I2Cx, uint8_t ucAddr, \
                  uint8_t *pucBuf, uint16_t unSize);
int32_t i2c_read(I2C_TypeDef *I2Cx, uint8_t ucAddr, \
                  uint8_t *pucBuf, uint16_t unSize);


#endif /*!< end __I2C_H */

/********************************end of file***********************************/
