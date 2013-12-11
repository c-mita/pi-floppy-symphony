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
    const int deltaDelay;
    const int track;
};

class MidiFile
{
    public:
    MidiFile( const char* fileInput );
    void Parse( const char* fileInput );
    std::vector<midiEvent> events;
    int clockRate;
    int type;
    int nTracks;

    private:
    void MergeTracks( std::vector< std::vector< midiEvent > * > * tracks );
    static int GetVarLength( char* buffer, int* offset );
    
};


#endif
