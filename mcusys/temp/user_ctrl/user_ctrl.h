#ifndef __USER_CTRL_H__
#define __USER_CTRL_H__

//���4Gģ���Ƿ����
//����
//0��δ����
//1������
int net_4g_sta(void);

//��ѯ�����Ƿ�����
int tcp_sta(void);

//��������
int tcp_connect(char *ip_port);

//�Ͽ�����
int tcp_disconnect(void);

//��������
int tcp_send(void *pdata,unsigned short n);

//�����ջ������Ƿ�������
int tcp_chkrec(void);

//��ȡ���������ݣ�����Ҫ��ȡ�����������ڻ����������ش���
//��ȷ���ض�ȡ���ֽ���
int tcp_rd_data(void *pdata,unsigned short max_n);



#endif
