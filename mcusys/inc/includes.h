/************************************Copyright (c)**************************************

            单片机编程助手，帮您从繁杂的重复劳动中脱离出来，专心做您的创新与业务                  

                                    wwww.mcuabc.cn

----------------------------------------文件信息-----------------------------------------
文 件 名: 
创 建 人: 
email:	mcusys@qq.com
创建日期: 
描    述: 
----------------------------------------版本信息-----------------------------------------
 版    本: V1.00
 说    明: 
----------------------------------------------------------------------------------------
Copyright(C) xsy,ccmcu,zcw 2019/06/18
All rights reserved
****************************************************************************************/

#ifndef __INCLUDES_H__
#define __INCLUDES_H__
#include "string.h"
#include "stdio.h"
//-------------------芯片相关开始--------------------
//-------------------芯片相关结束--------------------

#include "main.h"
#include "..\main\mcusys_main.h"
//------------------ 系统头文件 ------------------
//+++++User_Code_include0 Begin+++++//
//-----User_Code_include0 End-----//

#include "..\sys\sys_base0\chksums.h"
#include "..\sys\sys_base0\fixlen_que.h"
#include "..\sys\sys_base0\ring_buf.h"
#include "..\sys\sys_base0\utility.h"
#include "..\sys\sys_base1\sys_timer.h"
//+++++User_Code_include1 Begin+++++//
//-----User_Code_include1 End-----//

#include "..\os\ccos\ccos.h"
#include "..\os\ccos\ccos_cfg.h"
//+++++User_Code_include2 Begin+++++//
//-----User_Code_include2 End-----//

//------------------ 功能模块库 --------------------
#include "..\funcs\func_usart\func_usart.h"
#include "..\funcs\func_key32\key.h"
#include "..\funcs\func_key32\func_key.h"
#include "..\funcs\func_flash_iap\func_flash_iap.h"
#include "..\funcs\func_flash_1b_store\func_flash_1b_store.h"
#include "..\funcs\modbus\modbus.h"
#include "..\funcs\func_modbus_master\func_modbus_master.h"
//+++++User_Code_include_funcs Begin+++++//
#include "..\temp\product_hr\product_hr.h"
#include "..\temp\atcmd\atcmd.h"
#include "..\temp\func_cm\func_cm.h"
#include "..\temp\user_ctrl\user_ctrl.h"
//-----User_Code_include_funcs End-----//
//----------- 用户应用头文件 -----------
#include "..\apps\apps.h"
//+++++User_Code_include3 Begin+++++//
//-----User_Code_include3 End-----//
//==================================================================
#endif
