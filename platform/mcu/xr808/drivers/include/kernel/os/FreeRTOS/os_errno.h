/**
 * @file os_errno.h
 * @author XRADIO IOT WLAN Team
 */

/*
 * Copyright (C) 2017 XRADIO TECHNOLOGY CO., LTD. All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *    1. Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *    2. Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the
 *       distribution.
 *    3. Neither the name of XRADIO TECHNOLOGY CO., LTD. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _KERNEL_OS_FREERTOS_OS_ERRNO_H_
#define _KERNEL_OS_FREERTOS_OS_ERRNO_H_

#include "kernel/os/FreeRTOS/os_common.h"
#include "task.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Thread safe errno handling for FreeRTOS
 */

//#if (configNUM_THREAD_LOCAL_STORAGE_POINTERS != 0)

//#define OS_ERRNO_LOCATION_IDX		0

/**
 * @brief Get error number of the current thread
 * @return Error number of the current thread
 */
static __always_inline int OS_GetErrno(void)
{
	//return (int)pvTaskGetThreadLocalStoragePointer(NULL, OS_ERRNO_LOCATION_IDX);
	return 0;
}

/**
 * @brief Set error number of the current thread
 * @param[in] Error number to be set
 * @return None
 */
static __always_inline void OS_SetErrno(int err)
{
	//vTaskSetThreadLocalStoragePointer(NULL, OS_ERRNO_LOCATION_IDX, (void *)err);
}

//#endif /* configNUM_THREAD_LOCAL_STORAGE_POINTERS */

#ifdef __cplusplus
}
#endif

#endif /* _KERNEL_OS_FREERTOS_OS_ERRNO_H_ */
