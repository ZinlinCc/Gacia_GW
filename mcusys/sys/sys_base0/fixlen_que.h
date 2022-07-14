#ifndef __FIXLEN_QUE_H__
#define __FIXLEN_QUE_H__

//--------------------------------------------------------
//版本号管理
#define FIXLEN_QUE_VER_H 100
//--------------------------------------------------------

#include "ring_buf.h"

#define FIXLEN_QUE_T RING_BUF_T


extern int fixlen_que_Wr(FIXLEN_QUE_T *pr,unsigned char *buf,unsigned short n);
extern int fixlen_que_Rd(FIXLEN_QUE_T *pr,unsigned char *buf,unsigned short buf_size);
int fixlen_que_firsize(FIXLEN_QUE_T *pr);

#define fixlen_que_Clr Ring_Buf_Clr
#define fixlen_que_Init Ring_Buf_Init

#endif
