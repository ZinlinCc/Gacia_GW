#ifndef __SYS_TIMER_H__
#define __SYS_TIMER_H__

	#ifdef SYS_TIMER_MAIN
	 	#define SYS_TIMER_EXT
	#else
		#define SYS_TIMER_EXT extern
	#endif
	
	
	#include "..\sys_base0\utility.h"
		
		
	typedef struct {
		unsigned char div_qs;
		unsigned char div_1s;
		unsigned char div_10ms;
		unsigned long cnt_1s;
		unsigned char b_hs;
		//unsigned char b_flash_250ms;
		CALENDAR_T calendar;
	}TIMER_T;
	
	
	SYS_TIMER_EXT unsigned char b_flash_250ms;
	
	
	SYS_TIMER_EXT TIMER_T g_timer;
	
	void sys_timer(TIMER_T *p);
	void mcusys_systick(void);

#endif
