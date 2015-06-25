#include "includes.prl"
#include <exec/types.h>
#include <exec/memory.h>
#include <dos.h>
#include <graphics/gfxbase.h>
#include <graphics/gfx.h>
#include <hardware/custom.h>

#define BackgroundWidth 100 /*800 pixels (100 bytes) wide. */
#define ForegroundWidth 92  /*736 pixels (92 bytes) wide. */
#define ScreenHeight 192 /*Playing area 192 pixels high. */
#define NumberPlanes 3 /*3 bitplanes per playfield. */

#define BytesPerBackPlane (BackgroundWidth*ScreenHeight)
#define BytesPerForePlane (ForegroundWidth*ScreenHeight)
#define BackgroundMemory (2*NumberPlanes*BytesPerBackPlane)
#define ForegroundMemory (NumberPlanes*BytesPerForePlane)
#define MemNeeded (BackgroundMemory+ForegroundMemory)

#define LeftMouse 64

#define diwstrt 0x8e
#define diwstop 0x90
#define ddfstrt 0x92
#define ddfstop 0x94
#define INTREQ 0x9c
#define bplpt 0xe0
#define bplcon0 0x100
#define bplcon1 0x102
#define bplcon2 0x104
#define bpl1mod 0x108
#define bpl2mod 0x10a
#define sprpt 0x120
#define color 0x180

#define LOWORD(l) ((WORD)(l))
#define HIWORD(l) ((WORD)(((LONG)(l) >> 16) & 0xFFFF))

/* Gameplay */
#define FullFirepower 1

#define TableSize 22

#define aliensize (24<<6)+3

#define firedelay 3

#define missiles1 0
#define missiles2 0x1b0
#define missilemask 0x360
#define mult 0x3a8
#define multmask 0x3c0
#define smallship1 0x3d8
#define smallship2 0x428
#define ship1_1 0x478
#define ship1_2 0x6a8
#define ship2_1 0xb08
#define ship3_1 0x1198
#define ship4_1 0x1828
#define ship4_2 0x1a58
#define ship_explosion 0x1eb8
#define growth 0x2c20
#define outriders 0x2e20

#define explosion1 0
#define guardian_eye1 3456
#define tadpole 4992
#define eye 6528
#define bubble 12288
#define jellyfish1 13824
#define jellyfish2 15360
#define bordertl 16896
#define borderbl 19200
#define bordertr 21504
#define borderbr 23808
#define mouth 26112
#define slime 29184
#define snakebody 32640
#define snakehead 33024

extern struct Path;
extern struct Background;

void waitline223(void);
void checkpf(BOOL pf1);
void resetpf1(void);
void resetpf2(void);
void flipbgnd(void);
void drawfgnds(void);
void drawfgnd(int offset);
void blitfgnd(PLANEPTR plane1,PLANEPTR plane2,PLANEPTR plane3,PLANEPTR src);
void setupblit(void);
void buildbackgnd(void);
void drawback(UBYTE block,UWORD *planeA1,UWORD *planeA2,UWORD *planeB1,UWORD *planeB2);
void GameInit(void);
void clear_screen(void);
void check_collision(void);
void explode_ship(void);
void max_energy(UWORD offset);
void moveship(void);
ULONG xytosprite(UWORD x,UWORD y);
void draw_outrider(void);
void check_firekey(void);
BOOL get_firekey(void);
void decrease_energy(UWORD offset,UWORD energy);
void setup_mouse(void);
void joy(UWORD *up,UWORD *down,UWORD *right,UWORD *left);
void ship_to_copper(void);
void change_ship(void);
void restorebgnds(void);
void savebgnds(UWORD x,UWORD y,UBYTE * buffer,struct Background *screenbgnds);
void drawbobs(struct Background *screenbgnd,UBYTE *alien,UBYTE *mask);
struct Background *getscreeninfo(ULONG *buffer);
void process_aliens(void);
void update_script(struct Path *path,struct CoordPair *script);
struct Path *path_finished(struct Path *path,struct Alien *alien);
void check_seek(struct Path *path,struct CoordPair *script);
void init_pause(struct Path *path,struct CoordPair *script);
void loop_back(struct Path *path,struct CoordPair *script);
void toggle_offset(struct Path *path,struct CoordPair *script);
void seek_mode(struct Path *path,struct CoordPair *script);
void start_seekx(struct Path *path,struct CoordPair *script);
void start_seeky(struct Path *path,struct CoordPair *script);
void copy_coords(struct CoordPair *script);
void change_speed(struct Path *path,struct CoordPair *script);
void change_sprite(struct Path *path,struct CoordPair *script);
void reload_coords(struct Path *path,struct CoordPair *script);
void new_table(struct Path *path,struct CoordPair *script);
void restore_offset(struct Path *path,struct CoordPair *script);
void fire_heatseeker(struct Path *path,struct CoordPair *script);
void change_anim(struct Path *path,struct CoordPair *script);
void restart_table(struct Path *path,struct CoordPair *script);
void start_xy(struct Path *path,struct CoordPair *script);
void save_aliens(void);
void draw_aliens(void);
void blit_to_backgnd(struct Background *screenbgnd,UBYTE *mask);
void blit_to_buffer(UBYTE *buffer,struct Background *screenbgnd);
void coords_to_pf1offsets(ULONG x,ULONG y,UWORD *boffset,UWORD *soffset);
void coords_to_pf2offsets(ULONG x,ULONG y,UWORD *boffset,UWORD *soffset);
BOOL setup_addresses(struct Background *screenbgnd,UWORD size,UWORD soffset,UWORD boffset,
     PLANEPTR *planes);
PLANEPTR *check_end_screen(void);
void check_path(void);
void update_missiles(void);
void kill_alien(UWORD x,UWORD y);
void explode_alien(struct Path *path);
BOOL draw_missile(UWORD x,UWORD y,struct Background *table,UWORD size,UBYTE *buffer,UBYTE *sprite,
     UBYTE *mask);
void or_foregnds(struct Background *table,UWORD size,UWORD boffset,UBYTE *buffer);
void erase_missiles(void);
void CleanExit(BOOL freesystem);
void TakeSystem(void);
void FreeSystem(void);
UBYTE *readFile(char *filename,int *size,ULONG memType);