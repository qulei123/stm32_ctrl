
#ifndef _PIPO_H_
#define _PIPO_H_

#include <stdint.h>

typedef struct
{
    uint8_t *pBuf[2];           /* 缓冲区 */
    uint32_t Len;               /* 数据大小 */
    uint32_t FullIdx;           /* 区分缓冲区，指向满buf */
}T_Pipo, *PT_Pipo;


#define POPI(name)                      tPipo##name

#define PIPO_INSTANCE(name, size)               \
        static uint8_t buf0[size], buf1[size];  \
        T_Pipo POPI(name) =                     \
        {                                       \
            .pBuf[0] = buf0,                    \
            .pBuf[1] = buf1,                    \
            .Len = 0,                           \
            .FullIdx = 0                        \
        }
    
#define PIPO_EXTERN(name)               extern T_Pipo POPI(name)
/* 获取满buf */
#define Pipo_Get_Buf(name)              POPI(name).pBuf[POPI(name).FullIdx]

/* 获取空buf，并指向下一个buf */
#define Pipo_Get_Buf_Inc_Idx(name)            \
({                                                  \
    PT_Pipo ptPipo = &POPI(name);                   \
    uint32_t FullIdx = ptPipo->FullIdx;             \
    ptPipo->FullIdx  = !ptPipo->FullIdx;            \
    ptPipo->pBuf[FullIdx];                          \
})
#define Pipo_Get_Len(name)              POPI(name).Len
#define Pipo_Set_Len(name, len)         POPI(name).Len = len


#if 0
/* 获取空buf */
#define Pipo_Get_Empty_Buf(name)            POPI(name).pBuf[!POPI(name).FullIdx]
/* 获取满buf */
#define Pipo_Get_Full_Buf(name)             POPI(name).pBuf[POPI(name).FullIdx]
/* 设置buf大小，指向下一个空buf */
#define Pipo_Set_Len(name, len)             \
({                                          \
    PT_Pipo ptPipo  = &POPI(name);          \
    ptPipo->Len     = len                   \
    ptPipo->FullIdx = !ptPipo->FullIdx;     \
})
#define Pipo_Get_Len(name)                  POPI(name).Len
#endif





#if 0
void Pipo_Init(PT_Pipo *ptPipo, uint8_t *pBuf1, uint8_t *pBuf2)
{
    ptPipo->pBuf[0] = pBuf1;
    ptPipo->pBuf[1] = pBuf2;
    ptPipo->Len = 0;
    ptPipo->Idx = 0;
}

uint8_t *Pipo_Get_Buf(PT_Pipo *ptPipo)
{
    return ptPipo->pBuf[ptPipo->Idx];
}

uint8_t *Pipo_Get_Buf_Inc_Idx(PT_Pipo *ptPipo)
{
    uint32_t Idx = ptPipo->Idx;
    uint8_t  *pBuf = ptPipo->pBuf[Idx];
    
    ptPipo->Idx = !Idx;

    return pBuf;
}

void Pipo_Set_Len(PT_Pipo *ptPipo, uint32_t Len)
{
   ptPipo->Len = Len;
}

uint32_t Pipo_Get_Len(PT_Pipo *ptPipo)
{
   return ptPipo->Len;
}
#endif

#endif  // _PIPO_H_

