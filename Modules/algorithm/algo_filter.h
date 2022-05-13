
#ifndef __B_ALGO_SORT_H__
#define __B_ALGO_SORT_H__

#ifdef __cplusplus
extern "C" {
#endif

/*Includes ----------------------------------------------*/
#include <stdint.h>


#define ALGO_DT_S8      0
#define ALGO_DT_U8      1
#define ALGO_DT_S16     2
#define ALGO_DT_U16     3
#define ALGO_DT_S32     4
#define ALGO_DT_U32     5

uint16_t Filter_DeExtremeAver_U16(uint16_t *pbuf, uint32_t size);


#ifdef __cplusplus
}
#endif

#endif

