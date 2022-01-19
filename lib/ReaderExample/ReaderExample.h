#ifndef READEREXAMPLE_H_
#define READEREXAMPLE_H_

#include "common/TimeRepresentation.h"

class Reader_ifs;
class PrmBuffer_ifs;

PrmBuffer_ifs* createNewPrmBufferSimulator(size_t sample_frequency_log2, size_t intervals_buffer_length = (1 << 12),
                                           RelativeTime buffer_interval = {0, 30}, double signal_frequency = 0.1);

void removeAllPrmBufferSimulator();

Reader_ifs* getReaderExample(PrmBuffer_ifs*);

#endif
