#include "instrument.hpp"

// TODO

DefaultInstrument::DefaultInstrument(const Track & track)
{
	track_ = track;
	iterator_ = track_.begin();
	sampleNumber_ = 0;
	tempo_ = 500000;
	trackEnd = false;
}

bool DefaultInstrument::halted() {
	return (trackEnd);
}

void DefaultInstrument::getNewNote(double eventTime) {
	NoteNode newNote;
	bool existed = false;
	newNote.eventTime = eventTime;
	newNote.noteNumber = iterator_->asNoteEvent().getNoteNumber();
	newNote.noteVelocity = iterator_->asNoteEvent().getNoteVelocity();
	newNote.on = iterator_->asNoteEvent().getNoteOn();
	if(stateMachine_.empty())
		stateMachine_.push_back(newNote);
	for (auto it = stateMachine_.begin(); it != stateMachine_.end(); it++)
	{
		if (newNote.noteNumber == it->noteNumber)
		{
			existed = true;
			if (newNote.on == true && it->on == false)
			{
				it->on = true;
				it->eventTime = newNote.eventTime;
				it->noteVelocity = newNote.noteVelocity;
				break;
			}
			else if (newNote.on == false && it->on == true)
			{
				it->on = false;
				break;
			}
			else
				break;
		}
	}
	if(existed == false)
		stateMachine_.push_back(newNote);
}

double DefaultInstrument::getFrequency(int8_t noteNumber) {
	int octave = noteNumber / 12 - 5;
	int key = noteNumber % 12;
	double frequency;
	if (key == 0)
		frequency = 261.63;
	else if (key == 1)
		frequency = 277.18;
	else if (key == 2)
		frequency = 293.66;
	else if (key == 3)
		frequency = 311.13;
	else if (key == 4)
		frequency = 329.63;
	else if (key == 5)
		frequency = 349.63;
	else if (key == 6)
		frequency = 369.994;
	else if (key == 7)
		frequency = 392.0;
	else if (key == 8)
		frequency = 415.305;
	else if (key == 9)
		frequency = 440.0;
	else if (key == 10)
		frequency = 466.164;
	else if (key == 11)
		frequency = 493.88;

	frequency = frequency * pow(2, octave);
	return frequency;
}

double DefaultInstrument::Algorithm(int8_t noteNumber, int8_t velocity, double elapsed) {
	double envelop;
	double frequency = getFrequency(noteNumber);
	if (elapsed >= 0 && elapsed <= 0.0625)
		envelop = 1 / 0.0625 * elapsed;
	else if (elapsed > 0.0625 && elapsed <= 0.1125)
		envelop = 1 - (elapsed - 0.0625);
	else if (elapsed > 0.1125 && elapsed <= 0.2)
		envelop = 0.8;
	else if (elapsed > 0.2 && elapsed <= 0.25)
		envelop = 0.8 - (0.8 / 0.05)*(elapsed - 0.2);
	else
		envelop = 0;
	double output = 200 * velocity * envelop * sin(2 * M_PI*elapsed*frequency);
	return output;
}

double DefaultInstrument::sample(double deltaT) {
	double currentTime = sampleNumber_ * deltaT;
	sampleNumber_ = sampleNumber_ + 1;
	double sum = 0;
	while (!(iterator_ == track_.end()) )
	{
		double newEventTime = double(iterator_->getTickTime()) * double(tempo_) / 1000000 / double(track_.getTicksPerBeat());
		if (newEventTime > currentTime)
			break;
		if (iterator_->isTempoEvent())
			tempo_ = iterator_->asTempoEvent().getTempo();
		else if (iterator_->isNoteEvent()) {
			getNewNote(newEventTime);
		}
		else if (iterator_->isEndTrackEvent())
			trackEnd = true;
		else
			break;
		iterator_++;
	}
	
	for (auto it = stateMachine_.begin(); it != stateMachine_.end(); it++)
	{
		double elapsed = currentTime - it->eventTime;
		sum = sum + Algorithm(it->noteNumber, it->noteVelocity, elapsed);
	}
	return sum;
}

void DefaultInstrument::reset() {
	stateMachine_.clear();
	iterator_ = track_.begin();
	sampleNumber_ = 0;
	tempo_ = 500000;
	trackEnd = false;

}