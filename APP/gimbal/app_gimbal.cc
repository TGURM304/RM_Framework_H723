//
// Created by fish on 2024/11/17.
//

#include "app_gimbal.h"

#include "app_sys.h"
#include "sys_task.h"

#ifdef COMPILE_GIMBAL

// 静态任务，在 CubeMX 中配置
/**
 * @brief  云台的task函数，在函数内部实现云台的控制逻辑
 */
void app_gimbal_task(void *args) {
    // Wait for system init.
    while(!app_sys_ready())
        OS::Task::SleepMilliseconds(10);

    while(true) {
        OS::Task::SleepMilliseconds(1);
    }
}

/**
 * @brief 系统级初始化函数，仅执行一次的任务函数可放置于此
 */
void app_gimbal_init() {

}

#endif
