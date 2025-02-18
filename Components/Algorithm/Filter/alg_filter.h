//
// Created by fish on 2025/1/24.
//

#pragma once

#include <vector>

namespace Algorithm {
// 普通的滑动窗口均值滤波
class AverageFilter {
public:
    explicit AverageFilter(int size);
    double update(double val);
    void reset();
private:
    int size_, ptr_;
    double sum_;
    std::vector <double> data_;
};

// 一阶数字低通滤波器
class LowPassFilter {
public:
    /*!
     * @brief 初始化一阶数字低通滤波器
     * @param cut_freq 截止频率
     */
    explicit LowPassFilter(double cut_freq);
    /*!
     * @brief 更新滤波器
     * @param sample 采样值
     * @param dt 采样间隔
     * @return 滤波结果
     */
    double update(double sample, double dt);
    /*!
     * @brief 重置滤波器
     * @param sample 采样值
     */
    void reset(double sample);
private:
    double cut_freq_, last_out_;
};

// 二阶巴特沃斯低通滤波器
class LowPassFilter2p {
public:
    /*!
     * @brief 初始化二阶巴特沃斯低通滤波器
     * @param sample_freq 采样频率
     * @param cut_freq 截止频率
     */
    LowPassFilter2p(double sample_freq, double cut_freq);
    /*!
     * @brief 更新滤波器
     * @param sample 采样值
     * @return 滤波结果
     */
    double update(double sample);
    /*!
     * 重置滤波器
     * @param sample 采样值
     * @return 滤波结果
     */
    double reset(double sample);

private:
    double cut_freq_;
    double a1_, a2_, b0_, b1_, b2_;
    double delay_element_1_, delay_element_2_;
};
}