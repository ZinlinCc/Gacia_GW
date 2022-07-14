#ifndef __KEY_H__
#define __KEY_H__
/*
按键库函数，最多支持32个按键
每次读取按键状态，与上一次进行比较后，整理出两次相同的状态，认为是按键消抖以后的稳定状态，
然后判断是否有新的按键被按下，有新的按键按下的话，在按键队列里面加入一个新的键值
*/
	#define KEY32_CODE_POINT_MASK 	0x07
	#define KEY32_CODE_LONGMARK		0x80
	#define KEY32_CODE_LONGMARK1	0x40
	#define KEY32_LONG_THRE			200			//10ms一次，200次=2秒
	#define KEY32_CNT_MAX			6005		//10ms一次，200次=2秒
	
	enum{
		KEY_CODE_NULL=0,
		KEY_CODE_UP,		//1
		KEY_CODE_DOWN,		//2
		KEY_CODE_LEFT,		//3
		KEY_CODE_RIGHT,		//4
		KEY_CODE_OK,		//5
		KEY_CODE_CANCEL,	//6
	};
	
	
	typedef struct{
		unsigned char code_head;	//按键代码的头指针，指向最早进入缓存的按键代码
		unsigned char code_tail;	//按键代码的尾指针，指向最后进入缓存的按键代码的下一个位置
		unsigned char code[KEY32_CODE_POINT_MASK+1];		//按键代码缓存区，每产生一个新的按键信息，代码加入缓存
	}KEY_CODE_T;
	
	typedef  struct{
		unsigned int input_last;	//按键原始状态，每位代表每个按键
		unsigned int sta;			//经过防抖处理的按键信息，每位代表每个按键当前的有效状态
		unsigned char option;		//每一位长键按过的信息，这个会取消释放时候的短键
		unsigned short lk_thre0;	//长键0时间
		unsigned short lk_thre1;	//长键1时间
		unsigned short cnt[32];		//长键计时器
		KEY_CODE_T key_code;
	}KEY32_T;
//----------------------------------------------------------------
//每10-20mS执行一次，新的键码送入缓冲区
#define KEY_OPTION_SHORT_NEG	0x01
void key32_init(KEY32_T *p,unsigned char option,unsigned short lk_thre0,unsigned short lk_thre1);
unsigned char key32_oper(KEY32_T *p,unsigned int key_input,unsigned char key_nb);
//----------------------------------------------------------------
//加入一个键码到缓冲区，成功返回1，失败（缓冲已满）返回0
unsigned char add_key32_code(KEY_CODE_T *p,unsigned char key_code);
//从按键缓冲区里面获取一个键码，成功返回非零，无键码返回0
unsigned char get_key32_code(KEY_CODE_T *p);
//----------------------------------------------------------------
#endif
