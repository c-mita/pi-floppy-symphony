#include <initializer_list>
#include "Floppy.h"
#include "Midi.h"
#include <time.h>
#include <thread>
#include <pthread.h>
#include "FloppyPlayer.h"
#include <wiringPi.h>
#include <vector>
#include <cmath>
#include <cstdio>


FloppyPlayer::FloppyPlayer( std::initializer_list<int> pins )
{
    if ( 1 == pins.size() & 1 ) throw;
    for ( auto i = pins.begin(); i != pins.end(); i+=2 ) {
        Floppy* drive = new Floppy( *i, *(i+1) );
        FloppyInstance dataDrive = {drive, false, 0, 0, false};
        floppies.push_back( dataDrive );
    }
    double coefficient = 1000000.0 / 440.0;
    for ( int i = 0; i < 128; i++ ) {
        //subtract 1 in the exponent because the playback loop only does one switch per pass
        double multiplier = pow( 2.0, ( ( 69.0 - (double) i ) / 12.0 ) - 1 );
        notePeriods[i] = static_cast<int> ( coefficient * multiplier );
    }
}

FloppyPlayer::~FloppyPlayer()
{
    for ( int i = 0; i < floppies.size(); i++ ) {
        delete floppies[i].floppy;
    }
}

void FloppyPlayer::Play( std::vector< midiEvent >* track )
{
    finished = false;
    timespec delay;
    std::thread threadLoop( &FloppyPlayer::MidiLoop, this );
    
    struct sched_param threadParams;
    threadParams.sched_priority = 1;
    pthread_setschedparam( threadLoop.native_handle(), SCHED_FIFO, &threadParams );
    
    for ( auto instance = floppies.begin(); instance != floppies.end(); instance++ ) {
        instance->floppy->Reset();
        instance->floppy->Tone( 440.0, 1000 );
    }
    for ( auto event = track->begin(); event != track->end(); event++ ) {
        delay.tv_sec = event->delay / 1000000;
        delay.tv_nsec = ((event->delay) % 1000000) * 1000;
        nanosleep( &delay, NULL );
        printf( "delay = %d\t%d\tperiod = %d\n", delay.tv_sec, delay.tv_nsec, notePeriods[event->value] );
        switch ( event->type ) {
        case NOTE_DOWN:
            for ( int i = 0; i < floppies.size(); i++ ) {
                if ( !floppies[i].playing ) {
                    floppies[i].period = notePeriods[event->value];
                    floppies[i].playing = true;
                    break;
                }
            }
            break;
        case NOTE_UP:
            for ( int i = 0; i < floppies.size(); i++ ) {
                if ( floppies[i].playing && floppies[i].note == event->value ) {
                    floppies[i].playing = false;
                }
            }
            break; 
        }
    }
    finished = true;
    threadLoop.join();
}

void FloppyPlayer::MidiLoop()
{
    printf("Started thread\n");
    unsigned int lastPoll = micros();
    timespec delay;
    delay.tv_sec = 0;
    delay.tv_nsec = resolution * 1000;
    printf("tick is %dus\n", delay.tv_nsec / 1000);
    while ( !finished ) {
        unsigned int temp = micros();
        for ( int i = 0; i < floppies.size(); i++ ) {
            if ( floppies[i].playing && floppies[i].next <= 0 ) {
                floppies[i].floppy->Switch();
                floppies[i].next = floppies[i].period;
            }
            floppies[i].next -= (temp - lastPoll);
        }
        delay.tv_nsec = (resolution - (temp - lastPoll)) * 1000;
        lastPoll = temp;
        nanosleep( &delay, NULL );
    }
}
