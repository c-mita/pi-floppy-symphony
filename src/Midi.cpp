#include "Midi.h"
#include <fstream>
#include <cstring>
#include <climits>
#include <cstdio>

MidiFile::MidiFile( const char* fileInput )
{
    Parse( fileInput );
}

void MidiFile::Parse( const char* fileInput )
{
    std::ifstream file;
    file.open( fileInput );
    printf("Opened file\n");
    //read header
    char header[14];
    file.read( header, 14 );
    //test for MThd
    if ( !(0x4d == header[0] && 0x54 == header[1] 
        && 0x68 == header[2] && 0x64 == header[3]) ) return;
        
    type = header[9];
    if ( type >= 2 ) return; //no thanks
    
    nTracks = header[10];
    nTracks <<= 8;
    nTracks += header[11];
    
    clockRate = header[12];
    clockRate <<= 8;
    clockRate += header[13];
    
    file.seekg( 0, file.end );
    int length = file.tellg();
    file.seekg( 0, file.beg );
    file.ignore( 14, EOF );
    char* data = new char[ length - 14 ];
    file.read( data, length-14 );
    
    std::vector< std::vector<midiEvent> * > tracks;
    for ( int i = 0; i < nTracks; i++ ) { 
        tracks.push_back( new std::vector<midiEvent> );
    }
    file.close();
    printf("Closed file\n");
    int index = 0;
    int track = 0;
    int dataLength = length - 14;
    while ( index < dataLength - 3 && track < nTracks ) {
        if ( 'M' == data[index] && 'T' == data[index+1]  \
        && 'r' == data[index+2] && 'k' == data[index+3] ) {
            //start of new track, read track data
            index += 4;
            int trackSize = ( data[index] << 24 ) + ( data[index+1] << 16 ) \
                + ( data[index+2] << 8 ) + data[index+3];
            index += 4;
            //read track data
            //printf("track size = %d\n",trackSize);
            int trackOffset = index;
            while ( trackOffset <= index + trackSize ) {
                //printf("das While loop: trackOffset = %d\n", trackOffset);
                int delta = GetVarLength( data, &trackOffset );
                char type = data[trackOffset++];
                int varLength = 0;
                switch ( type ) {
                case 0xFF:
                    //meta event - check for tempo
                    if ( 0x51 == data[trackOffset] ) {
                        trackOffset += 2; //skip length too - it's three
                        int valueTempo = ( data[trackOffset] << 16 ) \
                            + ( data[trackOffset+1] << 8 ) + ( data[trackOffset+2] );
                        tracks[track]->push_back( { valueTempo, TEMPO, delta, track } );
                        trackOffset+=3;
                    } else {
                        //don't care
                        trackOffset++;
                        varLength = GetVarLength( data, &trackOffset );
                        trackOffset += varLength;
                    }
                    break;
                
                case 0xF0:
                case 0xF7:
                    //system event - don't care
                    varLength = GetVarLength( data, &trackOffset );
                    trackOffset += varLength;
                    break;
                    
                default:
                    //check for note up or down events
                    int typeHighByte = type & 0xF0;
                    if ( 0x80 == typeHighByte || 0x90 == typeHighByte ) {
                        int valueNote = data[trackOffset];
                        tracks[track]->push_back(  
                        { valueNote, 0x80 == typeHighByte ? NOTE_UP : NOTE_DOWN, delta, track } );
                    }
                    //jump over parameters
                    if ( 0xC0 == typeHighByte || 0xD0 == typeHighByte ) {
                        trackOffset += 1;
                    } else {
                        trackOffset += 2;
                    }
                    break;
                }
            }
            index += trackSize - 1;
            track++;
        }
        index++;
    }
    MergeTracks( &tracks );
    for ( int i = 0; i < nTracks; i++ ){
        delete tracks[i];
    }
}

void MidiFile::MergeTracks( std::vector< std::vector< midiEvent > * > * tracks )
{
    printf("%d tracks\n", tracks->size());
    for ( int i = 0; i < tracks->size(); i++ ) {
        printf( "track %d: %d\n", i, (*tracks)[i]->size() );
    }
    int deltaTotal = 0;
    int* deltaTracks = new int[ tracks->size() ];
    int* indexes = new int[ tracks->size() ];
    std::memset( deltaTracks, 0, sizeof(int) * tracks->size() );
    std::memset( indexes, 0, sizeof(int) * tracks->size() );
    bool finished = false;
    int index = 0;
    while ( !finished ) {
        int deltaMin = INT_MAX;
        int nextTrack = 0;
        finished = true;
        for ( int i = 0; i < tracks->size(); i++ ) {
            if ( (*tracks)[i]->size() == indexes[i] ) {
                continue;
            } else {
                finished = false; 
            }
            std::vector< midiEvent > * tempTrack = (*tracks)[i];
            if ( deltaMin > (*tempTrack)[ indexes[i] ].deltaDelay + deltaTracks[i] ) {
                deltaMin = (*tempTrack)[ indexes[i] ].deltaDelay + deltaTracks[i];
                nextTrack = i;
            }
        }
        std::vector< midiEvent > * track = (*tracks)[nextTrack];
        midiEvent * nextEvent = & ( (*track)[ indexes[nextTrack] ] );
        events.push_back( { nextEvent->value,  
                            nextEvent->type,
                            nextEvent->deltaDelay + deltaTracks[nextTrack] - deltaTotal,
                            nextEvent->track } );
        //events.push_back( {0, NOTE_UP, 0, 0} );
        deltaTotal += nextEvent->deltaDelay;
        deltaTracks[nextTrack] = deltaTotal;
        indexes[nextTrack]++;
    }
    delete [] indexes;
    delete [] deltaTracks;
}

int MidiFile::GetVarLength( char* buffer, int* offset )
{
    int var = 0;
    for ( int i = 0; i < 4; i++ ) {
        var <<= 7;
        var |= ( buffer[*offset] & 0x7F );
        if ( 0 == ( buffer[ (*offset)++ ] & 0x80 )) break;
    }
    return var;
}
