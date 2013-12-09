#include "Floppy.h"
#include <time.h>
#include <wiringPi.h>

Floppy::Floppy( int pStep, int pDir )
{
    pinStep = pStep;
    pinDir = pDir;
    pinMode(pinStep,OUTPUT);
    pinMode(pinDir,OUTPUT);
    maxPosition = 74;
    direction = false;
}

void Floppy::Reset()
{
    digitalWrite( pinDir, LOW );
    digitalWrite( pinStep, LOW );
    struct timespec delay;
    delay.tv_sec = 0;
    delay.tv_nsec = 10000000;
    for(int i=0;i<100;i++){
        digitalWrite( pinStep, HIGH );
        nanosleep( &delay,NULL );
        digitalWrite( pinStep, LOW );
        nanosleep( &delay,NULL );
    }
    digitalWrite( pinDir, HIGH );
    for(int i=0;i<5;i++){
        digitalWrite( pinStep, HIGH );
        nanosleep( &delay, NULL );
        digitalWrite( pinStep, LOW );
        nanosleep( &delay, NULL );
    }
    digitalWrite( pinDir, LOW );
    position = 0;
    direction = true;
}

void Floppy::Switch()
{
    if (on) {
        digitalWrite( pinStep, LOW );
        position += direction ? 1 : -1;
        if ( direction && (maxPosition <= position) ) {
            direction = false;
            digitalWrite( pinDir, LOW );
        } else if ( (!direction) && (0 == position) ) {
            direction = true;
            digitalWrite( pinDir, HIGH );
        }
    } else {
        digitalWrite( pinStep, HIGH );
    }
    on = !on;
}

void Floppy::Tone( float frequency, int duration )
{
    if(0.0==frequency){
        struct timespec pause;
        pause.tv_sec = 0;
        pause.tv_nsec = 1000000*duration;
        nanosleep(&pause, NULL);
        return;
    }
    int t = duration;
    struct timespec gap;
    gap.tv_sec = 0;
    gap.tv_nsec = 40000000;
    duration -= (gap.tv_nsec / 1000000);
    struct timespec delay;
    delay.tv_sec = 0;
    delay.tv_nsec = (long)(1000000000*(1 / frequency));
    long long int count = ( long long int)((frequency*((float)duration))/1000); //duration / (delay.tv_nsec / 1000000);
    digitalWrite(pinDir, direction ? HIGH : LOW);
    for(int i=0;i<count;i++){
        digitalWrite(pinStep, HIGH);
        nanosleep(&delay, NULL);
        digitalWrite(pinStep, LOW);
        position+= direction ? 1 : -1;
        if(direction && (maxPosition <= position)) {
            direction = false;
            digitalWrite(pinDir, LOW);
            //printf("here");
        }
        if((!direction) && (0 == position)){
            direction = true;
            digitalWrite(pinDir, HIGH);
        }
    }
    nanosleep(&gap, NULL);
    
}
