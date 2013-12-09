#ifndef MIDI_H
#define MIDI_H

#include <vector>

enum eventType{
    NOTE_DOWN,
    NOTE_UP,
    TEMPO
};

struct midiEvent{
    const int value;
    const eventType type;
    const int delay;
};



#endif
