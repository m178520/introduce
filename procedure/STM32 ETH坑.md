# 网口遇到的问题

- 使用netx需要开启中断

- ETH的lan8720A芯片初始化必须硬件复位一下，而正点原子的复位引脚连接在PCF8574T（由IIC控制输出I/O芯片）上
一定要先初始化

>IIC如果在SCL SDA线上有3.3V+上拉电阻，可设置为开漏模式，SDA引脚，可读可写。
>IIC如果在SCL SDA线上没有3.3V+上拉电阻，只可能手动代码更改SDA引脚的模式。使其可读可写。

- cubemx生成的代码中，需手动
```c
#define NX_DRIVER_ETH_HW_IP_INIT
```
这样在IP初始化时才会初始化ETH硬件。放在nx_stm32_phy_driver.c文件中初始化，会有部分覆盖cubemx生成的一些ETH的配置，出现问题。

- 在TCP/IP服务器与客户端断开连接时，会出现连接不上的问题,可通过下面方法
```c
		// ========== TCP SERVER 初始化 ==========
    status = nx_tcp_socket_create(&NetXDuoEthIpInstance, &TcpServerSocket, "TcpServer",
                                  NX_IP_NORMAL, NX_FRAGMENT_OKAY, NX_IP_TIME_TO_LIVE,
                                  TCP_WINDOW_SIZE, NX_NULL, NX_NULL);
    if(status != NX_SUCCESS)
    {
        printf("nx_tcp_socket_create fail:%d\r\n", status);
        while(1);
    }
    printf("TCP Server ready, port %d\r\n",TCP_SERVER_PORT);
		
		// 监听端口
        status = nx_tcp_server_socket_listen(&NetXDuoEthIpInstance, TCP_SERVER_PORT, &TcpServerSocket, 5, NX_NULL);
        if(status != NX_SUCCESS)
        {
            printf("listen fail:%d\r\n", status);
            while(1);
        }
        printf("Wait client connect...\r\n");
				
		// TCP服务循环
    while(1)
    {
        // 等待客户端接入（阻塞）
        status = nx_tcp_server_socket_accept(&TcpServerSocket, 0xffff);
        if(status == NX_SUCCESS)
        {
            printf("Client Connected!\r\n");
            while(1)
            {
                // 接收，超时100tick
                status = nx_tcp_socket_receive(&TcpServerSocket, &rx_pkt, 100);
                if(status == NX_SUCCESS)
                {
                    nx_packet_data_retrieve(rx_pkt, rcv_buf, &rcv_len);
                    if(rcv_len < sizeof(rcv_buf))
                    {
                        rcv_buf[rcv_len] = '\0';
                        printf("Recv Data: %s\r\n", rcv_buf);
                    }
                    // Echo回发
                    nx_tcp_socket_send(&TcpServerSocket, rx_pkt, 100);
                    nx_packet_release(rx_pkt);
                    rx_pkt = NULL;
                }
                else if(status == NX_NO_PACKET)
                {
                    continue;
                }
                else
                {
                    printf("Client Disconnect, err:%d\r\n", status);
                    break;
                }
            }
            // 断开连接
			nx_tcp_server_socket_unaccept(&TcpServerSocket); //断开必不可少
            nx_tcp_socket_disconnect(&TcpServerSocket, 200);
						
        }
				else
				{
					printf("Client Disconnect timeout\r\n");
				}
        // 重新监听，等待下一个客户端
        // ========= 重点：relisten 必须判断返回值 =========
			status = nx_tcp_server_socket_relisten(&NetXDuoEthIpInstance, TCP_SERVER_PORT, &TcpServerSocket);
			if(status == NX_SUCCESS)
			{
					printf("relisten ok, wait new client...\r\n");
			}
			else
			{
					printf("relisten fail:%d, retry listen\r\n", status);
					// relisten失败，兜底重新listen
					tx_thread_sleep(20);
					nx_tcp_server_socket_listen(&NetXDuoEthIpInstance, TCP_SERVER_PORT, &TcpServerSocket, 5, NX_NULL);
			}
    }
```

- SNTP获取校准时间间隔问题
```c
#define NX_SNTP_CLIENT_UNICAST_POLL_INTERVAL        60 //sntp获取频率 60为一分钟（前提为NX_IP_PERIODIC_RATE = 1000）
#define NX_SNTP_CLIENT_MAX_TIME_ADJUSTMENT					(24UL * 3600UL * 1000UL) 
```
NX_SNTP_CLIENT_MAX_TIME_ADJUSTMENT是用来判断当前与获取的sntp时间差值大于其值，则本次数据丢弃，所以需要大一些
