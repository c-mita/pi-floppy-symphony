#include "floppy.h"
#include <time.h>
#include <wiringPi.h>
#include <cstdio>

Floppy::Floppy(int step_pin, int dir_pin)
{
    p_step = step_pin;
    p_dir = dir_pin;
    pinMode(p_step,OUTPUT);
    pinMode(p_dir,OUTPUT);
    max_pos = 74;
    direction = false;
}

void Floppy::Reset()
{
    digitalWrite(p_dir, LOW);
    digitalWrite(p_step, LOW);
    struct timespec delay;
    delay.tv_sec = 0;
    delay.tv_nsec = 10000000;
    for(int i=0;i<100;i++){
        digitalWrite(p_step, HIGH);
        nanosleep(&delay,NULL);
        digitalWrite(p_step, LOW);
        nanosleep(&delay,NULL);
    }
    digitalWrite(p_dir, HIGH);
    for(int i=0;i<5;i++){
        digitalWrite(p_step, HIGH);
        nanosleep(&delay, NULL);
        digitalWrite(p_step, LOW);
        nanosleep(&delay, NULL);
    }
    digitalWrite(p_dir, LOW);
    pos = 0;
    direction = true;
}

void Floppy::Tone(float freq, int duration)
{
    if(0.0==freq){
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
    delay.tv_nsec = (long)(1000000000*(1 / freq));
    long long int count = ( long long int)((freq*((float)duration))/1000); //duration / (delay.tv_nsec / 1000000);
    digitalWrite(p_dir, direction ? HIGH : LOW);
    for(int i=0;i<count;i++){
        digitalWrite(p_step, HIGH);
        nanosleep(&delay, NULL);
        digitalWrite(p_step, LOW);
        pos+= direction ? 1 : -1;
        if(direction && (max_pos <= pos)) {
            direction = false;
            digitalWrite(p_dir, LOW);
            //printf("here");
        }
        if((!direction) && (0 == pos)){
            direction = true;
            digitalWrite(p_dir, HIGH);
        }
    }
    //printf("position: %d\tcount: %d\t  freq: %f\n",pos,count,freq);
    nanosleep(&gap, NULL);
    //printf("%d\n",count);
    //direction = !direction;
    
}
