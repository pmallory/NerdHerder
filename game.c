/*
 * Philip Mallory
 */

#include <stdlib.h>
#include "myLib.h"
#include "nerdLib.h"
#include "nerd_sprite_sheet.h"
#include "nerd_bg.h"
#include "blue_danube.h"

// globals
extern int state;
extern struct score {
    int nerds_correct;
    int nerds_incorrect;
    int bros_correct;
    int bros_incorrect;
} score;

OBJ_ATTR shadowOAM[128];
int bg_hof = 0;
int bg_vof = 0;
int brandon_row = 6;
int brandon_col = 6;
NPC npcs[NPC_COUNT];
POWERUP dew = {236, 15, 0, 0};
POWERUP natty = {236, 489, 0 ,0};
int NPCs_remaining = NPC_COUNT;

void game() {
    // set up display register
    REG_DISPCTL = MODE0 | BG3_ENABLE | SPRITE_ENABLE;

    // set up bg
    REG_BG3CNT =  CBB(0) | SBB(28) | BG_SIZE3 | COLOR256;
    loadPalette(nerd_bgPal);
    DMANow(3, (u32 *) nerd_bgTiles, &CHARBLOCKBASE[0], nerd_bgTilesLen);
    DMANow(3, (u32 *) nerd_bgMap, &SCREENBLOCKBASE[28], nerd_bgMapLen);
    REG_BG3HOFS = bg_hof;
    REG_BG3VOFS = bg_vof;

    // set up sprite sheet and palette
    DMANow(3, (u32 *)nerd_sprite_sheetTiles, &CHARBLOCKBASE[4], nerd_sprite_sheetTilesLen/2);
    DMANow(3, (u32 *)nerd_sprite_sheetPal, SPRITE_PALETTE, nerd_sprite_sheetPalLen/2);

    // diasble all sprites
    int i;
    for (i=0; i<128; i++) {
        OAM[i].attr0 = ATTR0_HIDE;
    }

    // initialize NPCs (bros and nerds). row and col are relative to the
    // background (512x512).
    for (i=0; i<NPC_COUNT; i++) {
        npcs[i].row = rand()%(256-32);
        npcs[i].col = rand()%200+125;
        npcs[i].race = rand()%2;
        npcs[i].alive = 1;
        npcs[i].ai_state = IDLE;
    }

    // initialize brandon sprite
    brandon_sp.attr0 = ATTR0_8BPP | ATTR0_SQUARE | brandon_row;
    brandon_sp.attr1 = ATTR1_SIZE32 | brandon_col;
    brandon_sp.attr2 = 2*OFFSET(0,0,16);

    // initialize dew and natty power up sprites
    dew_sp.attr2 = 2*OFFSET(8, 0, 16); 
    natty_sp.attr2 = 2*OFFSET(8, 1, 16);

    // make sprites visible
    oam_copy(OAM, shadowOAM, NPC_COUNT+3);

    // play that funky music, on repeat, on channel A
    REG_SOUNDCNT_X = SND_ENABLED;
    playSoundA(blue_danube, BLUE_DANUBELEN*.9, BLUE_DANUBEFREQ, 1);

    while(state == GAME) {
        key_poll();

        move_brandon();
        handle_powerups();
        draw_npcs();
        capture_npcs();

        if (!NPCs_remaining)
            state = GAMEOVER;
        if (key_hit(BUTTON_SELECT))
            state = PAUSE;

        waitForVblank();
        oam_copy(OAM, shadowOAM, NPC_COUNT+3);
    }
 }

void move_brandon() {
    static int step_counter = 0;
    static int flipped = 0;

    // move brandon (or the screen, to keep him in his bounding box), advance
    // animation every five pixels.
    if (key_held(BUTTON_RIGHT) && brandon_col != 240-32) {
        flipped = 0;
        if (brandon_col < 94+48-32 || bg_hof == 512-240)
            brandon_col++;
        else
           REG_BG3HOFS = ++bg_hof;
        if (step_counter == 0)
            brandon_sp.attr2 = 2*OFFSET(4,0,16);
        else if (step_counter == 5)
            brandon_sp.attr2 = 2*OFFSET(4,4,16);
        else if (step_counter == 10)
            brandon_sp.attr2 = 2*OFFSET(4,0,16);
        else if (step_counter == 15)
            brandon_sp.attr2 = 2*OFFSET(4,4,16);
    } else if (key_held(BUTTON_LEFT) && brandon_col != 0) {
        flipped = 1;
        if (brandon_col > 96 || bg_hof == 0)
            brandon_col--;
        else
            REG_BG3HOFS = --bg_hof;
        if (step_counter == 0)
            brandon_sp.attr2 = 2*OFFSET(4,0,16);
        else if (step_counter == 5)
            brandon_sp.attr2 = 2*OFFSET(4,4,16);
        else if (step_counter == 10)
            brandon_sp.attr2 = 2*OFFSET(4,0,16);
        else if (step_counter == 15)
            brandon_sp.attr2 = 2*OFFSET(4,4,16);
   } else if (key_held(BUTTON_UP) && brandon_row != 0) {
        flipped = 0;
        if (brandon_row > 56 || bg_vof == 0)
            brandon_row--;
        else
            REG_BG3VOFS = --bg_vof; 
        if (step_counter == 0)
            brandon_sp.attr2 = 2*OFFSET(0,8,16);
        else if (step_counter == 5)
            brandon_sp.attr2 = 2*OFFSET(0,12,16);
        else if (step_counter == 10)
            brandon_sp.attr2 = 2*OFFSET(0,8,16);
        else if (step_counter >= 15) {
            brandon_sp.attr2 = 2*OFFSET(0,12,16);
            flipped = 1;
        }
    } else if (key_held(BUTTON_DOWN) && brandon_row < 160-32) {
        flipped = 0;
        if (brandon_row < 56+48-32 || bg_vof == 256-160)
            brandon_row++;
        else
            REG_BG3VOFS = ++bg_vof;
        if (step_counter == 0)
            brandon_sp.attr2 = 2*OFFSET(0,0,16);
        else if (step_counter == 5)
            brandon_sp.attr2 = 2*OFFSET(0,4,16);
        else if (step_counter == 10)
            brandon_sp.attr2 = 2*OFFSET(0,0,16);
        else if (step_counter >= 15){
            brandon_sp.attr2 = 2*OFFSET(0,4,16);
            flipped = 1;
        }
    }

    (step_counter < 20) ? step_counter++ : (step_counter=0);

    // put updated location into shadowOAM
    brandon_sp.attr0 = (brandon_sp.attr0 & ~0xFF) | brandon_row;
    brandon_sp.attr1 = (brandon_sp.attr1 & ~0x1FF) | brandon_col;
    if (flipped)
        brandon_sp.attr1 |= ATTR1_HFLIP;
    else
        brandon_sp.attr1 &= ~ATTR1_HFLIP;
}

void capture_npcs() {
    int i;
    for (i=0; i<NPC_COUNT; i++) {
        if (!npcs[i].alive)
            continue;

        // check if this NPC is in dorm
        if (47 < npcs[i].row && npcs[i].row < 152 && npcs[i].col < 64) {
            npcs[i].alive = 0;
            shadowOAM[i+3].attr0 = ATTR0_HIDE;
            npcs[i].race==NERD ? score.nerds_correct++ : score.bros_incorrect++;
            NPCs_remaining--;
        }

        //check if this NPC is on M-Train
        if (87 < npcs[i].row && npcs[i].row <144 && npcs[i].col > 375) {
            npcs[i].alive = 0;
            shadowOAM[i+3].attr0 = ATTR0_HIDE;
            npcs[i].race==BRO ? score.bros_correct++ : score.nerds_incorrect++;
            NPCs_remaining--;
        }
    }
}

void draw_npcs() {
    int i;
    for (i=0; i<NPC_COUNT; i++) {
        // go onto next npc if this one has already left the playfield
        if (!npcs[i].alive)
            continue;
            
        // calculate this NPCs distance from Brandon
        int row_dist = abs(npcs[i].row-bg_vof - brandon_row);
        int col_dist = abs(npcs[i].col-bg_hof - brandon_col);

        // NPCs nearby Brandon are affected by his aura.
        if (row_dist < 32 && col_dist < 32) {
            // follow if Brandon has the right power up
            if ((npcs[i].race == NERD && dew.picked_up && !dew.dead) ||
                (npcs[i].race == BRO && natty.picked_up && !natty.dead)) {
                if (npcs[i].row-bg_vof-brandon_row > 0)
                    (npcs[i].row-=rand()%5);
                else 
                    (npcs[i].row+=rand()%5);
                if (npcs[i].col-bg_hof-brandon_col > 0)
                    (npcs[i].col-=rand()%5);
                else
                    (npcs[i].col+=rand()%5);
            } else {  // flee otherwise
                if (npcs[i].row-bg_vof-brandon_row > 0)
                    (npcs[i].row+=rand()%5);
                else 
                    (npcs[i].row-=rand()%5);
                if (npcs[i].col-bg_hof-brandon_col > 0)
                    (npcs[i].col+=rand()%5);
                else
                    (npcs[i].col-=rand()%5);
            }
        } else { //NPCs not near Brandon meander about slowly
            if (rand()%100 < 5) {
                npcs[i].row += rand()%7-3;
                npcs[i].col += rand()%7-3;
            }
        }

        // make sure NPCs don't go off the edge 
        if (npcs[i].row < 0)
            npcs[i].row = rand()%4;
        if (npcs[i].row > 256-33)
            npcs[i].row = 256-33-rand()%4;
        if (npcs[i].col < 0)
            npcs[i].col = 0;
        if (npcs[i].col > 512-33)
            npcs[i].col = 512-33-rand()%4;

        // draw
        shadowOAM[i+3].attr0 = ATTR0_8BPP | ATTR0_SQUARE | (0xFF&(npcs[i].row-bg_vof));
        shadowOAM[i+3].attr1 = ATTR1_SIZE32 | (0x1FF&(npcs[i].col-bg_hof));
        if (npcs[i].race == NERD)
            shadowOAM[i+3].attr2 = 2*OFFSET(4,8,16); 
        else
            shadowOAM[i+3].attr2 = 2*OFFSET(4,12,16);
    }
}

void handle_powerups() {
    // when a power up is activated it stays active for enough time to walk
    // accross the map.
    static int dew_counter = 512;
    static int natty_counter = 512;
    
    // if brandon is standing on dew, pick it up
    if (!dew.picked_up &&
        (dew.row-bg_vof > brandon_row) &&
        (dew.row-bg_vof < brandon_row+32) &&
        (dew.col-bg_hof > brandon_col) &&
        (dew.col-bg_hof < brandon_col+32))
            dew.picked_up = 1;

    // is brandon is stanfing on natty, pick it up!
    if (!natty.picked_up &&
        (natty.row-bg_vof > brandon_row) &&
        (natty.row-bg_vof < brandon_row+32) &&
        (natty.col-bg_hof > brandon_col) &&
        (natty.col-bg_hof < brandon_col+32))
            natty.picked_up = 1;

    if (!dew.picked_up) { // sit in corner
        dew_sp.attr0 = ATTR0_8BPP | ATTR0_SQUARE | (0xFF&(dew.row-bg_vof));
        dew_sp.attr1 = ATTR1_SIZE8 | (0x1FF&(dew.col-bg_hof));
    } else if (!dew.dead) { // follow brandon
        dew_sp.attr0 = ATTR0_8BPP | ATTR0_SQUARE | (brandon_row+16);
        dew_sp.attr1 = ATTR1_SIZE8 | (brandon_col+16);
        dew_counter--;
    }

    if (!natty.picked_up) {
        natty_sp.attr0 = ATTR0_8BPP | ATTR0_SQUARE | (0xFF&(natty.row-bg_vof));
        natty_sp.attr1 = ATTR1_SIZE8 | (0x1FF&(natty.col-bg_hof));
    } else if (!natty.dead){ // follow brandon
        natty_sp.attr0 = ATTR0_8BPP | ATTR0_SQUARE | (brandon_row+16);
        natty_sp.attr1 = ATTR1_SIZE8 | (brandon_col+16);
        natty_counter--;
    }

    // deactivate powerups after awhile
    if (!dew.dead && !dew_counter) {
        dew.dead = 1;
        dew_sp.attr0 = ATTR0_HIDE;    
    }
    if (!natty.dead && !natty_counter) {
        natty.dead = 1;
        natty_sp.attr0 = ATTR0_HIDE;    
    }
}

