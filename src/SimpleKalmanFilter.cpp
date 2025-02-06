/**
 * @copyright   Copyright (C) 2019 AZLAB, Inc. All rights reserved.
 * @author      Shinichiro Nakamura
 * @date        6th Nov. 2019
 */

#include <SimpleKalmanFilter.hpp>

SimpleKalmanFilter::SimpleKalmanFilter() : P(1), X(0)
{
}

SimpleKalmanFilter::~SimpleKalmanFilter()
{
}

void SimpleKalmanFilter::calc()
{
	K = (P + Q) / (P + Q + R);
	P = R * (P + Q) / (R + P + Q);
}

float SimpleKalmanFilter::update(const float measurement)
{
	calc();
	float result = X + (measurement - X) * K;
	X = result;
	return result;
}

float SimpleKalmanFilterXYZ::updateX(const float measurement)
{
	return x.update(measurement);
}

float SimpleKalmanFilterXYZ::updateY(const float measurement)
{
	return y.update(measurement);
}

float SimpleKalmanFilterXYZ::updateZ(const float measurement)
{
	return z.update(measurement);
}

