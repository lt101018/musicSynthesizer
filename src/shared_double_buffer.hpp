#ifndef SHARED_DOUBLE_BUFFER_HPP
#define SHARED_DOUBLE_BUFFER_HPP

#include <queue>
#include <mutex>
#include <condition_variable>
#include <iostream>

typedef double BufferType;  //need to be fixed

class double_buffer
{
public:

	void push(BufferType const& message);

	bool try_pop(BufferType& popped_value);

	void wait_and_pop(BufferType& popped_value);

	int getSize();

	void clear();

private:
	std::queue<BufferType> buffer1_, buffer2_;
	mutable std::mutex the_mutex;
	std::condition_variable the_condition_variable;

};

#endif