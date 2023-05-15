/***************************** game.h **********************************/

//-------------------------------------------------------------
// Costanti e parametri per il flusso di gioco
//-------------------------------------------------------------

#define 		PLAYER_SPC		0
#define 		ENEMY_SPC		1

#define 		FORWARD		0
#define 		BACKWARD		1

//margini della schermata di gioco
#define		MARGIN_BOUND		50		//margine laterale
#define		UPPER_MARGIN_BOUND	50+35+2	//margine superiore e inferiore
#define		BUL_MARGIN_BOUND	30		//margine valido per i proiettili

//valore della costante di tempo delta_t per i calcoli di posizione e velocità
#define		DELTA_T		GAME_PERIOD/100

#define		UNIT_ANGLE_ROTATION	0.1	//unità di rotazione per comando da tastiera	
#define		INC_VELOCITY		3	//incremento di velocità per comando
#define		MAX_VELOCITY		30	//velocità massima

#define		START_HEALTH		100	//salute iniziale
#define		START_CHARGE		100	//carica iniziale

#define 		OFFSET_BUL		30	//costante per la posizione dei proiettili
#define 		HIT_SPC_AREA		30	//valore per determinare l'area di impatto
							//tra proiettile e navicella 
							
#define 		BULLET1_MAX		200			//numero max proettili palyer
#define 		BUL1_VEL		(4*MAX_VELOCITY)	//vel max proiettili player
#define 		BULLET2_MAX		200			//numero max proettili nemico
#define 		BUL2_VEL		(3*MAX_VELOCITY)	//vel max proiettili nemico

#define 		BUL1_CHARGE		5	//carica per colpo del giocatore
#define		PLY_FIRE_ANGLE_ERR	5	//errore di mira del giocatore [1-10]

#define 		PLY_RECHARGE_RATE	1	//velocità di ricarica player
#define 		ENY_RECHARGE_RATE	1	//velocità di ricarica nemico

#define 		BUL2_CHARGE		5	//carica per colpo del giocatore
#define 		SHOT_RATE		0.3	//veloctà di sparo nemica [0..1]
#define 		RADAR_ERROR		80	//errore addizionale per l'acquizsizione
							//della posizione del player per la mira 
							//del nemico [+... ...-] error pixels

#define		KP_POS_CONTROLLER	0.3	//costante del controllore di posizione nemica
#define 		POS_DES_ERROR		20	//pixel range per raggiungere la pos des
#define 		UNDER_FIRE_ANG_ERROR	0.2	//errore per sapere se il nemuco è sottotiro
#define		NEW_POS_RATE		90	//velocità di aggiornamento della pos nemica
#define 		NEW_POS_ALERT		30	//velocità di aggiornamento della pos nemica
							//quando il nemico è sottotiro
													
//friction constant
#define 		MAX_FRICTION		1.49
#define 		MIN_FRICTION		0.11
#define 		INC_FRICTION		0.05

//bitmap paths 
#define ENEMY_PATH		"img/eny_spc.bmp"
#define PLAYER_PATH		"img/ply_spc.bmp"
#define BULLET1_PATH		"img/bullet1.bmp"
#define BULLET2_PATH		"img/bullet2.bmp"

//-------------------------------------------------------------
// GLOBAL VARIABLES
//-------------------------------------------------------------

BITMAP *enemy;
BITMAP *player;
BITMAP *bullet_blue;
BITMAP *bullet_red;
BITMAP *surface;

float friction = 0.1;	//attrito per ridurre la velocità nel tempo delle navicelle

//struttura principale delle navicelle
struct spaceship{
	int x, y;		//posizione x y sullo schermo
	float velx, vely;	//componenti x y della velocità
	float angle;		//angolo di orientamento della navicella
	int health;		//salute della navicella
	int charge;		//carica disponibile
	pthread_mutex_t mtx;	//semaforo per la protezione e sincronizzazione dei dati 
} player_spc, enemy_spc;

//struttura dei proiettili del player
struct bullet1{
	bool state;		//stato del proiettile: attivo o non attivo
	int damage;		//danno associato ad ogni proettile
	int x, y;		//posizione x y sullo schermo
	float velx, vely;	//componenti x y della velocità
	float angle;		//angolo di orientamento del proiettile
	pthread_mutex_t mtx;	//semaforo per la protezione e sincronizzazione dei dati 
}bul1;

//struttura dei proiettili del nemico
struct bullet2{
	bool state;		//stato del proiettile: attivo o non attivo
	int damage;		//danno associato ad ogni proettile
	int x, y;		//posizione x y sullo schermo
	float velx, vely;	//componenti x y della velocità
	float angle;		//angolo di orientamento del proiettile
	pthread_mutex_t mtx;	//semaforo per la protezione e sincronizzazione dei dati 
}bul2;

struct bullet1 bullet_1[BULLET1_MAX]; //def struttura di tutti i proiettili del player
int idx_bul1	=	0;		//indice dei proiettili del player su schermo
struct bullet2 bullet_2[BULLET2_MAX];	//def struttura di tutti i proiettili del nemico
int idx_bul2	=	0;		//indice dei proiettili del nemico su schermo

//indici per le statistiche
int shot_bul1	=	0;	//proiettili del player sparati
int hit_bul1	=	0;	//proiettili del player che hanno colpito il nemico
int shot_bul2	=	0;	//proiettili del nemico sparati
int hit_bul2	=	0;	//proiettili del nemico che hanno colpito il player

//variabili globali per le funzioni dell'AI
double	AI_i 		= 0;	//indice utilizzato per la creazione di numeri random
int 	count		= 80;	//indice per la temporizzazione della scelta di generare
				//nuove posizioni desiderate
int 	ready		= 1;    //indica se bisogna generare una nuova posizione (1) o no (0)

//coordinate della posizione desiderata
int x_des	=	XWIN*3/4;
int y_des	=	YWIN/2;
//coordinate della posizione del player per la mira del nemico
int true_x	=	XWIN/4;
int true_y	=	YWIN/2;

//-------------------------------------------------------------
// 	FUNCTIONS
//-------------------------------------------------------------

void init_spaceship(void);
void update_spc_state(void);
void rotate_spc(char spc, float angle);
void move_forward(char spc, char dir);
void move_spaceship(void);

void init_bullet(void);
void update_bullet_state(void);
void create_new_bullet1(void);
void propagate_bullet1(void);
void propagate_bullet2(void);
void recharge(void);

void enemy_shot (void);
void create_new_bullet2(void);
float compute_sight(void);
void new_pos_des(void);
void update_enemy_state(void);
void under_fired_alert(void);

/***************************** END OF FILE **********************************/
/****************************************************************************/
/* 	SPACEBATTLE GAME, RTS PROJECT, DEVELOPED BY MARCO BORRACCINO        */
/****************************************************************************/
