#include <iostream>
#include <fstream>
#include "midi.h"
#include <vector>
#include <cstdio>

MidiParse::MidiParse(const char* midi_file, int max_tracks)
{
    error_code = Parse(midi_file, max_tracks);
    //printf("%d",error_code);
}

int MidiParse::Parse(const char* midi_file, int max_tracks)
{
    std::ifstream file;
    file.open(midi_file);
    
    //read header
    char header[14];
    file.read(header, 14);
    if(!((0x4d == header[0]) && (0x54 == header[1]) \
      && (0x68 == header[2]) && (0x64 == header[3]))) {return -1;}
    
    if(1 < header[9]) {return -2;}
    track_count = header[10];
    track_count<<=8;
    track_count += header[11];
    //printf("%d\n",track_count);
    track_count = ((track_count >= max_tracks) ? max_tracks : track_count);
    int rate = header[12];
    rate <<= 8;
    rate += header[13];
    
    file.seekg(0, file.end);
    int length = file.tellg();
    file.seekg(0, file.beg);
    file.ignore(14, EOF);
    //printf("%x\n",file.get());
    char* data = new char[length-14];
    file.read(data, length-14);
    //printf("%x\n",data[0]);
    
    //tracks = new int*[track_count];
    for(int i=0;i<track_count;i++){
        std::vector<note> notes;
        tracks.push_back(notes);
    }
   
    file.close();
    
    //find first track
    int index = 0;
    //we start at one to skip the first track, because it's not useful
    for(index = 1; index<length-14; index++){
        int i = index;
        if ('M'==data[i] && 'T'==data[i+1] && 'r'==data[i+2] && 'k'==data[i+3]) break;
    }
    index += 4;
    //extract the size - the next 4 bytes
    int tracksize = (data[index]<<24) + (data[index+1]<<16) \
        + (data[index+2]<<8) + data[index+3];
    index += 4;
    
    int tempo = 150;
    int offset = index;
    int cdelta = 0;
    int current_note = -1;
    printf("Before loop\n");
    printf("%d\n",tracksize);
    while((offset-index) <= tracksize){
        int delta = GetVariable(data, &offset); //updates offset to past delta
        char type = data[offset++];
        int var_length = 0;
        cdelta += delta;
        //printf("type = %x\n",type);
        switch(type){
            case 0xff:
                //meta event
                //don't care about type
                offset++;
                var_length = GetVariable(data, &offset);
                offset+=var_length; //skip meta event;
                break;
                
            case 0xf0:
            case 0xf7:
                //system event
                var_length = GetVariable(data, &offset);
                offset+=var_length;
                break;
                
            default:
                //we might care about this event
                if(0x80 == (type & 0xF0)){
                    //note off
                    //float beats = (float)cdelta / (float)rate;
                    //beats *= (float)tempo;
                    //beats /= 60.0;
                    int numerator = cdelta * 60000;
                    int den = rate * tempo;
                    //printf("num = %d\t den = %d\n",numerator,den);
                    //if(current_note < 0) printf("oh shit!\n");
                    tracks[0][current_note].length = numerator / den;
                    cdelta = 0;
                }
                if(0x90 == (type & 0xF0)){
                    //note on
                    //check to see there was a pause
                    if(0 != cdelta){
                        //add note with number 128;
                        note pause;
                        pause.length = cdelta * 60000;
                        pause.length = pause.length / (rate*tempo);
                        pause.number = 128;
                        tracks[0].push_back(pause);
                        current_note++;
                    }
                    note temp;
                    temp.length = 0;
                    temp.number = data[offset];
                    tracks[0].push_back(temp);
                    current_note++;
                    cdelta=0;
                    //tracks[0][current_note].length = 0;
                    //tracks[0][current_note].number = data[offset];
                }
                if((0xC0 == (type & 0xF0)) || (0xD0 == (type & 0xF0))){
                    offset+=1; //skip over parameter
                } else {
                    offset+=2; //skip over parameters
                }                
                break;
        }
        //printf("looped with offset = %d\n",offset-index);
    }
    delete[] data;
    /*note final_note;
    final_note.length = 0;
    final_note.number = -1;
    tracks[0].push_back(final_note);*/
    return 0;
}

int MidiParse::GetVariable(char* data, int* offset)
{
    int var = 0;
    for(int i=0;i<4;i++){
        var <<= 7;
        var |= (data[offset[0]] & 0x7f);
        if(0 == (data[(offset[0])++] & 0x80)) break;
    }
    return var;
}
