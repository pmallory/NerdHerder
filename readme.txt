             _   _              _ _   _              _           
            | \ | |            | | | | |            | |          
            |  \| | ___ _ __ __| | |_| | ___ _ __ __| | ___ _ __ 
            | . ` |/ _ \ '__/ _` |  _  |/ _ \ '__/ _` |/ _ \ '__|
            | |\  |  __/ | | (_| | | | |  __/ | | (_| |  __/ |   
            \_| \_/\___|_|  \__,_\_| |_/\___|_|  \__,_|\___|_|   
                                                    

Story:
You are Brandon Whitehead, head TA. It is your sworn duty to guide students to
their academic destinies. You must encourage Georgia Tech's best and brightest
to fulfill their potential, while also seeking out those who can't make the cut
and eliminating them.

Your task in NerdHerder is to coral your best students to their dorm (located on
the west edge of campus) so that they'll study, while separating out the
flunkies and sending them to the M-Train (on the east of campus).

The tools at your disposal:
    Intimidation  - You instill fear into the hearts of men. Everyone nearby you
                    flees.
    Mountain Dew  - If you are carrying Mountain Dew (green and red can) nerds
                    will flock to you instead of running. The power-up only
                    lasts a brief time, use it wisely.
    Natural Light - If you are carrying Natty Light (silver and blue can) bros
                    will follow you. The power-up only lasts a brief time, use
                    it wisely.

Get as many nerds to the dorm as possible, and as many bros as possible to the
M-Train. Everyone that winds up in the wrong destination detracts from your
score.

Game Controls:
D-Pad ..... moves Brandon around, step on power up to activate it
Select .... pause/unpause, reset the game if on score screen

Sprites used:
    There are six Brandon sprites (for the different directions and for the 
        walking animation)
    One sprite each for the Nerds and the Bros
    One sprite for Mountain Dew
    One Sprite for Natty Light

DMA use:
    I used DMA to load the bg tiles, the tile map, sprite tiles and palette.
    The spash screen image is also DMA'd and the bg on the instructions and
    score screen is cleared by DMA (they're dine in bitmap mode). I also use
    DMA to keep the sound buffer filled.

Animation:
    Brandon's sprite is animated as it walks. There are separate sprites for
    facing up, down and right. Walking left is done by flipping the sprite in
    hardware. There are also sprites for each part of Brandon's gait, the
    walking sprite is switched out to the next animation frame after five
    pixels of motion. For walking up and down there is only an animation frame
    for lifting Brandon's right leg, the sprite is flipped to do the other half
    of the gait.

How to play:
    Move around the map herding bros and nerds. NPCs are removed from gameplay
    when they are entirely on top of the dorm or M-Train. Pick up a power-up by 
    standing on it. The power-ups only last long enough for you to walk
    across the map, and they can only be used once. You are allowed to walk on
    top of the dorm and M-Train to drive NPCs away from them.

To win/lose:
    The game ends when every bro/nerd has been sent to either the dorm or the
    M-Train. The score screen gives a quantitative and qualitative score.

Extra cool stuff:
    To start a new game from the score screen I used inline assembly to call
    the BIOS's soft reset function. That seems easier than going through and
    reinitializing all the variables and reseeding the RNG and stuff.

    There is background music. It doesn't work for me when using VBA for Linux,
    but it does on Macintosh and Windows. Maybe my VM is screwy? Anyways if it
    doesn't work for you that's something to be aware of. The music loops. The
    timing is done by vblank intrupts and DMA is used to keep the sound buffer
    full.

What does/doesn't work
    Everything in the game works (as far as I know). I would have liked to have
    a power-up sound effect and a visual effect (fade out or mosaic) for when an
    NPC is removed from play, but I didn't have time. Also the game is too easy.

