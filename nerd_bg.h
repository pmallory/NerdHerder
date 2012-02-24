
//{{BLOCK(nerd_bg)

//======================================================================
//
//	nerd_bg, 512x256@8, 
//	+ palette 256 entries, not compressed
//	+ 201 tiles (t|f|p reduced) not compressed
//	+ regular map (in SBBs), not compressed, 64x32 
//	Total size: 512 + 12864 + 4096 = 17472
//
//	Time-stamp: 2011-12-07, 23:32:09
//	Exported by Cearn's GBA Image Transmogrifier, v0.8.3
//	( http://www.coranac.com/projects/#grit )
//
//======================================================================

#ifndef GRIT_NERD_BG_H
#define GRIT_NERD_BG_H

#define nerd_bgTilesLen 12864
extern const unsigned short nerd_bgTiles[6432];

#define nerd_bgMapLen 4096
extern const unsigned short nerd_bgMap[2048];

#define nerd_bgPalLen 512
extern const unsigned short nerd_bgPal[256];

#endif // GRIT_NERD_BG_H

//}}BLOCK(nerd_bg)
