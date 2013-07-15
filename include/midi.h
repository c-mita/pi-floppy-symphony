#ifndef MIDI_H
#define MIDI_H

#include <vector>

struct note{
    int number;
    int length;
};

class MidiParse
{
    public:
        MidiParse(const char* midi_file, int max_tracks);
        int track_count;
        //int** tracks;
        std::vector< std::vector<note> > tracks;
        int error_code;
    private:
        int Parse(const char *file, int max_tracks);
        int GetVariable(char* buffer, int* offset);
};

#endif
