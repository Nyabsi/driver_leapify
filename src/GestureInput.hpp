#pragma once

#include <stdint.h>
#include <vector>
#include <map>

#include <LeapConnection.hpp> // for LeapHand

enum FingerType : uint8_t {
	FINGER_THUMB = 0, // reserved for internal-use, DO NOT use.
	FINGER_INDEX = 1,
	FINGER_MIDDLE = 2,
	FINGER_RING = 3,
	FINGER_PINKY = 4
};

enum GestureType : uint8_t {
	GESTURE_PINCH = 1,
	GESTURE_DUAL_PINCH = 2,
	GESTURE_TIMED_PINCH = 3,
	GESTURE_THUMB_STICK = 4,
	GESTURE_LOCKED_THUMB_STICK = 5,
	GESTURE_SWIPE_VERTICAL = 6
};

struct GestureVector3f_t {
	float x{ };
	float y{ };
	float z{ };
};

struct GestureState_t {
	bool held{ };
	bool click{ };
	bool up{  };
	bool down{ };
	float x{ };
	float y{ };
};

struct GestureConfiguration_t {
	uint64_t delayInMs{ 3000 };
};

struct GestureBinding_t {
	FingerType id{ };
	GestureType type{ };
	GestureState_t state{ };
	GestureConfiguration_t cfg{ };
};

struct GestureInternalState_t {
	bool isBeingHeld{ false };
	bool wasClicked{ false };
	bool isLocked{ false };
	uint32_t clickCounter{ };
	int64_t clickStart{ };
	GestureVector3f_t movementOrigin{ };
};

class GestureInput {
public:
	void setPinchMinimumDistance(float distance) { m_minimumPinchDistance = distance; }
	void setMovementDistanceThreshold(float distance) { m_distanceThreshold = distance; }

	void addBinding(FingerType finger, GestureType inputType, GestureConfiguration_t config);
	void Update(LeapHand hand);
	GestureState_t GetState(FingerType finger);
	float getNormalizedFingerCurl(LeapHand hand, uint8_t index);
	float getNormalizedCombinedCurl(LeapHand hand);
private:
	bool isPinchingThumb(GestureVector3f_t thumb, GestureVector3f_t other) const;
	GestureVector3f_t getNormalizedPosition(GestureVector3f_t origin, GestureVector3f_t other);

	float m_minimumPinchDistance{ 8.0f };
	float m_distanceThreshold{ 50.0f };
	std::vector<GestureBinding_t> m_bindings{ };
	std::map<FingerType, GestureInternalState_t> m_states{ };
};