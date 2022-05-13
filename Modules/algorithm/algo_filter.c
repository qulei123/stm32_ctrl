
#include "algo_filter.h"



/* 去极值平均滤波 */
uint16_t Filter_DeExtremeAver_U16(uint16_t *pbuf, uint32_t size)
{
    int      i   = 0;
    uint32_t max = 0, min = 0xffffffff, sum = 0;

    for (i = 0; i < size; i++)
    {
        if (pbuf[i] >= max)
        {
            max = pbuf[i];
        }

        if (pbuf[i] <= min)
        {
            min = pbuf[i];
        }

        sum += pbuf[i];
    }

    return ((sum - max - min) / (size - 2));
}


