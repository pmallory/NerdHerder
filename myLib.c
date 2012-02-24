/*
 * Philip Mallory
 */

#include "myLib.h"

unsigned short *videoBuffer = (u16 *)0x6000000;
unsigned short *frontBuffer = (u16 *)0x6000000;
unsigned short *backBuffer =  (u16 *)0x600A000;

DMA *dma = (DMA *)0x40000B0;


void setPixel3(int row, int col, unsigned short color)
{
    videoBuffer[OFFSET(row, col, SCREENWIDTH)] = color;
}

void drawRect3(int row, int col, int height, int width, unsigned short color)
{
    unsigned short c = color;
        
    int i;
    for(i = 0; i < height; i++)
    {
        DMANow(3,
                &c,
                &videoBuffer[OFFSET(row+i, col, SCREENWIDTH)],
                (width) | DMA_SOURCE_FIXED);
    }
}

void drawImage3(const unsigned short* image, int row, int col, int height, int width)
{
    int i;
    for(i = 0; i < height; i++)
    {
        DMANow(3,
            (unsigned short*)&image[OFFSET(i,0, width)],
            &videoBuffer[OFFSET(row+i, col, SCREENWIDTH)],
            (width));
    }
}

void fillScreen3(unsigned short color)
{
    unsigned short c = color;

    DMANow(3, &c, videoBuffer, (240*160) | DMA_SOURCE_FIXED);
    
}

void setPixel4(int row, int col, unsigned char colorIndex)
{
	unsigned short pixels = videoBuffer[OFFSET(row, col/2, SCREENWIDTH/2)];

	if(col % 2 == 0) // even
	{
		pixels &= 0xFF << 8;
		videoBuffer[OFFSET(row, col/2, SCREENWIDTH/2)] = pixels | colorIndex;
	}
	else // odd
	{
		pixels &= 0xFF;
		videoBuffer[OFFSET(row, col/2, SCREENWIDTH/2)] = pixels | colorIndex << 8;
	}
}

void drawRect4(int row, int col, int height, int width, unsigned char colorIndex) {
	unsigned short pixels = colorIndex << 8 | colorIndex;

	int r;
	for(r = 0; r < height; r++)	{
		if(col % 2 == 0) { // even starting col
			DMANow(3,
                 &pixels,
                 &videoBuffer[OFFSET(row + r, col/2, SCREENWIDTH/2)],
                 (width/2) | DMA_SOURCE_FIXED);	
			if(width % 2 == 1) { // if width is odd
				setPixel4(row+r, col+width - 1, colorIndex);
			}
		}

		else {  // old starting col
			setPixel4(row+r, col, colorIndex);

			if(width % 2 == 1) { // if width is odd
				DMANow(3,
                    &pixels,
                    &videoBuffer[OFFSET(row + r, (col+1)/2, SCREENWIDTH/2)],
                    (width/2) | DMA_SOURCE_FIXED);
			}

			else {  // width is even
				DMANow(3,
                    &pixels,
                    &videoBuffer[OFFSET(row + r, (col+1)/2, SCREENWIDTH/2)],
                    ((width/2)-1) | DMA_SOURCE_FIXED);
				setPixel4(row+r, col+width - 1, colorIndex);
			}
		}
	}
}

void fillScreen4(unsigned char colorIndex) {
	unsigned short pixels = colorIndex << 8 | colorIndex;
	DMANow(3, &pixels, videoBuffer, ((240 * 160)/2) | DMA_SOURCE_FIXED);
}

void drawBackgroundImage4(const unsigned short* image) {
	DMANow(3, (unsigned short*)image, videoBuffer, ((240 * 160)/2));
}

void drawImage4(const unsigned short* image, int row, int col, int height, int width) {
	if(col%2) 
		col++;

	int r;
	for(r = 0; r < height; r++)	{
		DMANow(3,
               (unsigned short*)&image[OFFSET(r,0,width/2)],
               &videoBuffer[OFFSET(row + r, col/2, SCREENWIDTH/2)],
               width/2);
	}
}

void drawSubImage4(const unsigned short* sourceImage,
                    int sourceRow,
                    int sourceCol,
                    int sourceWidth, 
				    int row,
                    int col,
                    int height,
                    int width) {
	if(sourceCol%2) {
		sourceCol++;
	}
	if(col%2) {
		col++;
	}

	int r;
	for(r = 0; r < height; r++) {
		DMANow(3, 
                (unsigned short*)&sourceImage[OFFSET(sourceRow + r,
                    sourceCol/2, sourceWidth/2)],
                &videoBuffer[OFFSET(row + r, col/2, SCREENWIDTH/2)],
                width/2);
	}
}

void loadPalette(const unsigned short* palette) {
	DMANow(3, (unsigned short*)palette, PALETTE, 256);
}


void DMANow(int channel, void* source, void* destination, unsigned int control) {
	dma[channel].src = source;
	dma[channel].dst = destination;
	dma[channel].cnt = DMA_ON | control;
}

void waitForVblank() {
	while(SCANLINECOUNTER > 160);
	while(SCANLINECOUNTER < 160);
}

void flipPage() {
    if(REG_DISPCTL & BACKBUFFER) {
        REG_DISPCTL &= ~BACKBUFFER;
        videoBuffer = backBuffer;
    }
    else {
        REG_DISPCTL |= BACKBUFFER;
        videoBuffer = frontBuffer;
    }
}

// for sound timing
int vblank_counter = 0;
// sound variables
int soundAStart = 0;    //vblank counter when started playung
int soundALooping = 0;  // is looping?
u8 *soundA;           // pts at sound array
int soundALength = 0;   // length of sound in samples
int soundADuration = 0; // duration in vblanks
int soundAFreq = 0;     // sampling frequency
int soundAPlaying = 0;  // is currently playing?
int soundBStart = 0;    //vblank counter when started playung
int soundBLooping = 0;  // is looping?
char *soundB;           // pts at sound array
int soundBLength = 0;   // length of sound in samples
int soundBDuration = 0; // duration in vblanks
int soundBFreq = 0;     // sampling frequency
int soundBPlaying = 0;  // is currently playing?


void setupSounds() {
    REG_SOUNDCNT_X = SND_ENABLED;

    REG_SOUNDCNT_H = SND_OUTPUT_RATIO_100 | // all the way turnt up!
                     DSA_OUTPUT_RATIO_100 | // 100% channel A volume
                     DSA_OUTPUT_TO_BOTH |   // channel a use both speakers
                     DSA_TIMER0 |           // channel a uses timer0
                     DSA_FIFO_RESET |
                     DSB_OUTPUT_RATIO_100 |
                     DSB_OUTPUT_TO_BOTH |
                     DSB_TIMER1 |
                     DSB_FIFO_RESET;

    REG_SOUNDCNT_L = 0;
}

void playSoundA(const u8* sound, int length, int freq, int isLooping) {
    if (soundAPlaying) {
        // turn off dma and timer
        dma[1].cnt = 0;
        REG_TM0CNT = 0;
    }

    int timerInterval = 16777216 / freq;
    // set up sound on DMA channel 1
    DMANow(1,
           (u32 *)sound,
           (u32 *)REG_FIFO_A,
           DMA_32 |
           DMA_REPEAT |
           DMA_DESTINATION_FIXED |
           DMA_AT_REFRESH |
           DMA_SOURCE_INCREMENT);

    REG_TM0CNT = 0; // turn off
    REG_TM0D = -timerInterval; //counting up to zero
    REG_TM0CNT = TIMER_ON; // turn timer back on

    soundAStart = vblank_counter;
    soundALooping = isLooping;
    soundA = (u8 *)sound;
    soundALength = length; //seconds
    soundADuration = ((length*60)/freq) - ((length/freq)*3)-1; //vbkanks
    soundAFreq = freq;
    soundAPlaying = 1;
}

void setupInterrupts() {
    REG_IME = 0; // turn off interrupts while fiddling with interrupts
    // turn on vblank interrupt
    REG_INTERRUPT = (u32)interruptHandler;
    REG_IE |= INT_VBLANK;
    REG_DISPSTAT |= INT_VBLANK_ENABLE;

    REG_IME = 1;
}

void interruptHandler() {
    REG_IME = 0; // turn of interrupts while fiddling with interrupts

    // is it a vblank interrupt?
    if(REG_IF & INT_VBLANK) {
        vblank_counter++;
        // is soundA at its end?
        if((vblank_counter > soundAStart+soundADuration) && soundAPlaying) {
            //stop it
            dma[1].cnt = 0;
            REG_TM0CNT = 0;
            soundAPlaying = 0;

            //is it looping?  if so play it again!
            if (soundALooping) {
                playSoundA(soundA, soundALength, soundAFreq, soundALooping);
            }
        }
        REG_IF = INT_VBLANK; // look out for next vblank interrupt
    }

    REG_IME = 1; //turn interrupts back on
}

void oam_copy(OBJ_ATTR *dst, const OBJ_ATTR *src, u32 count) {
    u32 *dstw = (u32*)dst, *srcw = (u32*)src;
    while (count--) {
        *dstw++ = *srcw++;
        *dstw++ = *srcw++;
    }
}

extern u16 __key_curr, __key_prev;
u16 __key_curr=0, __key_prev=0;

inline void key_poll() {
    __key_prev= __key_curr;
    __key_curr= ~BUTTON_ADDRESS & KEY_MASK;
}

inline u32 key_curr_state() {return __key_curr;}
inline u32 key_prev_state()  {  return __key_prev;}
inline u32 key_is_down(u32 key) { return __key_curr & key;  }
inline u32 key_is_up(u32 key) {return ~__key_curr & key; }
inline u32 key_was_down(u32 key) { return __key_prev & key; }
inline u32 key_was_up(u32 key)  {   return ~__key_prev & key; } 
inline u32 key_held(u32 key){ return ( __key_curr & __key_prev) & key; }
inline u32 key_hit(u32 key){ return ( __key_curr &~ __key_prev) & key; }
inline u32 key_released(u32 key) { return (~__key_curr & __key_prev) & key; }

