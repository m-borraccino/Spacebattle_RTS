/***************************** menu.h **********************************/

//-------------------------------------------------------------
// GLOBAL CONSTANTS
//-------------------------------------------------------------

//Y coordinate for menu graphics
#define 	Y_startgame   		534
#define 	Y_options		564
#define 	Y_credits		594
#define 	Y_quit			624

//bitmap paths 	
#define PLANET_PATH		"img/planet.bmp"
#define CREDITS_PATH		"img/options.bmp"
#define OPTIONS_PATH		"img/credits.bmp"
#define START_PATH		"img/start.bmp"
#define ENEMY_PATH		"img/eny_spc.bmp"
#define PLAYER_PATH		"img/ply_spc.bmp"
#define GAMEOVER_PATH		"img/gameover.bmp"

#define 	SCALE_FACTOR		28000		//fattore di scala per le immagini

//-------------------------------------------------------------
// GLOBAL VARIABLES
//-------------------------------------------------------------

BITMAP *planet;		
BITMAP *credits;
BITMAP *options;
BITMAP *start;
BITMAP *gameover_screen;

int green, white, red;

//-------------------------------------------------------------
// FUNCTIONS
//-------------------------------------------------------------

void start_menu(void);
void draw_selector(int sel);
void delete_selector(int prev_sel);

struct selector enter_function(struct selector s,unsigned char flag);
void A_function(struct selector s,unsigned char flag);
struct selector S_function(struct selector,unsigned char flag);
void D_function(struct selector s,unsigned char flag);
struct selector W_function(struct selector,unsigned char flag);
void ESC_function(struct selector s,unsigned char flag);

void options_screen(void);
void credits_screen(void);
void start_screen(void);

void draw_difficulty(int lvl);

/***************************** END OF FILE **********************************/
/****************************************************************************/
/* 	SPACEBATTLE GAME, RTS PROJECT, DEVELOPED BY MARCO BORRACCINO        */
/****************************************************************************/
