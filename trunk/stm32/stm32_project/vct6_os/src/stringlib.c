/*******************************************************************************
* File Name          : stringlib.c
* Description        : 源文件.
*
* Version            : V1.0
* Date               : 2011/11/25
* Author             : zuokong
* Description        : 创建.
********************************************************************************
* Copyright (c) 2011,深圳汉谱光彩科技股份有限公司
* All rights reserved.
*******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <math.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


/*******************************************************************************
* Function Name  : intpow
* Description    : .
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
int intpow(int x, int y)
{
    int num = 1;

    for(; y>0; y--)
    {
        num = num * x;
    }
	return num;
}

/*******************************************************************************
* Function Name  : FLOAT2STR
* Description    : .
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
char *FLOAT2STR(float bcd_data, int num)
{
    static char str[20];
    char ch, *right, *left, *xright, *xleft;
    unsigned int Value, xValue, tempnum, intpownum;
    float temp1;

    left = right = str;
    if(bcd_data < 0)
    {
        str[0] = '-';
   		left++;
   		right++;
   		bcd_data = fabs(bcd_data);
    }
    else
    {
   		str[0] = ' ';
   		left++;
   		right++;
    }

    tempnum = num;
    intpownum=intpow(10,num+1);
    temp1=(float)1000000000/intpownum;
    temp1=temp1/1000000000;
    bcd_data=bcd_data+temp1;
    Value =(int)bcd_data;
    intpownum=intpow(10,num);
    temp1 =((bcd_data-Value)*intpownum);
    xValue =(int)temp1;
    //把数字转换成字符串（倒置的）
    if(Value == 0x00)
    {
   	    *right = 0x30;
   	    //*(++right) = 0x30;
   	    right++;
    }
    else if(Value>0x00 && Value<0x0a)
    {
   	    *right = (Value&0x0f)+0x30;
   	    //*(++right) = 0x30;
   	    right++;
    } 		
    else
    {
   		while(Value)

   		{
      		*right = (Value%10)+0x30;
      		Value = Value/10;
    		right++;
   		}       
   	}
    xright = xleft = ++right;
    *(--right) = '.';
    right--;
    if(xValue == 0x00)
    {
   	    for(tempnum=num;tempnum>0;tempnum--)
   	    {
	   	    *xright = 0x30;
	   	    xright++;
	    }
    }
    else if(xValue>0x00 && xValue<0x0a)
    {
   	    *(xright++) = (xValue&0x0f)+0x30;
   	    for(tempnum=num;tempnum>1;tempnum--)
   	    {
   	   		*(xright++) = 0x30;
   	    }
    } 		
    else
    {
   		while(xValue)
   		{
      		*xright = (xValue%10)+0x30;
      		xValue = xValue/10;
      		xright++;
   		}
   	}
   	*xright-- = '\0';
    while(xright > xleft)
    {
        ch = *xleft;
        *xleft++ = *xright;
        *xright-- = ch;
    }
    //把倒置的字符串正放过来
    while(right > left)
    {
        ch = *left;
        *left++ = *right;
        *right-- = ch;
    }
    return str;
}

/**************************END OF FILE*****************************************/

