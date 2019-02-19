/** \file The DefaultInstrument Module
 */
#ifndef INSTRUMENT_HPP
#define INSTRUMENT_HPP
#define _USE_MATH_DEFINES

#include "event.hpp"
#include "track.hpp"
#include <math.h>


/** The default instrument. A simple fixed length note instrument using a
 * a sinusoid weighted by an envelope function.
 */
class DefaultInstrument{
public:

  /** No default constructor. A DefaultInstrument must have an associated 
   * track. 
   */
  DefaultInstrument() = delete;

  /** Cannot be copied. 
   */
  DefaultInstrument(const DefaultInstrument & ) = delete;

  /** Cannot be assigned.
   */
  DefaultInstrument & operator=(const DefaultInstrument & ) = delete;

  /** Construct a Default Instrument using the specified track.
   * \param track the track to read events from. 
   */
  DefaultInstrument(const Track & track);

  /** Returns true of the DefaultInstrument has stopped processing events.
   * This can occu e.g. if all events have been consumed or a EndTrackEvent
   * has been processed.
   */
  bool halted();

  /** Return the sample after the (real) time increment. Samples the 
   * instrument based on its state after processing all events prior 
   * to this time.
   * \param deltaT the time increment in seconds.
   */
  double sample(double deltaT);   //deltaT = 1, then divide samplerate
  //MIDI time is represented by a tick integer
  //T = microseconds per beat(defined by the tempo)   event_ -> getTempo()
  //M = MIDI clock tiks per beat (defined in MIDI file,)  track_.getTicksPerBeat()
  //T' = (t/10^6) seconds per beat
  //hence seconds per MIDI clock tick = T'/M
  // event real time: getTickTime()*T'/M
  //process any new Track events that became active since the last sample time
  //sum the waveforms for the currently active notes
  //Must maintain a container of notes active at the current time
  //	-note number
  //	-note velocity
  //	-note duration (E)
  //	-time turned on

  void reset();

  // use a Track iterator to process all events up to the current sample time

  //Probably want to define private helper functions to:
  //	-convert a note number to a frequency
  //	-determine the length of a note
  //	-evaluate the envelope functions
  //	-sum the active notes

  //while loop for deltaT(sample)
  //if event time before sample realtime:
  //use isxxxEvent to decide event type and get important values like tempo, note, blablabla


private:
  // TODO
	struct NoteNode{
		int8_t noteNumber;
		int8_t noteVelocity;
		double eventTime;
		bool on;
	};

	void getNewNote(double eventTime);
	double Algorithm(int8_t noteNumber, int8_t velocity, double elapsed);
	double getFrequency(int8_t noteNumber);

	Track track_;
	Track::ConstIteratorType iterator_;
	std::vector<NoteNode> stateMachine_;
	double sampleNumber_;
	bool trackEnd;
	intmax_t tempo_;

};
	


#endif
