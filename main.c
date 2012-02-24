/*
 * Philip Mallory
 */

#include "myLib.h"
#include "nerdLib.h"

// global variables
int state = SPLASH;
struct score {
    int nerds_correct;
    int nerds_incorrect;
    int bros_correct;
    int bros_incorrect;
};
struct score score = {0, 0, 0, 0};

int main() {
    setupInterrupts();
    setupSounds();

    while(1) {
        switch(state) {
            case SPLASH:
                splash();
                break;
            case INSTRUCTIONS:
                instructions();
                break;
            case GAME:
                game();
                break;
            case GAMEOVER:
                gameover();
                break;
            case PAUSE:
                pause();
                break;
        }
    }
    return 0;
}

