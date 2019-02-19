#include "track.hpp"

Track::Track(intmax_t ticks_per_beat)
{
	ticks_per_beat_ = ticks_per_beat;
}

intmax_t Track::getTicksPerBeat() const
{
	return ticks_per_beat_;
}


Track::ConstIteratorType Track::begin() const
{
	return trackList.cbegin();
}

Track::ConstIteratorType Track::end() const
{
	return trackList.cend();
}

void Track::addEndEvent(intmax_t tick_time)
{
	EndTrackEvent end;
	MIDIEvent newEvent(end, tick_time);
	trackList.push_back(newEvent);
}

void Track::addTempoEvent(intmax_t tempo, intmax_t tick_time)
{
	TempoEvent temp(tempo);
	MIDIEvent newEvent(temp, tick_time);
	trackList.push_back(newEvent);
}

void Track::addNoteEvent(uint8_t code, uint8_t data1, uint8_t data2, intmax_t tick_time)
{
	bool on;
	if (code == 9 && data2 != 0)
		on = true;
	else if (code == 8)
		on = false;
	else
		on = false;
	NoteEvent note(on, data1, data2);
	MIDIEvent newEvent(note, tick_time);
	trackList.push_back(newEvent);
}