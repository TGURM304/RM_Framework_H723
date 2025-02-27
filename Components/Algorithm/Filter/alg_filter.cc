//
// Created by fish on 2025/1/24.
//

#include "alg_filter.h"

#include <cmath>

using namespace Algorithm;

AverageFilter::AverageFilter(int size) : size_(size), ptr_(0), sum_(0) {
    data_.resize(size);
}

double AverageFilter::update(double val) {
    sum_ += val - data_[ptr_];
    data_[ptr_] = val;
    ptr_ = (ptr_ + 1) % size_;
    return sum_ / size_;
}

void AverageFilter::reset() {
    ptr_ = 0, sum_ = data_[0] = 0;
}

/*
 *  参考 XRobot: https://github.com/xrobot-org/XRobot/blob/master/src/component/comp_filter.cpp
 */

LowPassFilter::LowPassFilter(double cut_freq) : cut_freq_(cut_freq), last_out_(0) {
    ;
}

double LowPassFilter::update(double sample, double dt) {
    auto k = 2 * M_PI * cut_freq_ * dt;
    k = k / (1 + k);
    return last_out_ = k * sample + (1 - k) * last_out_;
}

void LowPassFilter::reset(double sample) {
    last_out_ = sample;
}

LowPassFilter2p::LowPassFilter2p(double sample_freq, double cut_freq) : cut_freq_(cut_freq), delay_element_1_(0), delay_element_2_(0) {
    if (cut_freq_ <= 0.0f) {
        /* no filtering */
        b0_ = 1;
        b1_ = 0;
        b2_ = 0;

        a1_ = 0;
        a2_ = 0;
        return;
    }
    auto FR = sample_freq / cut_freq_;
    auto OHM = std::tan(M_PI / FR);
    auto C = 1 + M_SQRT2 * OHM + OHM * OHM;

    b0_ = OHM * OHM / C;
    b1_ = 2 * b0_;
    b2_ = b0_;

    a1_ = 2 * (OHM * OHM - 1) / C;
    a2_ = (1 - M_SQRT2 * OHM + OHM * OHM) / C;
}

double LowPassFilter2p::update(double sample) {
    /* do the filtering */
    auto delay_element_0 = sample - delay_element_1_ * a1_ - delay_element_2_ * a2_;

    if (std::isinf(delay_element_0)) {
        /* don't allow bad values to propagate via the filter */
        delay_element_0 = sample;
    }

    auto out = delay_element_0 * b0_ + delay_element_1_ * b1_ + delay_element_2_ * b2_;

    delay_element_2_ = delay_element_1_;
    delay_element_1_ = delay_element_0;

    /* return the value. Should be no need to check limits */
    return out;
}

double LowPassFilter2p::reset(double sample) {
    auto dval = sample / (b0_ + b1_ + b2_);
    if (std::isfinite(dval)) {
        delay_element_1_ = dval;
        delay_element_2_ = dval;
    } else {
        delay_element_1_ = sample;
        delay_element_2_ = sample;
    }
    return update(sample);
}


