//
// Created by fish on 2024/12/18.
//

#include "app_chassis.h"

#include "app_sys.h"
#include "sys_task.h"

#ifdef COMPILE_CHASSIS

// 静态任务，在 CubeMX 中配置
/**
 * @brief  底盘的task函数，在这个函数内部实现底盘的控制逻辑
 */
void app_chassis_task(void *args) {
	// Wait for system init.
	while(!app_sys_ready()) OS::Task::SleepMilliseconds(10);

	while(true) {
		OS::Task::SleepMilliseconds(1);
	}
}

/**
 * @brief  系统级初始化函数，仅执行一次的任务函数可放置于此
 */
void app_chassis_init() {

}

#endif