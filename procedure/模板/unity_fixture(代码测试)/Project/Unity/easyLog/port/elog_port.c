/*
 * This file is part of the EasyLogger Library.
 *
 * Copyright (c) 2015, Armink, <armink.ztl@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * 'Software'), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Function: Portable interface for each platform.
 * Created on: 2015-04-28
 */

#include <elog.h>
#include <stdio.h>


static void format_time_str(char* buf, uint32_t h, uint32_t m, uint32_t s, uint32_t ms) {
  buf[0] = '0' + h / 10;
  buf[1] = '0' + h % 10;
  buf[2] = ':';
  buf[3] = '0' + m / 10;
  buf[4] = '0' + m % 10;
  buf[5] = ':';
  buf[6] = '0' + s / 10;
  buf[7] = '0' + s % 10;
  buf[8] = '.';
  buf[9] = '0' + ms / 100;
  buf[10] = '0' + (ms % 100) / 10;
  buf[11] = '0' + ms % 10;
  buf[12] = '\0';
}

/**
 * EasyLogger port initialize
 *
 * @return result
 */
ElogErrCode elog_port_init(void) {
  ElogErrCode result = ELOG_NO_ERR;

  /* add your code here */

  return result;
}

/**
 * EasyLogger port deinitialize
 *
 */
void elog_port_deinit(void) {

  /* add your code here */
}

/**
 * output log port interface
 *
 * @param log output of log
 * @param size log size
 */
void elog_port_output(const char* log, size_t size) {

  /* add your code here */
  size_t i;
    for(i = 0; i < size; i++)
    {
        (void)fputc((uint8_t)log[i], stdout);
    }
}

/**
 * output lock
 */
void elog_port_output_lock(void) {

  /* add your code here */
}

/**
 * output unlock
 */
void elog_port_output_unlock(void) {

  /* add your code here */
}

/**
 * get current time interface
 *
 * @return current time
 */
const char* elog_port_get_time(void) {

  /* add your code here */
  //static char time_str[13];
//  DTL dt={0};
//  Int ret=0;
//  RD_SYS_T(true, &dt, &ret);
//  static char buf[35];
//  snprintf(buf, sizeof(buf),"%04u-%02u-%02uT%02u:%02u:%02u.%03u", dt.YEAR, dt.MONTH, dt.DAY, dt.HOUR, dt.MINUTE, dt.SECOND, dt.MILLISECOND);
  //format_time_str(time_str, dt.HOUR, dt.MINUTE, dt.SECOND, dt.MILLISECOND);
//  return buf;
	return NULL;
}

/**
 * get current process name interface
 *
 * @return current process name
 */
const char* elog_port_get_p_info(void) {
  /*进程名接口:返回空字符串。若无 return，返回值未定义，GCC 优化后会返回随机指
  口士格寸化时按字符串访问非法地址，触发 HardFault */
  return "";
}

/**
 * get current thread name interface
 *
 * @return current thread name
 */
const char* elog_port_get_t_info(void) {
  /* 线程接口名称返回空字符串。原因同 elog_port_get_p_info */ 
  return "";
}