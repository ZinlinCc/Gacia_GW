/************************************Copyright (c)**************************************

            ��Ƭ��������֣������ӷ��ӵ��ظ��Ͷ������������ר�������Ĵ�����ҵ��                  

                                    wwww.mcuabc.cn

----------------------------------------�ļ���Ϣ-----------------------------------------
�� �� ��: 
�� �� ��: 
email:	mcusys@qq.com
��������: 
��    ��: 
----------------------------------------�汾��Ϣ-----------------------------------------
 ��    ��: V1.00
 ˵    ��: 
----------------------------------------------------------------------------------------
Copyright(C) xsy,ccmcu,zcw 2019/06/18
All rights reserved
****************************************************************************************/

#ifndef __INCLUDES_H__
#define __INCLUDES_H__
#include "string.h"
#include "stdio.h"
//-------------------оƬ��ؿ�ʼ--------------------
//-------------------оƬ��ؽ���--------------------

#include "main.h"
#include "..\main\mcusys_main.h"
//------------------ ϵͳͷ�ļ� ------------------
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

//------------------ ����ģ��� --------------------
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
//----------- �û�Ӧ��ͷ�ļ� -----------
#include "..\apps\apps.h"
//+++++User_Code_include3 Begin+++++//
//-----User_Code_include3 End-----//
//==================================================================
#endif
