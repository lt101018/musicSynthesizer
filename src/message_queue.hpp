#ifndef MESSAGE_QUEUE_HPP
#define MESSAGE_QUEUE_HPP

#include <queue>
#include <string>
#include <mutex>
#include <condition_variable>
#include <iostream> //for test

struct Message
{
	int flag; //1 is input, 2 is play, 3 is pause, 4 is stop, 5 is quit
	std::string input;
};

typedef Message QueueType;  //need to be fixed

class message_queue
{
public:

	void push(QueueType const& message);

	bool empty() const;

	bool try_pop(QueueType& popped_value);

	void wait_and_pop(QueueType& popped_value);

private:
	std::queue<QueueType> the_queue;
	mutable std::mutex the_mutex;
	std::condition_variable the_condition_variable;

};

#endif