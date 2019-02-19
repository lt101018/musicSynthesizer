#include "shared_double_buffer.hpp"

void double_buffer::push(BufferType const& message)
{
		std::unique_lock<std::mutex> lock(the_mutex);
		buffer1_.push(message);

		//std::cout << "Producer has added product " << message << std::endl;
		lock.unlock();
		the_condition_variable.notify_one();
}

bool double_buffer::try_pop(BufferType& popped_value)
{
	std::lock_guard<std::mutex> lock(the_mutex);
	if (buffer2_.empty() && !buffer1_.empty())
	{
		swap(buffer1_, buffer2_);
	}
	else if(buffer2_.empty() && buffer1_.empty())
		return false;
	popped_value = buffer2_.front();
	buffer2_.pop();
	//std::cout << "Consumer has taken product " << popped_value << std::endl;
	return true;
}

void double_buffer::wait_and_pop(BufferType& popped_value)
{
	std::unique_lock<std::mutex> lock(the_mutex);
	while (buffer1_.empty()&&buffer2_.empty())
	{
		the_condition_variable.wait(lock);
	}
	if (buffer2_.empty() && !buffer1_.empty())
	{
		swap(buffer1_, buffer2_);
	}

	popped_value = buffer2_.front();
	buffer2_.pop();
	//std::cout << "Consumer has taken product " << popped_value << std::endl;
}

int double_buffer::getSize() {
	std::lock_guard<std::mutex> lock(the_mutex);
	return buffer1_.size();
}


void double_buffer::clear() {
	std::lock_guard<std::mutex> lock(the_mutex);
	while (!buffer1_.empty())
		buffer1_.pop();
	while (!buffer2_.empty())
		buffer2_.pop();

}