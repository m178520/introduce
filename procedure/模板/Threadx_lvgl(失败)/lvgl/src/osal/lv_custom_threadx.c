/**
 * @file lv_freertos.c
 *
 */

/**
 * Copyright 2023 NXP
 *
 * SPDX-License-Identifier: MIT
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_os_private.h"
#if LV_USE_OS == LV_OS_CUSTOM


#include "../core/lv_global.h"

/*********************
 *      DEFINES
 *********************/

#define ulMAX_COUNT 10U

#define globals LV_GLOBAL_DEFAULT()

#define ATOMIC_COMPARE_AND_SWAP_SUCCESS 1
#define ATOMIC_COMPARE_AND_SWAP_FAIL    0
/**********************
 *      TYPEDEFS
 **********************/
static  TX_THREAD   TaskStartTCB;
static  uint8_t TaskStartStk[LV_DRAW_THREAD_STACK_SIZE];
/**********************
 *  STATIC PROTOTYPES
 **********************/

static uint32_t Atomic_CompareAndSwap_u32(volatile uint32_t * pDestination,
                                          uint32_t newValue,
                                          uint32_t comparand );

static uint32_t Atomic_Increment_u32(volatile uint32_t * pValue );

static void prvRunThread(ULONG pxArg);

static void prvMutexInit(lv_mutex_t * pxMutex);

static void prvCheckMutexInit(lv_mutex_t * pxMutex);

static void prvCondInit(lv_thread_sync_t * pxCond);

static void prvCheckCondInit(lv_thread_sync_t * pxCond);

static void prvCheckCondInitIsr(lv_thread_sync_t * pxCond);

#if !LV_USE_FREERTOS_TASK_NOTIFY
static void prvTestAndDecrement(lv_thread_sync_t * pxCond,
                                uint32_t ulLocalWaitingThreads);
#endif

/**********************
 *  STATIC VARIABLES
 **********************/

#ifdef ESP_PLATFORM
    static portMUX_TYPE critSectionMux = portMUX_INITIALIZER_UNLOCKED;
#endif

/**********************
 *      MACROS
 **********************/


    #define _enter_critical()  \
							TX_INTERRUPT_SAVE_AREA  \
							TX_DISABLE
    #define _exit_critical()    TX_RESTORE
    #define _enter_critical_isr() \
							TX_INTERRUPT_SAVE_AREA  \
              TX_DISABLE
    #define _exit_critical_isr(x) TX_RESTORE

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

lv_result_t lv_thread_init(lv_thread_t * pxThread,  const char * const name,
                           lv_thread_prio_t xSchedPriority,
                           void (*pvStartRoutine)(void *), size_t usStackSize,
                           void * xAttr)
{
		UINT status;
    pxThread->pTaskArg = xAttr;
    pxThread->pvStartRoutine = pvStartRoutine;
	
	#if LV_USE_OS == LV_OS_CUSTOM
	 
	pxThread->pStackStart = lv_malloc(usStackSize);
	lv_memzero(pxThread->pStackStart, usStackSize);
	#endif
    status = tx_thread_create(&pxThread->xThread,
                               (CHAR *)name,
                               prvRunThread,
                               (ULONG)0,
                               pxThread->pStackStart,
                               usStackSize,
                               (UINT)xSchedPriority,
                               (UINT)xSchedPriority,
                               TX_NO_TIME_SLICE,
                               TX_AUTO_START);
    if(status != TX_SUCCESS)
    {
        LV_LOG_ERROR("tx_thread_create failed! status:%d", status);
        return LV_RESULT_INVALID;
    }
    return LV_RESULT_OK;
}

lv_result_t lv_thread_delete(lv_thread_t * pxThread)
{
    tx_thread_terminate(&pxThread->xThread);
    tx_thread_delete(&pxThread->xThread);
    return LV_RESULT_OK;
}

lv_result_t lv_mutex_init(lv_mutex_t * pxMutex)
{
    /* If mutex in uninitialized, perform initialization. */
    prvCheckMutexInit(pxMutex);

    return LV_RESULT_OK;
}

lv_result_t lv_mutex_lock(lv_mutex_t * pxMutex)
{
    /* If mutex in uninitialized, perform initialization. */
		prvCheckMutexInit(pxMutex);
    UINT status;
    TX_THREAD * self = tx_thread_identify();
    if(pxMutex->owner == self)
    {
        pxMutex->recur_cnt++;
        return LV_RESULT_OK;
    }

    status = tx_mutex_get(&pxMutex->xMutex, TX_WAIT_FOREVER);
    if(status != TX_SUCCESS)
    {
        LV_LOG_ERROR("tx_mutex_get failed!");
        return LV_RESULT_INVALID;
    }
    pxMutex->owner = self;
    pxMutex->recur_cnt = 1;
    return LV_RESULT_OK;
}

lv_result_t lv_mutex_lock_isr(lv_mutex_t * pxMutex)
{
    /* If mutex in uninitialized, perform initialization. */
//    prvCheckMutexInit(pxMutex);

//    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

//    BaseType_t xMutexTakeStatus = xSemaphoreTakeFromISR(pxMutex->xMutex, &xHigherPriorityTaskWoken);
//    if(xMutexTakeStatus != pdTRUE) {
//        LV_LOG_ERROR("xSemaphoreTake failed!");
//        return LV_RESULT_INVALID;
//    }

//    /* If xHigherPriorityTaskWoken is now set to pdTRUE then a context switch
//    should be performed to ensure the interrupt returns directly to the highest
//    priority task.  The macro used for this purpose is dependent on the port in
//    use and may be called portEND_SWITCHING_ISR(). */
//    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

    return LV_RESULT_OK;
}

lv_result_t lv_mutex_unlock(lv_mutex_t * pxMutex)
{
    /* If mutex in uninitialized, perform initialization. */
    prvCheckMutexInit(pxMutex);
    TX_THREAD * self = tx_thread_identify();
    if(pxMutex->owner != self)
    {
        LV_LOG_ERROR("mutex unlock: not owner!");
        return LV_RESULT_INVALID;
    }
    pxMutex->recur_cnt --;
    if(pxMutex->recur_cnt > 0)
    {
        return LV_RESULT_OK;
    }
    pxMutex->owner = NULL;
    UINT status = tx_mutex_put(&pxMutex->xMutex);
    if(status != TX_SUCCESS)
    {
        LV_LOG_ERROR("tx_mutex_put failed!");
        return LV_RESULT_INVALID;
    }
    return LV_RESULT_OK;
}

lv_result_t lv_mutex_delete(lv_mutex_t * pxMutex)
{
    if(pxMutex->xIsInitialized == TX_FALSE)
        return LV_RESULT_INVALID;
    tx_mutex_delete(&pxMutex->xMutex);
    pxMutex->owner = NULL;
    pxMutex->recur_cnt = 0;
    pxMutex->xIsInitialized = TX_FALSE;
    return LV_RESULT_OK;
}

lv_result_t lv_thread_sync_init(lv_thread_sync_t * pxCond)
{
    /* If the cond is uninitialized, perform initialization. */
    prvCheckCondInit(pxCond);

    return LV_RESULT_OK;
}
lv_result_t lv_thread_sync_wait(lv_thread_sync_t * pxCond)
{
    lv_result_t lvRes = LV_RESULT_OK;

    /* If the cond is uninitialized, perform initialization. */
    prvCheckCondInit(pxCond);

    uint32_t ulLocalWaitingThreads;
		UINT status;
    /* Acquire the mutex. */
    tx_mutex_get(&pxCond->xSyncMutex, TX_WAIT_FOREVER);

    while(!pxCond->xSyncSignal) {
        /* Increase the counter of threads blocking on condition variable, then
         * release the mutex. */

        /* Atomically increments thread waiting by 1, and
         * stores number of threads waiting before increment. */
        ulLocalWaitingThreads = Atomic_Increment_u32(&pxCond->ulWaitingThreads);

        status = tx_mutex_put(&pxCond->xSyncMutex);

        /* Wait on the condition variable. */
        if(status == TX_SUCCESS) {
            UINT semStatus = tx_semaphore_get(&pxCond->xCondWaitSemaphore, TX_WAIT_FOREVER);

            /* Relock the mutex. */
            status = tx_mutex_get(&pxCond->xSyncMutex, TX_WAIT_FOREVER);

            if(semStatus != TX_SUCCESS) {
                LV_LOG_ERROR("xSemaphoreTake(xCondWaitSemaphore) failed!");
                lvRes = LV_RESULT_INVALID;

                /* Atomically decrements thread waiting by 1.
                 * If iLocalWaitingThreads is updated by other thread(s) in between,
                 * this implementation guarantees to decrement by 1 based on the
                 * value currently in pxCond->ulWaitingThreads. */
                prvTestAndDecrement(pxCond, ulLocalWaitingThreads + 1);
            }
        }
        else {
            LV_LOG_ERROR("xSemaphoreGive(xSyncMutex) failed!");
            lvRes = LV_RESULT_INVALID;

            /* Atomically decrements thread waiting by 1.
             * If iLocalWaitingThreads is updated by other thread(s) in between,
             * this implementation guarantees to decrement by 1 based on the
             * value currently in pxCond->ulWaitingThreads. */
            prvTestAndDecrement(pxCond, ulLocalWaitingThreads + 1);
        }
    }

    pxCond->xSyncSignal = TX_FALSE;

    /* Release the mutex. */
    tx_mutex_put(&pxCond->xSyncMutex);

    return lvRes;
}

lv_result_t lv_thread_sync_signal(lv_thread_sync_t * pxCond)
{
    /* If the cond is uninitialized, perform initialization. */
    prvCheckCondInit(pxCond);

		UINT status;
    /* Acquire the mutex. */
    tx_mutex_get(&pxCond->xSyncMutex, TX_WAIT_FOREVER);

    pxCond->xSyncSignal = TX_TRUE;

    /* Local copy of number of threads waiting. */
    uint32_t ulLocalWaitingThreads = pxCond->ulWaitingThreads;

    /* Test local copy of threads waiting is larger than zero. */
    while(ulLocalWaitingThreads > 0) {
        /* Atomically check whether the copy in memory has changed.
         * If not, set the copy of threads waiting in memory to zero. */
        if(ATOMIC_COMPARE_AND_SWAP_SUCCESS == Atomic_CompareAndSwap_u32(
               &pxCond->ulWaitingThreads,
               0,
               ulLocalWaitingThreads)) {
            /* Unblock all. */
            for(uint32_t i = 0; i < ulLocalWaitingThreads; i++) {
                tx_semaphore_put(&pxCond->xCondWaitSemaphore);
            }

            break;
        }

        /* Local copy is out dated. Reload from memory and retry. */
        ulLocalWaitingThreads = pxCond->ulWaitingThreads;
    }

    /* Release the mutex. */
    tx_mutex_put(&pxCond->xSyncMutex);

    return LV_RESULT_OK;
}

lv_result_t lv_thread_sync_delete(lv_thread_sync_t * pxCond)
{
#if !LV_USE_FREERTOS_TASK_NOTIFY
    /* Cleanup all resources used by the cond. */
		tx_semaphore_delete(&pxCond->xCondWaitSemaphore);
    tx_mutex_delete(&pxCond->xSyncMutex);
    pxCond->ulWaitingThreads = 0;
#endif
    pxCond->xSyncSignal = TX_FALSE;
    pxCond->xIsInitialized = TX_FALSE;

    return LV_RESULT_OK;
}

lv_result_t lv_thread_sync_signal_isr(lv_thread_sync_t * pxCond)
{
//    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

//    /* If the cond is uninitialized, perform initialization. */
//    prvCheckCondInitIsr(pxCond);

//#if LV_USE_FREERTOS_TASK_NOTIFY
//    uint32_t mask = _enter_critical_isr();
//    TaskHandle_t xTaskToNotify = pxCond->xTaskToNotify;
//    pxCond->xTaskToNotify = NULL;
//    if(xTaskToNotify == NULL) {
//        /* No task waiting to be notified. Send this signal for later */
//        pxCond->xSyncSignal = pdTRUE;
//    }
//    /* If a task is already waiting, there is no need to set the sync signal */
//    _exit_critical_isr(mask);

//    if(xTaskToNotify != NULL) {
//        /* There is a task waiting. Send a notification to it */
//        vTaskNotifyGiveFromISR(xTaskToNotify, &xHigherPriorityTaskWoken);
//        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
//    }
//    /* If there was no task waiting to be notified, we sent a signal for it to see later. */
//#else
//    /* Enter critical section to prevent preemption. */
//    uint32_t mask = _enter_critical_isr();

//    pxCond->xSyncSignal = pdTRUE;
//    BaseType_t xAnyHigherPriorityTaskWoken = pdFALSE;

//    /* Unblock all. */
//    for(uint32_t i = 0; i < pxCond->ulWaitingThreads; i++) {
//        xSemaphoreGiveFromISR(pxCond->xCondWaitSemaphore, &xAnyHigherPriorityTaskWoken);
//        xHigherPriorityTaskWoken |= xAnyHigherPriorityTaskWoken;
//    }

//    _exit_critical_isr(mask);
//    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
//#endif

    return LV_RESULT_OK;
}


void lv_threadx_task_switch_in(const char * name)
{
  
}

void lv_threadx_task_switch_out(void)
{
  
}

#if LV_OS_IDLE_PERCENT_CUSTOM
uint32_t lv_os_get_idle_percent(void)
{
    if(globals->freertos_non_idle_time_sum + globals->freertos_idle_time_sum == 0) {
        LV_LOG_WARN("Not enough time elapsed to provide idle percentage");
        return 0;
    }

    uint32_t pct = (globals->freertos_idle_time_sum * 100) / (globals->freertos_idle_time_sum +
                                                              globals->freertos_non_idle_time_sum);

    globals->freertos_non_idle_time_sum = 0;
    globals->freertos_idle_time_sum = 0;

    return pct;
}
#endif

void lv_sleep_ms(uint32_t ms)
{
	
    tx_thread_sleep(ms);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void prvRunThread(ULONG  pxArg)
{
    lv_thread_t * pxThread = (lv_thread_t *)pxArg;

    /* Run the thread routine. */
    pxThread->pvStartRoutine((void *)pxThread->pTaskArg);

    tx_thread_delete(NULL);
}

static void prvMutexInit(lv_mutex_t * pxMutex)
{
	UINT status;
    status = tx_mutex_create(
        &pxMutex->xMutex,
        "lvgl_mutex",     /* mutex名字，调试用 */
        TX_INHERIT /* 开启递归，等价xSemaphoreCreateRecursiveMutex */
    );

    /* Ensure that the FreeRTOS mutex was successfully created. */
if(status != TX_SUCCESS)
    {
        LV_LOG_ERROR("tx_mutex_create failed! status=%d", status);
			pxMutex->xIsInitialized = TX_FALSE;
        return;
    }

    /* Mutex successfully created. */
    pxMutex->xIsInitialized = TX_TRUE;
}

static void prvCheckMutexInit(lv_mutex_t * pxMutex)
{
    /* Check if the mutex needs to be initialized. */
    if(pxMutex->xIsInitialized == TX_FALSE) {
        /* Mutex initialization must be in a critical section to prevent two threads
         * from initializing it at the same time. */
        _enter_critical();

        /* Check again that the mutex is still uninitialized, i.e. it wasn't
         * initialized while this function was waiting to enter the critical
         * section. */
        if(pxMutex->xIsInitialized == TX_FALSE) {
            prvMutexInit(pxMutex);
        }

        /* Exit the critical section. */
        _exit_critical();
    }
}

static void prvCondInit(lv_thread_sync_t * pxCond)
{
	UINT status;
	
    pxCond->xIsInitialized = TX_TRUE;
    pxCond->xSyncSignal = TX_FALSE;

#if LV_USE_FREERTOS_TASK_NOTIFY
    pxCond->xTaskToNotify = NULL;
#else
    status = tx_semaphore_create(&pxCond->xCondWaitSemaphore, "lv_cond_sem", 0U);

    /* Ensure that the FreeRTOS semaphore was successfully created. */
    if(status != TX_SUCCESS)
    {
        LV_LOG_ERROR("tx_semaphore_create failed! status=%d", status);
        return;
    }

    /* 创建配套互斥锁 */
    status = tx_mutex_create(&pxCond->xSyncMutex, "lv_cond_mtx", TX_INHERIT);

    /* Ensure that the FreeRTOS mutex was successfully created. */
    if(status != TX_SUCCESS)
    {
        LV_LOG_ERROR("tx_mutex_create failed! status=%d", status);
        /* 回滚：信号量已经创建成功，必须销毁 */
        tx_semaphore_delete(&pxCond->xCondWaitSemaphore);
        return;
    }

    /* Condition variable successfully created. */
    pxCond->ulWaitingThreads = 0;
#endif
}

static void prvCheckCondInit(lv_thread_sync_t * pxCond)
{
    /* Check if the condition variable needs to be initialized. */
    if(pxCond->xIsInitialized == TX_FALSE) {
        /* Cond initialization must be in a critical section to prevent two
         * threads from initializing it at the same time. */
        _enter_critical();

        /* Check again that the condition is still uninitialized, i.e. it wasn't
         * initialized while this function was waiting to enter the critical
         * section. */
        if(pxCond->xIsInitialized == TX_FALSE) {
            prvCondInit(pxCond);
        }

        /* Exit the critical section. */
        _exit_critical();
    }
}

static void prvCheckCondInitIsr(lv_thread_sync_t * pxCond)
{
    /* Check if the condition variable needs to be initialized. */
    if(pxCond->xIsInitialized == TX_FALSE) {
        /* Cond initialization must be in a critical section to prevent two
         * threads from initializing it at the same time. */
        _enter_critical_isr();

        /* Check again that the condition is still uninitialized, i.e. it wasn't
         * initialized while this function was waiting to enter the critical
         * section. */
        if(pxCond->xIsInitialized == TX_FALSE) {
            prvCondInit(pxCond);
        }

        /* Exit the critical section. */
        _exit_critical_isr(mask);
    }
}

#if !LV_USE_FREERTOS_TASK_NOTIFY
static void prvTestAndDecrement(lv_thread_sync_t * pxCond,
                                uint32_t ulLocalWaitingThreads)
{
    /* Test local copy of threads waiting is larger than zero. */
    while(ulLocalWaitingThreads > 0) {
        /* Atomically check whether the copy in memory has changed.
         * If not, decrease the copy of threads waiting in memory. */
        if(ATOMIC_COMPARE_AND_SWAP_SUCCESS == Atomic_CompareAndSwap_u32(
               &pxCond->ulWaitingThreads,
               ulLocalWaitingThreads - 1,
               ulLocalWaitingThreads)) {
            /* Signal one succeeded. Break. */
            break;
        }

        /* Local copy may be out dated. Reload from memory and retry. */
        ulLocalWaitingThreads = pxCond->ulWaitingThreads;
    }
}
#endif

static inline uint32_t Atomic_CompareAndSwap_u32(volatile uint32_t * pDestination,
                                          uint32_t newValue,
                                          uint32_t comparand )
{
    uint32_t swapped = 0;
    _enter_critical();
    if( *pDestination == comparand )
    {
        *pDestination = newValue;
        swapped = 1;
    }
    _exit_critical();
    return swapped;
}

static uint32_t Atomic_Increment_u32(volatile uint32_t * pValue )
{
    uint32_t oldVal;
    TX_INTERRUPT_SAVE_AREA
    TX_DISABLE;
    oldVal = *pValue;
    *pValue = oldVal + 1;
    TX_RESTORE;
    return oldVal;
}

#endif /*LV_USE_OS == LV_OS_FREERTOS*/
