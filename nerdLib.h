/*
 * Philip Mallory
 */

#ifndef NERD_LIB
#define NERD_LIB

#define brandon_sp shadowOAM[2]
#define dew_sp shadowOAM[0]
#define natty_sp shadowOAM[1]

enum races {NERD, BRO};
enum AI_states {IDLE, ATTRACT, REPEL};
enum states {SPLASH, INSTRUCTIONS, GAME, GAMEOVER, PAUSE};

#define NPC_COUNT 50

typedef struct {
    int row;
    int col;
    int race;
    int alive;
    int ai_state;
} NPC;

typedef struct {
    int row;
    int col;
    int picked_up;
    int dead;
} POWERUP;

// prototypes
void splash(void);
void instructions(void);
void game(void);
void pause(void);
void gameover(void);
void move_brandon(void);
void capture_npcs(void);
void draw_npcs(void);
void handle_powerups(void);


#endif
