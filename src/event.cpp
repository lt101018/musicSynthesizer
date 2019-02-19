#include "event.hpp"

// TODO

//constructor
TempoEvent::TempoEvent(intmax_t t)
{
	tempo_ = t;
}

intmax_t TempoEvent::getTempo() const
{
	return tempo_;
}

NoteEvent::NoteEvent(bool on, int8_t notenum, int8_t notevelocity)
{
	on_ = on;
	notenum_ = notenum;
	notevelocity_ = notevelocity;
}

// Get note change type. true if ON, false if OFF.
bool NoteEvent::getNoteOn()
{
	return on_; 
}

// Get MIDI note number to change. Takes on values in [0,127]
int8_t NoteEvent::getNoteNumber() const
{
	return notenum_;
}

// Get the velocity of the note change.
int8_t NoteEvent::getNoteVelocity() const
{
	return notevelocity_;
}


MIDIEvent::MIDIEvent(TempoEvent e, intmax_t ticktime)
{
	evenType_ = tempo;
	event_.tempoEvent_ = e;
	ticktime_ = ticktime;
}

MIDIEvent::MIDIEvent(EndTrackEvent e, intmax_t ticktime)
{
	evenType_ = end;
	event_.endTrackEvent_ = e;
	ticktime_ = ticktime;
}

MIDIEvent::MIDIEvent(NoteEvent e, intmax_t ticktime)
{
	evenType_ = note;
	event_.noteEvent_ = e;
	ticktime_ = ticktime;
}

bool MIDIEvent::isTempoEvent() const
{
	return (evenType_ == tempo);
}

bool MIDIEvent::isEndTrackEvent() const
{
	return (evenType_ == end);
}

bool MIDIEvent::isNoteEvent() const
{
	return (evenType_ == note);
}

intmax_t MIDIEvent::getTickTime() const
{
	return ticktime_;
}

TempoEvent MIDIEvent::asTempoEvent() const
{
	return event_.tempoEvent_;
}

NoteEvent MIDIEvent::asNoteEvent() const
{
	return event_.noteEvent_;
}