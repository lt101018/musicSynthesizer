#ifndef SYNTHESIZER_HPP
#define SYNTHESIZER_HPP

/** \file Synthesizer Module
*/
#include "midi.hpp"
#include "track.hpp"
#include "instrument.hpp"
#include <iostream>
#include "message_queue.hpp"
#include "shared_double_buffer.hpp"

void synthesize(message_queue *mq, double_buffer *db);

#endif