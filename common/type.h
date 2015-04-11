#ifndef  __type___h__
#define  __type___h__

#include <stddef.h>
#include <stdbool.h>

typedef unsigned char  uint8;
typedef signed   char  int8;
typedef unsigned short uint16;
typedef signed   short int16;
typedef unsigned int   uint32;
typedef signed   int   int32;
typedef float          fp32;
typedef double         fp64;
typedef unsigned long long uint64;

typedef unsigned char  INT8U;
typedef signed   char  INT8S;
typedef unsigned short INT16U;
typedef signed   short INT16S;
typedef unsigned int   INT32U;
typedef signed   int   INT32S;
typedef float          FP32;
typedef double         FP64;
typedef signed long long int64;

typedef    signed char I8;
typedef   unsigned char U8 ;
typedef    signed short  I16;
typedef  unsigned short   U16;
typedef    signed long  I32;
typedef  unsigned long  U32 ;
typedef  I16    I16P ;
typedef  U16   U16P;




#ifndef UNUSED
#define UNUSED(A)  (void) ((A))
#endif




typedef bool BOOL;
typedef unsigned int size_t16;
typedef unsigned int size_t32;
typedef wchar_t wchar;
typedef struct {unsigned short x0,y0,x1,y1;} RECT_U16 ;
typedef struct {unsigned int x0,y0,x1,y1;} RECT_U32 ;
typedef struct {int x0,y0,x1,y1;} RECT_I32 ;
typedef struct {unsigned short x,y;} POINT_16;
typedef struct {unsigned short x,y;} POINT_32;




/**
* container_of - cast a member of a structure out to the containing structure
* @ptr:     the pointer to the member.
* @type:     the type of the container struct this is embedded in.
* @member:     the name of the member within the struct.
*
*/
#define container_of(ptr, type, member) ((type *)( (char *)ptr - offsetof(type,member)))

#endif





