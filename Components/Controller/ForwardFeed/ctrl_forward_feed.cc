//
// Created by fish on 2025/1/3.
//

#include "ctrl_forward_feed.h"

using namespace Controller;

double ForwardFeed::update(double current, double target) {
	return current + target;
}
