#ifndef FLOPPY_H
#define FLOPPY_H

class Floppy
{
    public:
        Floppy( int pStep, int pDuration );
        void Reset();
        void Tone( float frequency, int duration );
        void Switch();
    private:
        int pinStep;
        int pinDir;
        int position;
        int maxPosition;
        bool direction;
        bool on;
};


#endif
