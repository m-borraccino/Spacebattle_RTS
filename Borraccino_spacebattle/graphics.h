/***************************** graphics.h **********************************/

//-------------------------------------------------------------
// Costants for graphics routines
//-------------------------------------------------------------

//costanti e variabili per disegnare la salute
#define 		UPPER_DIST_H		20
#define 		LAT_DIST_H		20
#define 		HEIGHT_RECT_H		10
#define 		LENGHT_RECT_H		400

int health_points_player[4]={LAT_DIST_H,UPPER_DIST_H+HEIGHT_RECT_H,
			      LAT_DIST_H+LENGHT_RECT_H,UPPER_DIST_H};
int health_points_enemy[4]={XWIN-LENGHT_RECT_H-LAT_DIST_H,UPPER_DIST_H+HEIGHT_RECT_H,
			     XWIN-LAT_DIST_H,UPPER_DIST_H};

//costanti e variabili per disegnare l'energia
#define 		UPPER_DIST_C		35
#define 		LAT_DIST_C		20
#define 		HEIGHT_RECT_C		2
#define 		LENGHT_RECT_C		400

int charge_points_player[4]={LAT_DIST_C,UPPER_DIST_C+HEIGHT_RECT_C,
			      LAT_DIST_C+LENGHT_RECT_C,UPPER_DIST_C};
int charge_points_enemy[4]={XWIN-LENGHT_RECT_C-LAT_DIST_C,UPPER_DIST_C+HEIGHT_RECT_C,
			     XWIN-LAT_DIST_C,UPPER_DIST_C};

//-------------------------------------------------------------
// 	FUNCTIONS
//-------------------------------------------------------------

void countdown(void);
void draw_GO(int millisec);

void update_graphics(void);
void draw_spaceship(void);
void draw_bullet(void);
void draw_health(void);
void draw_charge(void);
void draw_lvl_speed(void);
void draw_pause_screen(void);

void gameover(void);
void draw_statistics(void);

unsigned int angle_float_to_256(float angle);


/***************************** END OF FILE **********************************/
/****************************************************************************/
/* 	SPACEBATTLE GAME, RTS PROJECT, DEVELOPED BY MARCO BORRACCINO        */
/****************************************************************************/
