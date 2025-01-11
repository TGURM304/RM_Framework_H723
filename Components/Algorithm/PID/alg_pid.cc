//
// Created by fish on 2024/9/8.
//

#include "alg_pid.h"
#include <algorithm>

#define Limit(input, max) { input = input > max ? max : (input < -max ? -max : input); }

using namespace Algorithm;

void PID::clear() {
    err[0] = err[1] = err[2] = measure[0] = measure[1] = measure[2] = p_out = i_out = d_out = out = 0;
}

double PID::update(double current, double target) {
    err[2] = err[1], err[1] = err[0], err[0] = target - current;
    measure[2] = measure[1], measure[1] = measure[0], measure[0] = current;

    double p_cur = err[0], i_cur = err[0], d_cur = err[0] - err[1];

    // TODO: 这些优化的效果还没测试。2024/12/19
    // PID 优化，ref: ref: https://www.cnblogs.com/WangHongxi/p/12405456.html
    // 梯形积分，取相邻误差均值做积分
    if(option_ & TRAPEZOID_INTEGRAL) {
        i_cur = (err[0] + err[1]) / 2;
    }
    // 变速积分
    if(option_ & CHANGING_INTEGRAL_RATE) {
        // 判断当前误差与积分是否同向
        if(err[0] * i_out > 0) {
            if(std::abs(err[0]) <= scalar_b) {
                // 完整积分 do nothing
                ;
            } else if(std::abs(err[0]) <= scalar_a + scalar_b) {
                // 线性函数变速积分
                i_cur *= (scalar_a + scalar_b - std::abs(err[0])) / scalar_a;
            } else {
                // 终止积分
                i_cur = 0;
            }
        }
    }
    // 积分抗饱和，若 PID 输出已到限幅，则停止积分
    if(option_ & INTEGRAL_LIMIT) {
        if(std::abs(out) == out_limit_) {
            if(err[0] * i_out > 0) {
                i_cur = 0;
            }
        }
    }
    // 微分先行，忽略误差的微分，只保留测量值的微分
    if(option_ & DERIVATIVE_ON_MEASUREMENT) {
        d_cur = measure[1] - measure[0];
    }

    p_out =  Kp_ * p_cur;
    i_out += Ki_ * i_cur;
    d_out =  Kd_ * d_cur;
    Limit(i_out, iout_limit_);
    out = p_out + i_out + d_out;
    Limit(out, out_limit_);
    return out;
}