//
// Created by fish on 2024/9/8.
//

#pragma once

/*
 * Algorithm - PID
 * Usage:
 *    auto pid = Algorithm::PID(option, Kp, Ki, Kd, out_limit, iout_limit);
 *    auto out = pid.update(current, target);
 */

namespace Algorithm {
    class PID {
      public:
        typedef enum {
            NONE = 0,
            TRAPEZOID_INTEGRAL          = 0b00000001,    // 梯形积分
            DERIVATIVE_ON_MEASUREMENT   = 0b00000010,    // 微分先行
            CHANGING_INTEGRAL_RATE      = 0b00000100,    // 变速积分（需要设置参数 scalar_a、scalar_b）
            INTEGRAL_LIMIT              = 0b00001000,    // 积分抗饱和
        } pid_option_e;
        typedef struct {
            int option;
            double Kp, Ki, Kd, out_limit, iout_limit;
        } pid_param_t;
        PID() : Kp_(0), Ki_(0), Kd_(0), out_limit_(0), iout_limit_(0), option_(NONE) {}
        explicit PID(int option) : Kp_(0), Ki_(0), Kd_(0), out_limit_(0), iout_limit_(0), option_(option) {}
        PID(int option, double Kp, double Ki, double Kd, double out_limit, double iout_limit) :
            Kp_(Kp), Ki_(Ki), Kd_(Kd), out_limit_(out_limit), iout_limit_(iout_limit), option_(option) {}
        void clear();
        double update(double current, double target);
        void set_para(double Kp, double Ki, double Kd) {
            Kp_ = Kp; Ki_ = Ki; Kd_ = Kd; this->clear();
        }
        void set_para(double Kp, double Ki, double Kd, double out_limit, double iout_limit) {
            Kp_ = Kp, Ki_ = Ki, Kd_ = Kd, out_limit_ = out_limit, iout_limit_ = iout_limit; this->clear();
        }
        void set_para(const pid_param_t &param) {
            Kp_ = param.Kp, Ki_ = param.Ki, Kd_ = param.Kd, out_limit_ = param.out_limit, iout_limit_ = param.iout_limit, option_ = param.option;
            this->clear();
        }

        // 变速积分参数
        double scalar_a = 0, scalar_b = 0;

      private:
        int option_;
        double Kp_, Ki_, Kd_, out_limit_, iout_limit_;
        double p_out = 0, i_out = 0, d_out = 0, out = 0, err[3] = { 0 }, measure[3] = { 0 };
    };
}