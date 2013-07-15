#include <wiringPi.h>
#include <time.h>
#include <cstdio>
#include "floppy.h"
#include <cmath>
#include "midi.h"
#include <vector>


float freqs[129] = {0};

const char midifile[] = "tetris.mid";

int main()
{
    if(-1 == wiringPiSetupGpio()) {
        //std::cout<<"Could not setup GPIO"<<std::endl;
        printf("Could not setup GPIO!\n");
        return 0;
    }
    Floppy drive = Floppy(4, 17);
    printf("Recalibrating drive\n");
    drive.Reset();
    
    for(int i=0;i<128;i++){
        freqs[i] = 440.0;
        freqs[i] *= pow(2.0, (float)(i-69)/(float)12);
    }
    
    printf("Drives set\nLet's make some noise!\n");
    
    MidiParse parser = MidiParse(midifile,1);
    
    printf("Parsed\n");
    
    std::vector<note> score = parser.tracks[0];
    for(int i=0;i<score.size();i++){
        if(score[i].number == 128) drive.Tone(freqs[score[i].number],score[i].length);
        else drive.Tone(freqs[score[i].number-12],score[i].length);
        //printf("%d\n",score[i].length);
    }
    //printf("%d\n",score.size());    
    /*drive.Tone(freqs[64],400);
    drive.Tone(freqs[59],200);
    drive.Tone(freqs[60],200);
    drive.Tone(freqs[62],400);
    drive.Tone(freqs[60],200);
    drive.Tone(freqs[59],200);
    drive.Tone(freqs[57],400);*/
    
    /*drive.Tone(329.63, 400);
    drive.Tone(246.94, 200);
    drive.Tone(261.63, 200);
    drive.Tone(293.66, 400);
    drive.Tone(261.63, 200);
    drive.Tone(246.94, 200);
    drive.Tone(220.00, 400); //second bar
    drive.Tone(220.00, 200);
    drive.Tone(261.63, 200);
    drive.Tone(329.63, 400);
    drive.Tone(293.66, 200);
    drive.Tone(261.63, 200);
    drive.Tone(246.94, 600); //third bar
    drive.Tone(261.63, 200);
    drive.Tone(293.63, 400);
    drive.Tone(329.63, 400);
    drive.Tone(261.63, 400); //fourth bar
    drive.Tone(220.00, 400);
    drive.Tone(220.00, 800);*/
    printf("Done\n");
    return 0;
}

