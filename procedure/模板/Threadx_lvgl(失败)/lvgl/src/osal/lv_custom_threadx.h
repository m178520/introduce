/**
 * @file lv_freertos.h
 *
 */

/**
 * Copyright 2023 NXP
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef LV_FREERTOS_H
#define LV_FREERTOS_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lv_os_private.h"

#if LV_USE_OS == LV_OS_CUSTOM


#include "tx_api.h"

/*********************
 *      DEFINES
 *********************/
#define ulMAX_COUNT 10U
/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    void (*pvStartRoutine)(void *);       /**< Application thread function. */
    void * pTaskArg;                      /**< Arguments for application thread function. */
    TX_THREAD xThread;                    /**< ThreadX thread control block */
    void * pStackStart;                   /**< Thread stack buffer start address */
} lv_thread_t;

typedef struct {
    bool xIsInitialized;                  /**< Set to true if this mutex is initialized, false otherwise. */
    TX_MUTEX xMutex;                      /**< ThreadX mutex. */
    TX_THREAD * owner;                    /**< Recursive lock owner thread */
    uint32_t recur_cnt;                   /**< Recursive lock nest count */
} lv_mutex_t;

typedef struct {
    bool xIsInitialized;                  /**< Set to true if this condition variable is initialized, false otherwise. */
    bool xSyncSignal;                     /**< Set to true if the thread is signaled, false otherwise. */
    TX_SEMAPHORE xCondWaitSemaphore;      /**< Threads block on this semaphore in lv_thread_sync_wait. */
    uint32_t ulWaitingThreads;            /**< The number of threads currently waiting on this condition variable. */
    TX_MUTEX xSyncMutex;                  /**< Threads take this mutex before accessing the condition variable. */
} lv_thread_sync_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Set it for `traceTASK_SWITCHED_IN()` as
 * `lv_freertos_task_switch_in(pxCurrentTCB->pcTaskName)`
 * to save the start time stamp of a task
 * @param name      the name of the which is switched in
 */
void lv_freertos_task_switch_in(const char * name);

/**
 * Set it for `traceTASK_SWITCHED_OUT()` as
 * `lv_freertos_task_switch_out()`
 * to save finish time stamp of a task
 */
void lv_freertos_task_switch_out(void);


/**********************
 *      MACROS
 **********************/

#endif /*LV_USE_OS == LV_OS_FREERTOS*/

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_FREERTOS_H*/
