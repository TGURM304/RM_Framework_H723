//
// Created by fish on 2024/11/17.
//

#include "app_gimbal.h"

#include "app_sys.h"
#include "sys_task.h"

#ifdef COMPILE_GIMBAL

// 静态任务，在 CubeMX 中配置
void app_gimbal_task(void *args) {
    // Wait for system init.
    while(!app_sys_ready()) OS::Task::SleepMilliseconds(10);

    while(true) {
        OS::Task::SleepMilliseconds(1);
    }
}

void app_gimbal_init() {

}

#endif