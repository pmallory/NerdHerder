/*
 * Philip Mallory
 */

#include "text.h"
#include "myLib.h"
#include "nerdLib.h"

extern int state;

void instructions() {
    REG_DISPCTL = MODE3 | BG2_ENABLE;
    fillScreen3(COLOR(0,31,31)); 

    drawRect3(0, 0,0,240,160);

    drawString(1, 1, "NERD HERDER!", COLOR(31,31,31));
    drawString(21, 1, "As head TA you control the destiny of", COLOR(31,31,31));
    drawString(31, 1, "scores of students. It takes dedication,", COLOR(31,31,31));
    drawString(41, 1, "strength, and the ability to intimidate.", COLOR(31,31,31));
    drawString(51, 1, "In this game you separate the wheat", COLOR(31,31,31));
    drawString(61, 1, "from the chaff. Intimidate the nerds to", COLOR(31,31,31));
    drawString(71, 1, "their dorm so they'll do their homework.", COLOR(31,31,31));
    drawString(81, 1, "Send the flunkies to the M-Train so they", COLOR(31,31,31));
    drawString(91, 1, "won't bother the rest of us. Also there", COLOR(31,31,31));
    drawString(101, 1, "might be power ups?", COLOR(31,31,31));
    drawString(121, 1, "Press A to return", COLOR(31,31,31));

    while (state == INSTRUCTIONS) {
        key_poll();
        if (key_hit(BUTTON_A))
            state = SPLASH;
    }
}

