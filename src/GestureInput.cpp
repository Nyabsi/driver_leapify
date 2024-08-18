#include "GestureInput.hpp"
#include <LeapC.h>
#include <cmath>
#include <Windows.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

void GestureInput::addBinding(FingerType finger, GestureType inputType, GestureConfiguration_t config)
{
	GestureBinding_t binding{ };
	binding.id = finger;
	binding.type = inputType;
	binding.cfg = config;

	m_bindings.push_back(binding);
}

void GestureInput::Update(LeapHand hand)
{
	for (auto& binding : m_bindings)
	{
		auto& state = m_states[binding.id];

		GestureVector3f_t target{ };
		switch (binding.id)
		{
		case FINGER_INDEX:
		{
			target = { hand.index.distal.next_joint.x, hand.index.distal.next_joint.y, hand.index.distal.next_joint.z };
			break;
		}

		case FINGER_MIDDLE:
		{
			target = { hand.middle.distal.next_joint.x, hand.middle.distal.next_joint.y, hand.middle.distal.next_joint.z };
			break;
		}

		case FINGER_RING:
		{
			target = { hand.ring.distal.next_joint.x, hand.ring.distal.next_joint.y, hand.ring.distal.next_joint.z };
			break;
		}

		case FINGER_PINKY:
		{
			target = { hand.pinky.distal.next_joint.x, hand.pinky.distal.next_joint.y, hand.pinky.distal.next_joint.z };
			break;
		}
		}

		state.isBeingHeld = isPinchingThumb({ hand.thumb.distal.next_joint.x, hand.thumb.distal.next_joint.y, hand.thumb.distal.next_joint.z }, target);

		if (!state.wasClicked && state.isBeingHeld)
		{
			state.movementOrigin = { target.x, target.y, target.z };
			state.clickStart = LeapGetNow() * 0.001f;
			state.wasClicked = true;
			state.clickCounter++;
		}

		switch (binding.type)
		{
		case GESTURE_PINCH:
		{
			if (state.isBeingHeld)
			{
				binding.state.click = true;
			}
			else
			{
				binding.state.click = false;
				state.wasClicked = false;
			}
			break;
		}

		case GESTURE_TIMED_PINCH:
		{
			if (state.isBeingHeld)
			{
				if ((state.clickStart + binding.cfg.delayInMs) < LeapGetNow() * 0.001f)
				{
					binding.state.click = true;
				}
			}
			else
			{
				binding.state.click = false;
				state.wasClicked = false;
			}
			break;
		}

		case GESTURE_DUAL_PINCH:
		{
			if (state.isBeingHeld)
			{
				if (state.clickCounter >= 2)
				{
					binding.state.click = true;
					state.clickCounter = 0;
				}
			}
			else
			{
				binding.state.click = false;
				state.wasClicked = false;
			}
			break;
		}

		case GESTURE_THUMB_STICK:
		{
			if (state.isBeingHeld)
			{
				auto position = getNormalizedPosition(state.movementOrigin, target);
				binding.state.x = position.x;
				binding.state.y = position.y;
				binding.state.held = true;
			}
			else {
				state.wasClicked = false;
				binding.state.x = 0.0f;
				binding.state.y = 0.0f;
				binding.state.held = false;
			}
			break;
		}

		case GESTURE_LOCKED_THUMB_STICK:
		{
			if (state.isBeingHeld)
			{
				if (state.clickCounter >= 2)
				{
					state.isLocked = !state.isLocked;
					state.clickCounter = 0;
				}
			}
			else {
				state.wasClicked = false;
			}

			if (state.isLocked)
			{
				auto position = getNormalizedPosition(state.movementOrigin, target);
				binding.state.x = position.x;
				binding.state.y = position.y;
				binding.state.held = true;
			}
			else {
				binding.state.x = 0.0f;
				binding.state.y = 0.0f;
				binding.state.held = false;
			}

			break;
		}

		case GESTURE_SWIPE_VERTICAL:
		{
			if (state.isBeingHeld)
			{
				auto position = getNormalizedPosition(state.movementOrigin, target);

				if (position.z > 0.75f)
					binding.state.up = true;
				else
					binding.state.up = false;

				if (position.z < -0.75f)
					binding.state.down = true;
				else
					binding.state.down = false;

				binding.state.click = true;
			}
			else {
				binding.state.up = false;
				binding.state.down = false;
				binding.state.click = false;
				state.wasClicked = false;
			}

			break;
		}
		}
	}
}

GestureState_t GestureInput::GetState(FingerType finger)
{
	auto binding = std::find_if(m_bindings.begin(), m_bindings.end(), [&](GestureBinding_t binding) {
		return binding.id == finger;
		});

	return (*binding).state;
}

bool GestureInput::isPinchingThumb(GestureVector3f_t thumb, GestureVector3f_t other) const
{
	float lx = thumb.x - other.x;
	float ly = thumb.y - other.y;
	float lz = thumb.z - other.z;

	return sqrt(lx * lx + ly * ly + lz * lz) < m_minimumPinchDistance;
}

GestureVector3f_t GestureInput::getNormalizedPosition(GestureVector3f_t origin, GestureVector3f_t other)
{
	GestureVector3f_t result{};

	float lx = ((origin.x - other.x) / m_distanceThreshold);
	float ly = ((origin.y - other.y) / m_distanceThreshold);
	float lz = ((origin.z - other.z) / m_distanceThreshold);

	result.x = (lx < -1) ? -1 : (lx > 1) ? 1 : lx;
	result.z = (ly < -1) ? -1 : (ly > 1) ? 1 : ly;
	result.y = (lz < -1) ? -1 : (lz > 1) ? 1 : lz;

	return result;
}

float GestureInput::getNormalizedFingerCurl(LeapHand hand, uint8_t index)
{
	float result = 0.0f;

	float llx{}, lly{}, llz{};
	for (size_t i = 0; i < 4; i++)
	{
		const auto& bone = hand.digits[index].bones[i];

		float lx = bone.next_joint.x - bone.prev_joint.x;
		float ly = bone.next_joint.y - bone.prev_joint.y;
		float lz = bone.next_joint.z - bone.prev_joint.z;

		float magnitude = std::sqrt(lx * lx + ly * ly + lz * lz);

		if (magnitude > 0.0f) {
			lx /= magnitude;
			ly /= magnitude;
			lz /= magnitude;
		}

		float dot = (lx * llx + ly * lly + lz * llz);
		dot = std::fmax(0.0f, std::fmin(1.0f, dot));
		result += std::acos(dot);

		llx = lx;
		lly = ly;
		llz = lz;
	}

	const float mapped = (result - M_PI * 0.50f) / (M_PI - M_PI * 0.25f);
	result = (mapped < -1) ? 0 : (mapped > 1) ? 1 : mapped;

	return result;
}

float GestureInput::getNormalizedCombinedCurl(LeapHand hand)
{
	float result = 0.0f;

	float llx{}, lly{}, llz{};
	for (size_t i = 2; i < 4; i++)
	{
		float fingerResult = 0.0f;

		for (size_t j = 0; j < 4; j++)
		{
			const auto& bone = hand.digits[i + 1].bones[j];

			float lx = bone.next_joint.x - bone.prev_joint.x;
			float ly = bone.next_joint.y - bone.prev_joint.y;
			float lz = bone.next_joint.z - bone.prev_joint.z;

			float magnitude = std::sqrt(lx * lx + ly * ly + lz * lz);

			if (magnitude > 0.0f) {
				lx /= magnitude;
				ly /= magnitude;
				lz /= magnitude;
			}

			float dot = (lx * llx + ly * lly + lz * llz);
			dot = std::fmax(0.0f, std::fmin(1.0f, dot));

			fingerResult += std::acos(dot);

			llx = lx;
			lly = ly;
			llz = lz;
		}

		result += fingerResult;
	}

	result /= 5.0f;

	const float mapped = (result - M_PI * 0.75f) / (M_PI - M_PI * 0.75f);
	result = (mapped < -1) ? 0 : (mapped > 1) ? 1 : mapped;

	return result;
}
