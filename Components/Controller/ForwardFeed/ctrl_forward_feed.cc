//
// Created by fish on 2025/1/3.
//

#include "ctrl_forward_feed.h"

using namespace Controller;

float ForwardFeed::update(float current, float target) {
	return current + target;
}
