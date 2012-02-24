/*
 * Philip Mallory
 */ 

#include <stdio.h>
#include "myLib.h"
#include "nerdLib.h"
#include "text.h"

extern int state;
extern struct score {
    int nerds_correct;
    int nerds_incorrect;
    int bros_correct;
    int bros_incorrect;
} score;

void gameover() {
    // turn off sound!
    REG_SOUNDCNT_X = 0; 
    dma[1].cnt = 0;
    REG_TM0CNT = 0;

    REG_DISPCTL = MODE3 | BG2_ENABLE;
    fillScreen3(0); 

    char ncor[40];
    sprintf(ncor, "%d nerds herded correctly", score.nerds_correct);
    char ninc[40];
    sprintf(ninc, "%d nerds herded incorrectly", score.nerds_incorrect);
    char bcor[40];
    sprintf(bcor, "%d bros herded correctly", score.bros_correct);
    char binc[40];
    sprintf(binc, "%d bros herded incorrectly", score.bros_incorrect);
    
    double percent_correct = (((double)score.nerds_correct+
                                (double)score.bros_correct)/
                                NPC_COUNT)*100;
    char percent[40];
    sprintf(percent, "%.1f%% herded correctly", percent_correct);

    char qualitative_score[40];
    char qualitative_score2[40];
    if (percent_correct == 100)
        sprintf(qualitative_score, "Perfect job, congratulations!");
    else if (percent_correct > 85)
        sprintf(qualitative_score, "Pretty good, have a cupcake!");
    else {
        sprintf(qualitative_score, "You saved the HOPE scholarship fund a");
        sprintf(qualitative_score2, "lot of money. Congratualtions?");
    }
    drawString(1, 1, "Mission accomplished!", COLOR(31,31,31));
    drawString(21, 1, ncor, COLOR(31,31,31));
    drawString(31, 1, ninc, COLOR(31,31,31));
    drawString(41, 1, bcor, COLOR(31,31,31));
    drawString(51, 1, binc, COLOR(31,31,31));
    drawString(71, 1, percent, COLOR(31,31,31));
    drawString(81, 1, qualitative_score, COLOR(31,31,31));
    drawString(91, 1, qualitative_score2, COLOR(31,31,31));
    drawString(151, 1, "Press select to try again!" ,COLOR(31,31,31));

    while (state == GAMEOVER) {
        key_poll();
        if (key_hit(BUTTON_SELECT))
            // soft reset bios call, returns us to fresh state
            __asm__("swi 0x00");
    }

}

