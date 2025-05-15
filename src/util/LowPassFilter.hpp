#pragma once

#include <stdint.h>
#include <array>

template<typename T, size_t S>
class LowPassFilter {
public:
	LowPassFilter() {
		for (int i = 0; i < S; i++)
			m_samples[i] = 0; // ?
	}

	T getValue() { return m_value;  }

	void filterValue(T newValue) {
		m_value = calculateWeight() / m_samples.size();

		m_index++;
		if (m_samples.size() == m_index)
			m_index = 0;

		m_samples[m_index] = newValue;
	}
private:
	T calculateWeight() {
		T result {};
		for (const auto& sample : m_samples)
			result += sample;
		return result;
	}

	int32_t m_index { 0 };
	T m_value { };
	std::array<T, S> m_samples { };
};