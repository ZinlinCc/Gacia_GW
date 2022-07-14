#ifndef __MODBUS_H__
#define __MODBUS_H__

//==============================================================================
//+++++User_Code_define Begin+++++//
//-----User_Code_define End-----//	

#ifndef __MODBUS_DEFINE
	#define MODBUS_HR_EN		//HR读写使能
	//#define MODBUS_RW_EN		//RW指令使能
	//#define MODBUS_COIL_EN		//线圈操作使能
	//#define MODBUS_COIL_OFS_EN	//线圈地址偏移使能
	//#define MODBUS_INPUT_EN		//输入状态读取使能
	//#define MODBUS_INPUT_OFS_EN	//输入状态读取地址偏移使能
#endif
//==============================================================================
#ifdef MODBUS_COIL_OFS_EN
	#define COIL_OFS p->coil_ofs
#else
	#define COIL_OFS 0
#endif

#ifdef MODBUS_INPUT_OFS_EN
	#define INPUT_OFS p->input_ofs
#else
	#define INPUT_OFS 0
#endif

#define MODBUS_SLV_BASE_NB	5

typedef enum{
MD_FUNC_NULL,
MD_RD_COIL=1,
MD_RD_INPUT=2,
MD_RD_HR=3,
MD_FR_SCOIL=5,
MD_FR_SHR=6,
MD_FR_MCOIL=15,
MD_FR_MHR=16,
MD_FR_MHR_RDHR=23
} MD_FUNC;


#define MODBUS_FAIL				0
#define MODBUS_SUCCESS			1
#define MODBUS_SUCCESS_EXT		2
#define MODBUS_FORMAT_OK		0
#define MODBUS_ERR_TIMEOUT      -1      //超时
#define MODBUS_ERR_BYTELESS     -2      //字节数少
#define MODBUS_ERR_CRC          -3      //CRC错
#define MODBUS_ERR_SLV          -4      //从机地址错
#define MODBUS_ERR_FUNC         -5      //从机命令错
#define MODBUS_ERR_BYTE         -6      //接收字节数错
#define MODBUS_ERR_BYTECOUNT    -7      //ByteCount错
#define MODBUS_ERR_ADR          -8      //地址错
#define MODBUS_ERR_DAT          -9      //数据错
#define MODBUS_ERR_N            -10     //字节数错
#define MODBUS_SLV_RETURN_NML	1
#define MODBUS_SLV_RETURN_EXT	2

//MODBUS 常量结构体
typedef struct {
	unsigned char slv;		//从机地址
	unsigned char func;		//命令
	unsigned char rec_sta;	//接收状态
	unsigned char b_ext;	//状态
#ifdef MODBUS_COIL_OFS_EN
	unsigned short coil_ofs;//线圈HR偏移量
#endif
#ifdef MODBUS_INPUT_OFS_EN
	unsigned short input_ofs;//输入点HR偏移量
#endif
	unsigned short hr_n;	//HR的数量
	unsigned short *phr;	//HR地址
	unsigned short da_adr;	//数据地址
	unsigned short da_n;	//数据个数/数据内容
	unsigned short rww_adr;	//RW命令的写入地址
	unsigned short rww_n;	//RW命令的写入数量
	}MODBUS_T;
	
	#define	MODBUS_ADR_BOADCAST 0x00
	#define MODBUS_SLV_IAP 0xff
	
	#define MODBUS_STA_BOADCAST 0x01



typedef enum {
	MD_MST_STA_NULL,
	MD_MST_STA_OK,			//发送并接收完成
	MD_MST_STA_READY,		//上一帧完成，可以启动发送
	MD_MST_STA_INPROCESS,	//正在进程中
	MD_MST_STA_SEND_ERR,	//发送错误
	MD_MST_STA_REC_ERR		//接收错误
} MD_MST_STA;


extern int modbus_slv_rec(MODBUS_T *p,unsigned char *rb,int n);
extern int modbus_slv_send(MODBUS_T *p,unsigned char *rb);
extern int modbus_master_send(MODBUS_T *p,unsigned char *rb);
extern int modbus_master_rec(MODBUS_T *p,unsigned char *rb,int n);

#define MODBUS_OT_RO	1
#define MODBUS_OT_RW	0

#define MODBUS_SLV_NULL	0		//无操作
#define MODBUS_SLV_OK	1		//操作正确
#define MODBUS_SLV_FORMAT_OK	2	//格式正确,但不是自己的数据包
#define MODBUS_SLV_FORMAT_ERR	3	//格式错误
#define MODBUS_SLV_FUNC_ERR		4	//命令错误
//Modbus从模式函数;p:串口结构体;phr:HR的指针,option:bit0=1只读,=0读写


//Modbus主模式函数;p:串口结构体;pm:主模式命令结构体指针,
//b_dir:是否直接控制,=0读取、写入内容根据数据地址偏移对.phr.pwr操作,=1忽略数据地址只接写入.phr.pwr
#define MODBUS_MASTER_RW_DIR 1
#define MODBUS_MASTER_RW_HR 0
//extern void task_modbus_master(USART_T *p,struct ST_MODBUS_MASTER *pm,u8 b_dir);



//---------------------------------------------------------------------------
#define MODBUS_COIL(ADR)		((HR[(ADR)/16 + MODBUS_SLV_COIL_OFS] & (0x01 << (ADR&0x0f)))!=0) 	//返回COIL地址所在的内容
#define SET_MODBUS_COIL(ADR)	(HR[(ADR)/16 + MODBUS_SLV_COIL_OFS] |= (0x01 << (ADR&0x0f)))		//置位COIL
#define CLR_MODBUS_COIL(ADR)	(HR[(ADR)/16 + MODBUS_SLV_COIL_OFS] &= ~(0x01 << (ADR&0x0f)))	//清零COIL
#define CPL_MODBUS_COIL(ADR)	(HR[(ADR)/16 + MODBUS_SLV_COIL_OFS] ^= (0x01 << (ADR&0x0f)))		//翻转COIL

#define MODBUS_INPUT(ADR)		((HR[(ADR)/16 + MODBUS_SLV_INPUT_OFS] & (0x01 << (ADR&0x0f)))!=0) 	//返回INPUT地址所在的内容
#define SET_MODBUS_INPUT(ADR)	(HR[(ADR)/16 + MODBUS_SLV_INPUT_OFS] |= (0x01 << (ADR&0x0f)))		//置位INPUT
#define CLR_MODBUS_INPUT(ADR)	(HR[(ADR)/16 + MODBUS_SLV_INPUT_OFS] &= ~(0x01 << (ADR&0x0f)))	//清零INPUT
#define CPL_MODBUS_INPUT(ADR)	(HR[(ADR)/16 + MODBUS_SLV_INPUT_OFS] ^= (0x01 << (ADR&0x0f)))		//翻转INPUT
//---------------------------------------------------------------------------
#define MODBUS_ADR(HRN) (unsigned short)(&HRN-HR)



#endif
