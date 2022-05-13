#ifndef __GLOBAL_CONFIG_H
#define __GLOBAL_CONFIG_H

#include "common.h"


/***************************** 配置信息 ******************************/
#define	S_M_BUILD															// 定义了进行组网

/*---------- 服务器默认地址配置 ------------*/
#define	SERVER_ADDR		0x0000
#define DEFAULT_PAN		0x4221

/*---------- flash的配置 ------------*/
#define STACKSIZE							0x400						// 栈的大小

/*---------- 温度报警的范围 ------------*/
#define TEMPET_MIN					-10
#define TEMPET_MAX					50

/*------------ 霍尔传感器 --------------*/
#define		ZZJVOLT_MIN					50
#define		ZZJDELTA						8						 		// 误差范围，193 +/- 8 大概为1A
#define		ZZJDELTA_HALF				ZZJDELTA/2			// 误差范围，193 +/- 6 大概为0.5A

#define   ZZJ_DC_BV						193							// 霍尔传感器的基准电压2.5V  194  base value
#define   ZZJ_DC_TV_MIN         ZZJ_DC_BV - ZZJDELTA               // threshold value
#define   ZZJ_DC_TV_MAX         ZZJ_DC_BV + ZZJDELTA               // threshold value

/*---------- HR报警的范围 ------------*/
#define HR_MIN					-8
#define HR_MAX					8

//#define		XHJ_AC_VPT          126     			// 交流电压的直流偏置1.63V,可以通过平均值计算出来

/*---------- 各种设备采集数据算法配置 ------------*/
#define  XHJ_CALCPOT 				  10							// 有效值平均次数

/*---------- 各种时间参数的配置 ------------*/
#define ENABLE_WDT       											// 看门狗的开关
#define	ZZJ_TIME						1000							// 开始拍照

/* 看门狗的超时时间配置 */
#define   WDT_TIMEOUT_S(n)				1000000*(n)				// 其他设备的统一超时时间（n s）
#define 	TIMER_WDT_TIMEOUT_S(n)	1000*(n)					// 将timer1模拟成定时器，超时时间为n s（设备n分钟没有轮询到，复位）
#define 	TIMER_WDT_TIMEOUT_M(n)	1000*60*(n)				// 将timer1模拟成定时器，超时时间为n min（设备n分钟没有轮询到，复位）

/* 入网的一些配置信息 */
#define	 S_M_ReTime						1000*18					// 这个值应该在8-19之间
#define  S_M_FirstNum					30							// 首次入网的30次数
#define  S_M_DelayTime				1000*60*10			// 再入网次数的延时时间20min


/*--------—---- 中断优先级配置 -------------*/
#define SET_PRIORITY

#define DMA_AD_PRIO			0
#define COMMCT_PRIO			1
#define VC0706_PRIO			2
#define TIMER1_PRIO			3
#define TIMER0_PRIO			4
#define TIMER3_PRIO			5
#define TIMER2_PRIO			6

/*--------—---- 串口的配置信息 -------------*/
/* 波特率的配置 */
#define COMMCT_BR			230400    //230400
#define VC0706_BR			115200

//#define BAOMA_TEST
//#define MCU_1
#define MCU_2

#if defined(BAOMA_TEST)		// 使用开发板测试XHJ时， COMMCT_UART_PORT = 2; ZZJ_ADC_CHANNEL = 5
		#define COMMCT_UART_PORT			2
		#define VC0706_UART_PORT	 		0
		#define ZZJ_ADC_CHANNEL	 			5
//		#define USED_UART_DEBUG_PORT	3
#elif defined(MCU_1)
		#define COMMCT_UART_PORT			1
		#define VC0706_UART_PORT	 		3
		#define ZZJ_ADC_CHANNEL	 			6
//		#define USED_UART_DEBUG_PORT	2
#elif defined(MCU_2)
		#define COMMCT_UART_PORT			1
		#define VC0706_UART_PORT	 		3
		#define ZZJ_ADC_CHANNEL	 			5
//		#define USED_UART_DEBUG_PORT	2
#endif

/* 通信UART（驱动2992）的配置 */
#if (COMMCT_UART_PORT==1)
		#define COMMCT_UART						LPC_UART1						// 自己做的板子的通信口(LPC_UART_TypeDef *)
		#define COMMCT_IRQn						UART1_IRQn
		#define	GPDMA_CONN_UART_Tx		GPDMA_CONN_UART1_Tx
#elif (COMMCT_UART_PORT==2)														// 开发板的通信口
		#define COMMCT_UART						LPC_UART2
		#define COMMCT_IRQn						UART2_IRQn
		#define	GPDMA_CONN_UART_Tx		GPDMA_CONN_UART2_Tx
#elif (COMMCT_UART_PORT==0)														// 测试
		#define COMMCT_UART						LPC_UART0
		#define COMMCT_IRQn						UART0_IRQn
		#define	GPDMA_CONN_UART_Tx		GPDMA_CONN_UART0_Tx
#elif (COMMCT_UART_PORT==3)														// 测试  使用p0.25、p0.26，和信号机的AD0.2、AD0.3 冲突 
		#define COMMCT_UART						LPC_UART3
		#define COMMCT_IRQn						UART3_IRQn
		#define	GPDMA_CONN_UART_Tx		GPDMA_CONN_UART3_Tx
#endif

/* 摄像头UART（驱动VC0706）的配置 */
#if (VC0706_UART_PORT==2)
		#define VC0706_UART		LPC_UART2				// 备用vc0706_uart口
		#define VC0706_UART		LPC_UART2
		#define VC0706_IRQn		UART2_IRQn
#elif (VC0706_UART_PORT==3)
		#define VC0706_UART		LPC_UART3				// 自己做的板子 vc0706_uart口
		#define VC0706_UART		LPC_UART3
		#define VC0706_IRQn		UART3_IRQn
#elif (VC0706_UART_PORT==0)
		#define VC0706_UART		LPC_UART0				// 测试
		#define VC0706_UART		LPC_UART0
		#define VC0706_IRQn		UART0_IRQn
#endif

/* 直流AD采集的配置 */
#if (ZZJ_ADC_CHANNEL==5)
		#define ADC_CHANNEL		ADC_CHANNEL_5				// 开发板采集信号机直流采集通道5
#elif (ZZJ_ADC_CHANNEL==6)
		#define ADC_CHANNEL		ADC_CHANNEL_6				// 自己做的板子 采集信号机直流采集通道6（p0.3和UART0冲突）
#endif

///* 调试UART的配置 */
//#if (USED_UART_DEBUG_PORT==0)
//		#define DEBUG_UART_PORT	LPC_UART0     // 开发板使用的调试口
//#elif (USED_UART_DEBUG_PORT==2)
//		#define DEBUG_UART_PORT	LPC_UART2			// 自己做的板子用	的调试口
//#elif (USED_UART_DEBUG_PORT==3)
//		#define DEBUG_UART_PORT	LPC_UART3			// 测试 使用p0.25、p0.26，和信号机的AD0.2、AD0.3 冲突 
//#endif

#define USERDBG					// 打印信息开关,	同时也控制了调试串口UART0的初始化

#ifdef  USERDBG 												// 以下的打印信息都是字符串显示
#define  DBG_String			_DBG    				// 打印字符串（不换行）
#define  DBG_H8					_DBH    	 			// 打印一个字节（十六进制）
#define  DBG_H16				_DBH16     			// 打印2个字节（十六进制）
#define  DBG_H32				_DBH32     			// 打印4个字节（十六进制）
#define  DBG_D8					_DBD    	 			// 打印一个字节（十进制）
#define  DBG_D16				_DBD16    			// 打印2个字节（十进制）
#define  DBG_D32				_DBD32     			// 打印4个字节（十进制）
#define	 DBG_BufnH8			Array_BufnH8		// 打印数组
#define	 DBG_BufnH16		Array_BufnH16		// 打印数组
#define  DBG_GetChar		_DG							// 得到一个字节
#else
#define  DBG_String(x)
#define  DBG_H8(x)
#define  DBG_H16(x)
#define  DBG_H32(x)
#define  DBG_D8(x)
#define  DBG_D16(x)
#define  DBG_D32(x)
#define	 DBG_BufnH8(DataBuf, Len)
#define	 DBG_BufnH16(DataBuf, Len)
#define  DBG_GetChar(x)
#endif /* USERDBG */

/* 无法屏蔽的打印消息 */
#define  MSG_String			_DBG    				// 打印字符串（不换行）
#define  MSG_H8					_DBH    	 			// 打印一个字节（十六进制）
#define  MSG_H16				_DBH16     			// 打印2个字节（十六进制）
#define  MSG_H32				_DBH32     			// 打印4个字节（十六进制）
#define  MSG_D8					_DBD    	 			// 打印一个字节（十进制）
#define  MSG_D16				_DBD16    			// 打印2个字节（十进制）
#define  MSG_D32				_DBD32     			// 打印4个字节（十进制）
#define	 MSG_BufnH8			Array_BufnH8		// 打印数组
#define	 MSG_BufnH16		Array_BufnH16		// 打印数组
#define  MSG_APP_VER(n) MSG_String("| App Ver: F15L"#n"2 \n")		// 打印程序版本号
#define  MSG_GetChar		_DG							// 得到一个字节


#endif //__GLOBAL_CONFIG_H
