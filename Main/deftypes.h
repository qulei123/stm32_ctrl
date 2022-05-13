
#ifndef _DEF_TYPES_H_
#define _DEF_TYPES_H_

/* Includes ------------------------------------------------------------------- */
#include <stdint.h>

/* 基本类型定义 */
typedef void        VOID;
typedef char        CHAR;
typedef int         INT;
typedef float       FLOAT;
typedef double      DOUBLE;

typedef uint64_t    U64;
typedef uint32_t    U32;
typedef uint16_t    U16;
typedef uint8_t     U8;

typedef int64_t     S64;
typedef int32_t     S32;
typedef int16_t     S16;
typedef int8_t      S8;

typedef U16         BE16;
typedef U32         BE32;

typedef enum {FALSE = 0, TRUE = !FALSE} BOOL;


#endif   /* _DEF_TYPES_H_ */

