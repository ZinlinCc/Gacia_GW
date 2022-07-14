#ifndef __USER_CTRL_H__
#define __USER_CTRL_H__

//检测4G模块是否就绪
//返回
//0：未就绪
//1：就绪
int net_4g_sta(void);

//查询网络是否连接
int tcp_sta(void);

//连接网络
int tcp_connect(char *ip_port);

//断开网络
int tcp_disconnect(void);

//发送数据
int tcp_send(void *pdata,unsigned short n);

//检测接收缓冲区是否有数据
int tcp_chkrec(void);

//读取缓冲区数据，若需要读取的数据量大于缓冲区，返回错误
//正确返回读取的字节数
int tcp_rd_data(void *pdata,unsigned short max_n);



#endif
