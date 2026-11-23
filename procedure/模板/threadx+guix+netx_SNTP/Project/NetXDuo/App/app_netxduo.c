/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_netxduo.c
  * @author  MCD Application Team
  * @brief   NetXDuo applicative file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* USER CODE END Header */

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/* Includes ------------------------------------------------------------------*/
#include "app_netxduo.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "nxd_dhcp_client.h"
#include "nxd_dns.h"
#include "nxd_sntp_client.h"
#include "nx_web_http_client.h"

#include "user_global.h"
#include "rtc.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define NTP2UNIX_OFFSET  2208988800UL
#define UTC8_OFFSET      (8UL * 3600UL)
#define SNTP_SERVER_IP    IP_ADDRESS(203,107,6,88)
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TX_THREAD      NxAppThread;
NX_PACKET_POOL NxAppPool;
NX_IP          NetXDuoEthIpInstance;
/* USER CODE BEGIN PV */
/**IP参数*/
void ip_change_callback(NX_IP *ip_ptr, void *notify_ptr);

/*DHCP参数*/
NX_DHCP        dhcp_client;
static  TX_SEMAPHORE   dhcp_sem; 			//获取IP成功同步信号	
void dhcp_state_cb(NX_DHCP *dhcp_ptr, UCHAR new_state);

//TCP参数
//NX_TCP_SOCKET  TcpServerSocket;
//#define TCP_SERVER_PORT     8088
//#define TCP_WINDOW_SIZE     1024

/*DNS参数*/
//NX_DNS         dns_client;

/*SNTP参数*/

//#define NX_SNTP_CLIENT_UNICAST_POLL_INTERVAL        60 //sntp获取频率 60为一分钟（前提为NX_IP_PERIODIC_RATE = 1000）
//#define NX_SNTP_CLIENT_MAX_TIME_ADJUSTMENT					(24UL * 3600UL * 1000UL)
NX_SNTP_CLIENT sntp_client;
static void sntp_update_callback(NX_SNTP_TIME_MESSAGE *time_update_ptr, NX_SNTP_TIME *local_time);
static void unix_to_datetime(ULONG unix_t, NX_SNTP_DATE_TIME *tm);

/*HTTP参数*/
//NX_WEB_HTTP_CLIENT  web_client;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
static VOID nx_app_thread_entry (ULONG thread_input);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/**
  * @brief  Application NetXDuo Initialization.
  * @param memory_ptr: memory pointer
  * @retval int
  */
UINT MX_NetXDuo_Init(VOID *memory_ptr)
{
  UINT ret = NX_SUCCESS;
  TX_BYTE_POOL *byte_pool = (TX_BYTE_POOL*)memory_ptr;
  CHAR *pointer;

  /* USER CODE BEGIN MX_NetXDuo_MEM_POOL */
  /* USER CODE END MX_NetXDuo_MEM_POOL */

  /* USER CODE BEGIN 0 */
	
  /* USER CODE END 0 */

  /* Initialize the NetXDuo system. */
  nx_system_initialize();

    /* Allocate the memory for packet_pool.  */
  if (tx_byte_allocate(byte_pool, (VOID **) &pointer, NX_APP_PACKET_POOL_SIZE, TX_NO_WAIT) != TX_SUCCESS)
  {
    return TX_POOL_ERROR;
  }

  /* Create the Packet pool to be used for packet allocation,
   * If extra NX_PACKET are to be used the NX_APP_PACKET_POOL_SIZE should be increased
   */
  ret = nx_packet_pool_create(&NxAppPool, "NetXDuo App Pool", DEFAULT_PAYLOAD_SIZE, pointer, NX_APP_PACKET_POOL_SIZE);

  if (ret != NX_SUCCESS)
  {
    return NX_POOL_ERROR;
  }

    /* Allocate the memory for Ip_Instance */
  if (tx_byte_allocate(byte_pool, (VOID **) &pointer, Nx_IP_INSTANCE_THREAD_SIZE, TX_NO_WAIT) != TX_SUCCESS)
  {
    return TX_POOL_ERROR;
  }

   /* Create the main NX_IP instance */
  ret = nx_ip_create(&NetXDuoEthIpInstance, "NetX Ip instance", NX_APP_DEFAULT_IP_ADDRESS, NX_APP_DEFAULT_NET_MASK, &NxAppPool, nx_stm32_eth_driver,
                     pointer, Nx_IP_INSTANCE_THREAD_SIZE, 5);

  if (ret != NX_SUCCESS)
  {
    return NX_NOT_SUCCESSFUL;
  }

    /* Allocate the memory for ARP */
  if (tx_byte_allocate(byte_pool, (VOID **) &pointer, DEFAULT_ARP_CACHE_SIZE, TX_NO_WAIT) != TX_SUCCESS)
  {
    return TX_POOL_ERROR;
  }

  /* Enable the ARP protocol and provide the ARP cache size for the IP instance */

  /* USER CODE BEGIN ARP_Protocol_Initialization */
	nx_ip_address_change_notify(&NetXDuoEthIpInstance, ip_change_callback, NULL);
  /* USER CODE END ARP_Protocol_Initialization */

  ret = nx_arp_enable(&NetXDuoEthIpInstance, (VOID *)pointer, DEFAULT_ARP_CACHE_SIZE);

  if (ret != NX_SUCCESS)
  {
    return NX_NOT_SUCCESSFUL;
  }

  /* Enable the ICMP */

  /* USER CODE BEGIN ICMP_Protocol_Initialization */

  /* USER CODE END ICMP_Protocol_Initialization */

  ret = nx_icmp_enable(&NetXDuoEthIpInstance);

  if (ret != NX_SUCCESS)
  {
    return NX_NOT_SUCCESSFUL;
  }

  /* Enable TCP Protocol */

  /* USER CODE BEGIN TCP_Protocol_Initialization */

  /* USER CODE END TCP_Protocol_Initialization */

  ret = nx_tcp_enable(&NetXDuoEthIpInstance);

  if (ret != NX_SUCCESS)
  {
    return NX_NOT_SUCCESSFUL;
  }

  /* Enable the UDP protocol required for  DHCP communication */

  /* USER CODE BEGIN UDP_Protocol_Initialization */

  /* USER CODE END UDP_Protocol_Initialization */

  ret = nx_udp_enable(&NetXDuoEthIpInstance);

  if (ret != NX_SUCCESS)
  {
    return NX_NOT_SUCCESSFUL;
  }

   /* Allocate the memory for main thread   */
  if (tx_byte_allocate(byte_pool, (VOID **) &pointer, NX_APP_THREAD_STACK_SIZE, TX_NO_WAIT) != TX_SUCCESS)
  {
    return TX_POOL_ERROR;
  }

  /* Create the main thread */
  ret = tx_thread_create(&NxAppThread, "NetXDuo App thread", nx_app_thread_entry , 0, pointer, NX_APP_THREAD_STACK_SIZE,
                         NX_APP_THREAD_PRIORITY, NX_APP_THREAD_PRIORITY, TX_NO_TIME_SLICE, TX_AUTO_START);

  if (ret != TX_SUCCESS)
  {
    return TX_THREAD_ERROR;
  }

  /* USER CODE BEGIN MX_NetXDuo_Init */

  /* USER CODE END MX_NetXDuo_Init */

  return ret;
}

/**
* @brief  Main thread entry.
* @param thread_input: ULONG user argument used by the thread entry
* @retval none
*/
static VOID nx_app_thread_entry (ULONG thread_input)
{
  /* USER CODE BEGIN Nx_App_Thread_Entry 0 */
	UINT status;
    ULONG ip_addr, netmask,gw_addr;
    ULONG sntp_sec, frac;

	
		NX_PACKET *rx_pkt = NULL;
    UCHAR rcv_buf[128];
    ULONG rcv_len;
	
		/*==========DHCP获取IP==========================================*/
		tx_semaphore_create(&dhcp_sem,"dhcp_sem",0);
	
    status = nx_dhcp_create(&dhcp_client, &NetXDuoEthIpInstance, "dhcp client");
    if(status != NX_SUCCESS)
    {
        printf("nx_dhcp_create error:%d\r\n", status);
        while(1);
    }
		
		nx_dhcp_state_change_notify(&dhcp_client, dhcp_state_cb);
		
    status = _nxe_dhcp_start(&dhcp_client);
    if(status != NX_SUCCESS)
    {
        printf("_nxe_dhcp_start error:%d\r\n", status);
        while(1);
    }
		
    printf("DHCP started, waiting for IP...\r\n");

		tx_semaphore_get(&dhcp_sem,TX_WAIT_FOREVER);/*IP获取成功*/

		/*========== ping网关测试 测试是否获取成功，不可Ping自己==========*/
			NX_PACKET *ping_pkt = NULL;
			nx_ip_gateway_address_get(&NetXDuoEthIpInstance, &gw_addr);
			printf("gateway = 0x%08lX\r\n", gw_addr);
		
			nx_ip_address_get(&NetXDuoEthIpInstance,&ip_addr, &netmask);
			printf("Ping... IP: %08lX\r\n",ip_addr);
			tx_thread_sleep(5000);
			CHAR ping_data[32] = "pingtest";
		//netxwork不能ping自己
			status = nx_icmp_ping(&NetXDuoEthIpInstance, gw_addr, ping_data, sizeof(ping_data), &ping_pkt, 8000);
			if(status == NX_SUCCESS)
			{
					printf("Ping OK\r\n");
					if(ping_pkt != NULL)
					{
							nx_packet_release(ping_pkt);
							ping_pkt = NULL;
					}
			}
			else
			{
					printf("Ping FAIL, status:%d\r\n", status);
			}
			
		/*==========SNTP 先初次获取ntp时间，再次开启轮询获取==========================================*/
			status = nx_sntp_client_create(&sntp_client,
                                        &NetXDuoEthIpInstance,
                                        0,
                                        &NxAppPool,
                                        NX_NULL, NX_NULL, NX_NULL);
			if(status != NX_SUCCESS)
			{
					printf("[SNTP] nx_sntp_client_create fail:%d\r\n", status);
					while(1)
					{
							tx_thread_sleep(TX_TIMER_TICKS_PER_SECOND * 5);
					}
			}
			printf("[SNTP] sntp client create ok\r\n");

			status = nx_sntp_client_initialize_unicast(&sntp_client, SNTP_SERVER_IP);
			if(status != NX_SUCCESS)
			{
					printf("[SNTP] nx_sntp_client_initialize_unicast fail:%d\r\n", status);
					while(1)
					{
							tx_thread_sleep(TX_TIMER_TICKS_PER_SECOND * 5);
					}
			}
			printf("[SNTP] unicast init ok\r\n");

			nx_sntp_client_set_time_update_notify(&sntp_client, sntp_update_callback);

			status = nx_sntp_client_run_unicast(&sntp_client);
			if(status != NX_SUCCESS)
			{
					printf("[SNTP] run_unicast fail:%d\r\n", status);
			}

    while(1)
    {
			HAL_GPIO_TogglePin(GPIOB,GPIO_PIN_1);
			tx_thread_sleep(2000);
    }
			
  /* USER CODE END Nx_App_Thread_Entry 0 */

}
/* USER CODE BEGIN 2 */
/**
 * @brief 把Unix时间戳（UTC+8，北京时间，1970-01-01起算秒）转成年月日时分秒
 * @param unix_t  输入：Unix时间戳（已经加好东8区偏移）
 * @param tm      输出：年月日时分秒结构体
 */
static void unix_to_datetime(ULONG unix_t, NX_SNTP_DATE_TIME *tm)
{
    // 每个月天数，平年
    const UINT month_days[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
    ULONG days;
    ULONG sec;
    UINT year, mon;
    UINT is_leap;

    // 1. 拆分：总天数 + 当天剩余秒数
    days = unix_t / 86400UL;
    sec  = unix_t % 86400UL;

    // 时分秒
    tm->hour = sec / 3600UL;
    sec = sec % 3600UL;
    tm->minute  = sec / 60UL;
    tm->second  = sec % 60UL;

    // 2. 从1970年开始逐年找年份
    year = 1970;
    while(1)
    {
        // 判断闰年
        is_leap = ((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0);
        ULONG y_days = is_leap ? 366UL : 365UL;
        if(days < y_days)
        {
            break;
        }
        days -= y_days;
        year++;
    }
    tm->year = year;

    // 3. 逐月找月份
    for(mon = 0; mon < 12; mon++)
    {
        ULONG m_days = month_days[mon];
        if(mon == 1 && is_leap)
        {
            m_days = 29UL;
        }
        if(days < m_days)
        {
            break;
        }
        days -= m_days;
    }
    tm->month = mon + 1;    // 数组从0开始，月份从1开始
    tm->day = days + 1;   // 天数从1开始
}
void dhcp_state_cb(NX_DHCP *dhcp_ptr, UCHAR new_state)
{
    if(new_state == NX_DHCP_STATE_BOUND)
    {
        printf("[DHCP] BOUND，获取IP成功\r\n");
    }
    else if(new_state == NX_DHCP_STATE_INIT)
    {
        printf("[DHCP] 地址丢失，重新请求\r\n");
    }
		else
		{
			printf("[DHCP] %d\r\n",new_state);
			printf("[DHCP] 正在找服务器\r\n");
		}
}

void ip_change_callback(NX_IP *ip_ptr, void *notify_ptr)
{
    // 在回调内部，自己调用 nx_ip_address_get 去读IP！
    ULONG ip, mask;
    nx_ip_address_get(ip_ptr, &ip, &mask);
    if(ip != 0)
    {
        printf("[IP_READY] ARP探测完成，IP正式生效: %08lX\r\n", ip);
        tx_semaphore_put(&dhcp_sem);
    }
    else
    {
        printf("[IP_CLEAR] IP被清空\r\n");
    }
}

static void sntp_update_callback(NX_SNTP_TIME_MESSAGE *time_update_ptr, NX_SNTP_TIME *local_time)
{
    NX_SNTP_DATE_TIME tm;
    // time_update_ptr->seconds 是 NTP基准(1900) UTC
//    ULONG unix_utc = local_time->seconds - 2208988800UL;
//    unix_to_datetime(unix_utc, &tm);
//    printf("[SNTP] UTC: %04d-%02d-%02d %02d:%02d:%02d\r\n", tm.year, tm.month, tm.day, tm.hour, tm.minute, tm.second);

    // 北京时间 +8h
    ULONG ntp_bj = local_time->seconds + 8UL * 3600;
    ULONG unix_bj = ntp_bj - 2208988800UL;
    unix_to_datetime(unix_bj, &tm);
//    printf("[SNTP] Beijing: %04d-%02d-%02d %02d:%02d:%02d\r\n", tm.year, tm.month, tm.day, tm.hour, tm.minute, tm.second);

	// ========= 写入硬件RTC =========
   RTC_WriteClock(tm.year, tm.month, tm.day, tm.hour, tm.minute, tm.second);
//		 RTC_LOG_Printf("RTC写入成功\r\n");
}
/* USER CODE END 2 */
