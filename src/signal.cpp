#include "signal.hpp"

// TODO

Signal::Signal(unsigned int sampleRate): max(0)
{
	sampleRate_ = sampleRate;
}

unsigned int Signal::getSampleRate() {
	return sampleRate_;
}

void Signal::normalize() {
	double limit = std::numeric_limits<int16_t>::max();
	if(max > limit)
		for (auto it = signal_.begin(); it != signal_.end(); it++)
		{
			*it = round(*it * limit / max);
		}
	else
		for (auto it = signal_.begin(); it != signal_.end(); it++)
		{
			*it = round(*it);
		}
		//std::transform(signal_.begin(), signal_.end(), signal_.begin(), std::bind1st(std::multiplies<double>(), limit/max));
}

void Signal::push_back(double value) {
	signal_.push_back(value);
	if (abs(value) > max)
		max = abs(value);
}

std::size_t Signal::size() {
	return signal_.size();
}

double Signal::operator[](std::size_t index)
{
	return signal_[index];
}