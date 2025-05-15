/**
 * @copyright   Copyright (C) 2019 AZLAB, Inc. All rights reserved.
 * @author      Shinichiro Nakamura
 * @date        6th Nov. 2019
 */

#ifndef SIMPLEKALMANFILTER_H
#define SIMPLEKALMANFILTER_H

class SimpleKalmanFilter {
public:
	SimpleKalmanFilter();
	~SimpleKalmanFilter();
	float GetQ() { return Q; };
	float GetR() { return R; };
	void SetQ(float q) { Q = q; };
	void SetR(float r) { R = r; };
	float update(const float measurement);
private:
	float Q = 0.000150;
	float R = 0.01;
	float P;
	float X;
	float K;
	void calc();
};

class SimpleKalmanFilterXYZ {
public:
	SimpleKalmanFilterXYZ() {}
	~SimpleKalmanFilterXYZ() {}
	float GetQ() { return x.GetQ(); };
	float GetR() { return x.GetR(); };
	void SetQ(float q) {
		x.SetQ(q);
		y.SetQ(q);
		z.SetQ(q);
	};
	void SetR(float r) {
		x.SetR(r);
		y.SetR(r);
		z.SetR(r);
	};
	float updateX(const float measurement);
	float updateY(const float measurement);
	float updateZ(const float measurement);
private:
	SimpleKalmanFilter x;
	SimpleKalmanFilter y;
	SimpleKalmanFilter z;
};

#endif

