#ifndef FLOPPYPLAYER_H
#define FLOPPYPLAYER_H

#include <vector>
#include <initializer_list>
#include "Floppy.h"


struct FloppyInstance {
    Floppy* floppy;
    bool playing;
    int period;
    int next;
    int note;
};

class FloppyPlayer
{
public:
    FloppyPlayer( std::initializer_list<int> pins );
    ~FloppyPlayer();
    void Play( std::vector< midiEvent >* track );
private:
    void MidiLoop();
    std::vector<FloppyInstance> floppies;
    bool finished;
    const int resolution = 40;
    int notePeriods[128];         
};


#endif