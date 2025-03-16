//
// Created by fish on 2025/1/1.
//

#include "app_ins.h"

#include "ctrl_pid.h"
#include "alg_quaternion_ekf.h"
#include "app_conf.h"
#include "app_terminal.h"
#include "bsp_adc.h"
#include "bsp_buzzer.h"
#include "bsp_def.h"
#include "bsp_uart.h"
#include "bsp_flash.h"
#include "bsp_time.h"
#include "sys_task.h"
#include "tim.h"

#define GYRO_CORRECT_SAMPLE_COUNT 10000
#define GYRO_CORRECT_SAMPLE_ALTERNATE_COUNT 1500

#define IMU_TEMPERATURE_CONTROL_TIMER &htim3
#define IMU_TEMPERATURE_CONTROL_CHANNEL TIM_CHANNEL_4

static bool inited_ = false;
static app_ins_data_t data;

static struct __attribute__((__packed__)) {
    int key        = 0;
    double data[3] = { 0, 0, 0 };
} ins_flash_data;

Controller::PID temp_pid;

int ins_flag = 0;
double gyro_correct[3];

void app_ins_init() {
    bsp_imu_init();
    BSP_ASSERT(bsp_adc_vbus() > 0);
    if(bsp_adc_vbus() > 22) {
        temp_pid.set_para(100, 1, 0, 10000, 500);
    } else if(bsp_adc_vbus() > 10) {
        temp_pid.set_para(500, 2, 1, 10000, 1000);
    } else {
        temp_pid.set_para(12500, 10, 1, 25000, 10000);
    }
    HAL_TIM_PWM_Start(IMU_TEMPERATURE_CONTROL_TIMER, IMU_TEMPERATURE_CONTROL_CHANNEL);
    IMU_QuaternionEKF_Init(10, 0.001, 10000000, 1, 0.001f, 0);

    bsp_flash_read("ins", &ins_flash_data, sizeof(ins_flash_data));
    if(ins_flash_data.key == SYS_FLASH_KEY) {
        memcpy(gyro_correct, ins_flash_data.data, 3 * sizeof(double));
        ins_flag = 2;
    } else {
        ;
        // TODO: 需要判断 flash 是否损坏，若 flash 损坏则进入校准流程。
        // TODO: 现在获取不到 flash 数据时会直接进入校准。
    }

    app_terminal_register_cmd("ins", "ins commands", [](const auto &args) -> bool {
        auto running = app_terminal_running_flag();
        if(args.size() == 1) {
            TERMINAL_INFO("usage: ins cali/watch/test/config\r\n");
            return true;
        }
        if(args[1] == "cali") {
            ins_flag = 1;
            TERMINAL_INFO("正在校准陀螺仪，校准过程中请勿移动陀螺仪...\r\n");
            int count       = GYRO_CORRECT_SAMPLE_COUNT;
            gyro_correct[0] = gyro_correct[1] = gyro_correct[2] = 0;
            while(count-- and *running) {
                gyro_correct[0] += data.raw.gyro[0];
                gyro_correct[1] += data.raw.gyro[1];
                gyro_correct[2] += data.raw.gyro[2];
                TERMINAL_SEND(TERMINAL_CLEAR_LINE, sizeof TERMINAL_CLEAR_LINE);
                TERMINAL_INFO("[%d] %lf, %lf, %lf",
                              GYRO_CORRECT_SAMPLE_COUNT - count,
                              gyro_correct[0],
                              gyro_correct[1],
                              gyro_correct[2]);
                OS::Task::SleepMilliseconds(1);
            }
            TERMINAL_INFO("\r\n");
            if(!*running) return false;
            gyro_correct[0] /= GYRO_CORRECT_SAMPLE_COUNT;
            gyro_correct[1] /= GYRO_CORRECT_SAMPLE_COUNT;
            gyro_correct[2] /= GYRO_CORRECT_SAMPLE_COUNT;
            TERMINAL_INFO("正在保存数据: %lf, %lf, %lf\r\n", gyro_correct[0], gyro_correct[1], gyro_correct[2]);
            ins_flash_data.key = SYS_FLASH_KEY;
            memcpy(ins_flash_data.data, gyro_correct, 3 * sizeof(double));
            if(bsp_flash_write("ins", &ins_flash_data, sizeof(ins_flash_data))) {
                ins_flag = 2;
                TERMINAL_INFO("校准完成\r\n");
            } else {
                TERMINAL_ERROR("Flash 写入失败\r\n");
            }
            return true;
        }
        if(args[1] == "watch") {
            while(*running) {
                TERMINAL_INFO("%f,%f,%f,%f\r\n", data.roll, data.pitch, data.yaw, data.raw.temp);
                OS::Task::SleepMilliseconds(1);
            }
            return true;
        }
        if(args[1] == "test") {
            if(ins_flag != 2) {
                TERMINAL_ERROR("陀螺仪未校准\r\n");
                return false;
            }
            TERMINAL_INFO("正在测试陀螺仪，测试过程中请勿移动陀螺仪... (5s * 5)\r\n");
            double sum = 0;
            for(int i = 1; i <= 5; i++) {
                double st = data.yaw;
                OS::Task::SleepSeconds(5);
                double ed = data.yaw;
                TERMINAL_INFO("test #%d = %lf deg/min\r\n", i, (ed - st) * 12);
                sum += (ed - st) * 12;
            }
            TERMINAL_INFO("avg = %lf deg/min\r\n", sum / 5);
            return true;
        }
        if(args[1] == "config") {
            TERMINAL_INFO("%lf, %lf, %lf\r\n", gyro_correct[0], gyro_correct[1], gyro_correct[2]);
            return true;
        }
        return false;
    });

    inited_ = true;
}

void app_ins_task(void *args) {
    while(!inited_)
        OS::Task::SleepMilliseconds(10);

    int freq_cnt = 0;
    int count    = !ins_flag * GYRO_CORRECT_SAMPLE_ALTERNATE_COUNT;

    while(true) {
        bsp_imu_read(&data.raw);

        if(++freq_cnt == 100)
            __HAL_TIM_SetCompare(IMU_TEMPERATURE_CONTROL_TIMER,
                                 IMU_TEMPERATURE_CONTROL_CHANNEL,
                                 std::max(0.0f, temp_pid.update(data.raw.temp, 40))),
                freq_cnt = 0;

        if(ins_flag == 2) {
            double gyro[3] = { data.raw.gyro[0] - gyro_correct[0],
                               data.raw.gyro[1] - gyro_correct[1],
                               data.raw.gyro[2] - gyro_correct[2] };
            IMU_QuaternionEKF_Update(static_cast<float>(gyro[0]),
                                     static_cast<float>(gyro[1]),
                                     static_cast<float>(gyro[2]),
                                     data.raw.accel[0],
                                     data.raw.accel[1],
                                     data.raw.accel[2]);
            std::tie(data.roll, data.pitch, data.yaw) = IMU_QuaternionEKF_Data();
        }
        if(ins_flag == 0) {
            if(count) {
                gyro_correct[0] += data.raw.gyro[0];
                gyro_correct[1] += data.raw.gyro[1];
                gyro_correct[2] += data.raw.gyro[2];
                count--;
            } else {
                gyro_correct[0] /= GYRO_CORRECT_SAMPLE_ALTERNATE_COUNT;
                gyro_correct[1] /= GYRO_CORRECT_SAMPLE_ALTERNATE_COUNT;
                gyro_correct[2] /= GYRO_CORRECT_SAMPLE_ALTERNATE_COUNT;
                ins_flag = 2;
            }
        }

        OS::Task::SleepMilliseconds(1);
    }
}

uint8_t app_ins_status() {
    return ins_flag;
}

const app_ins_data_t *app_ins_data() {
    return &data;
}
