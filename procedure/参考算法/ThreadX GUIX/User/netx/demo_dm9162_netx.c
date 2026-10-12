/*
*********************************************************************************************************
*
*	模块名称 : 网络协议栈测试
*	文件名称 : demo_dm9162_netx.c
*	版    本 : V1.0
*	说    明 : 测试的功能说明
*              1. 默认IP地址192.168.28.245，在本文件配置，用户可根据需要修改。
*              2. 可以在电脑端用网络调试软件创建TCP Client连接此服务器端，端口号1001。
*              3. 实现了一个简单的回环通信，用户使用上位机发送的数据通过板子返回到上位机。
*
*	修改记录 :
*		版本号   日期         作者        说明
*		V1.0    2021-01-30   Eric2013     首发
*
*	Copyright (C), 2018-2030, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/	
#include "includes.h"	



/*
*********************************************************************************************************
*	                                    IP相关
*********************************************************************************************************
*/
#define DEFAULT_PORT                    1001    /* TCP服务器监听端口号 */

#define IP_ADDR0                        192
#define IP_ADDR1                        168
#define IP_ADDR2                        28
#define IP_ADDR3                        245

#define  MAX_TCP_CLIENTS                1

/*
*********************************************************************************************************
*	                                    NetX任务和通信组件
*********************************************************************************************************
*/
/* 上电先将其设置到低优先级，待网线插入后提升优先级到3 */
#define  APP_CFG_TASK_NETX_PRIO                           28u    
#define  APP_CFG_TASK_NETX_STK_SIZE                     4096u
static   uint64_t  AppTaskNetXStk[APP_CFG_TASK_NETX_STK_SIZE/8];

/* 提升NetX处理任务优先级到3 */
#define  APP_CFG_TASK_NETX_PRIO1                           3u 

/* 提升NetX应用任务优先级到6 */
#define  APP_CFG_TASK_NetXPro_PRIO1                        6u

NX_TCP_SOCKET TCPSocket;
TX_SEMAPHORE  Semaphore;


/*
*********************************************************************************************************
*	                                    NetX任务和通信组件
*********************************************************************************************************
*/
UCHAR data_buffer[4096];

NX_PACKET_POOL    pool_0;
NX_IP             ip_0;  

/* 数据包内存池 */
#define NX_PACKET_POOL_SIZE ((1536 + sizeof(NX_PACKET)) * 20)
ULONG  packet_pool_area[NX_PACKET_POOL_SIZE/4 + 4];

/* ARP缓存 */
ULONG    arp_space_area[512 / sizeof(ULONG)];                                                      
ULONG    error_counter;


#define PRINT_DATA(addr, port, data)        do {                                            \
                                                  printf("[%lu.%lu.%lu.%lu:%u] -> '%s' \r\n", \
                                                  (addr >> 24) & 0xff,                      \
                                                  (addr >> 16) & 0xff,                      \
                                                  (addr >> 8) & 0xff,                       \
                                                  (addr & 0xff), port, data);               \
                                               } while(0)

extern TX_THREAD   AppTaskNetXProTCB;
extern TX_THREAD   *netx_thread_ptr;                                               
extern VOID  nx_driver_stm32h7xx(NX_IP_DRIVER *driver_req_ptr);
static VOID tcp_listen_callback(NX_TCP_SOCKET *socket_ptr, UINT port);
  
                                               
/*
*********************************************************************************************************
*	函 数 名: NetXTest
*	功能说明: TCPnet应用
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/    
void NetXTest(void)
{
    UINT status;
    UINT ret;
    ULONG socket_state;
    UINT old_priority;

    ULONG source_ip_address;
    NX_PACKET *data_packet;

    UINT source_port;
    ULONG bytes_read;

    /* 创建信号量，用于通知TCP服务器监听到新连接 */
    tx_semaphore_create(&Semaphore, "App Semaphore", 0);
    

    /* 初始化NetX */
    nx_system_initialize();

    /* 创建内存池 */
    status =  nx_packet_pool_create(&pool_0, 
                                     "NetX Main Packet Pool", 
                                     1536,  (ULONG*)(((int)packet_pool_area + 15) & ~15) , 
                                     NX_PACKET_POOL_SIZE);

    /* 检测创建是否失败 */
    if (status) error_counter++;

    /* 例化IP */
    status = nx_ip_create(&ip_0, 
                            "NetX IP Instance 0", 
                            IP_ADDRESS(IP_ADDR0, IP_ADDR1, IP_ADDR2, IP_ADDR3), 
                            0xFFFFFF00UL, 
                            &pool_0, nx_driver_stm32h7xx,
                            (UCHAR*)AppTaskNetXStk,
                            sizeof(AppTaskNetXStk),
                            APP_CFG_TASK_NETX_PRIO);
                            
            
    /* 检测创建是否失败 */
    if (status) error_counter++;

    /* 使能ARP，并提供ARP缓存 */
    status =  nx_arp_enable(&ip_0, (void *)arp_space_area, sizeof(arp_space_area));

    /* 使能fragment */    
    status = nx_ip_fragment_enable(&ip_0);

    /* 检测使能成功 */
    if (status) error_counter++;

    /* 使能TCP */
    status =  nx_tcp_enable(&ip_0);

    /* 检测使能成功 */
    if (status) error_counter++;

    /* 使能UDP  */
    status =  nx_udp_enable(&ip_0);

    /* 检测使能成功 */
    if (status) error_counter++;

    /* 使能ICMP */
    status =  nx_icmp_enable(&ip_0);

    /* 检测使能成功 */
    if (status) error_counter++;   
    
    /* NETX初始化完毕后，重新设置优先级 */
    tx_thread_priority_change(netx_thread_ptr, APP_CFG_TASK_NETX_PRIO1, &old_priority);
    tx_thread_priority_change(&AppTaskNetXProTCB, APP_CFG_TASK_NetXPro_PRIO1, &old_priority);
    
    /////////////////////////////////////////////////////////////////////////////
    /* 创建TCP Socket */
    ret = nx_tcp_socket_create(&ip_0, 
                               &TCPSocket, 
                               "TCP Server Socket", 
                               NX_IP_NORMAL, 
                               NX_FRAGMENT_OKAY,
                               NX_IP_TIME_TO_LIVE, 
                               4320, 
                               NX_NULL, 
                               NX_NULL);
    if (ret)
    {
		Error_Handler(__FILE__, __LINE__);    
    }

    /*
    * 监听新的链接。
    * 创建回调TCP_listen_callback表示监听到新连接。
    */
    ret = nx_tcp_server_socket_listen(&ip_0, DEFAULT_PORT, &TCPSocket, MAX_TCP_CLIENTS, tcp_listen_callback);

    if (ret)
    {
        Error_Handler(__FILE__, __LINE__);
    }

    if(tx_semaphore_get(&Semaphore, TX_WAIT_FOREVER) != TX_SUCCESS)
    {

    }
    else
    {
        /* 启动TCP通信前，接收新连接 */
        ret = nx_tcp_server_socket_accept(&TCPSocket, TX_WAIT_FOREVER);

        if (ret)
        {
            Error_Handler(__FILE__, __LINE__);
        }
    }
    
    
	while(1)
	{
        TX_MEMSET(data_buffer, '\0', sizeof(data_buffer));

        /* 获取socket状态 */
        nx_tcp_socket_info_get(&TCPSocket, NULL, NULL, NULL, NULL, NULL, NULL, NULL, &socket_state, NULL, NULL, NULL);

        /* 如果连接还没有建立，继续接受新连接，成功的话开启接收数据 */
        if(socket_state != NX_TCP_ESTABLISHED)
        {
            ret = nx_tcp_server_socket_accept(&TCPSocket, NX_IP_PERIODIC_RATE);
        }

        if(ret == NX_SUCCESS)
        {
            /* 接收TCP客户端发的TCP数据包 */
            ret = nx_tcp_socket_receive(&TCPSocket, &data_packet, NX_WAIT_FOREVER);

            if (ret == NX_SUCCESS)
            {
                /* 获取客户端的IP地址和端口 */
                nx_udp_source_extract(data_packet, &source_ip_address, &source_port);

                /* 获取客户端发来的数据 */
                nx_packet_data_retrieve(data_packet, data_buffer, &bytes_read);

                /* 打印接收到数据 */
                PRINT_DATA(source_ip_address, source_port, data_buffer);

                /* 立即将接收到的数据发送回去 */
                ret =  nx_tcp_socket_send(&TCPSocket, data_packet, NX_IP_PERIODIC_RATE);

                if (ret == NX_SUCCESS)
                {

                }

                /* 释放数据包 */
                nx_packet_release(data_packet);
            }
            else
            {
                /* 断开连接，重新监听 */
                nx_tcp_socket_disconnect(&TCPSocket, NX_WAIT_FOREVER);
                nx_tcp_server_socket_unaccept(&TCPSocket);
                nx_tcp_server_socket_relisten(&ip_0, DEFAULT_PORT, &TCPSocket);
            }
        }
        else
        {
            /* 处于空闲状态 */
            
        }
	}  	
}

/*
*********************************************************************************************************
*	函 数 名: tcp_listen_callback
*	功能说明: 服务器监测连接
*	形    参: -
*	返 回 值: 无
*********************************************************************************************************
*/  
static VOID tcp_listen_callback(NX_TCP_SOCKET *socket_ptr, UINT port)
{
    tx_semaphore_put(&Semaphore);
}   

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
