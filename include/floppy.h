#ifndef FLOPPY_H
#define FLOPPY_H

class Floppy
{
    public:
        Floppy(int step_pin, int dir_pin);
        void Reset();
        void Tone(float freq, int duration);
    private:
        int p_step; //step pin
        int p_dir; //direction pin
        int pos; //head position
        int max_pos;
        bool direction;
};


#endif
