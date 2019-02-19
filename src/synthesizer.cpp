#include"synthesizer.hpp"

void synthesize(message_queue *mq, double_buffer *db){
	Track track;
	double deltaT = 1 / double(8000);
	double value;
	int state = 0;//0 is unparsed, 1 is parsed stop
	while (true)
	{
		QueueType a;
		mq->wait_and_pop(a);
		if (a.flag == 1)
		{
			track = readMIDIFromFile(a.input).front();
			state = 1;
		}
		else if (a.flag == 2 && state == 1)
		{
			DefaultInstrument inst(track);
			while (true)
			{
				QueueType b;
				if (mq->try_pop(b))
				{
					if (b.flag == 3)
					{
						while (true)
						{
							mq->try_pop(b);
							if (b.flag == 2 || b.flag == 4)
							{
								break;
							}	
						}
					}
					if (b.flag == 4)
					{
						state = 1;
						break;
					}
				}
				if (!inst.halted())
				{
					value = inst.sample(deltaT);
					//sig.push_back(value);//push to double buffer
				}
				else
					value = 0;
				while (db->getSize() > 10000)
				{
					int x = 0;
				}
				db->push(value);


			}
		}
		else if (a.flag == 5)
		{
			break;
		}

	}
	

	
	
	
}