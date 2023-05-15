/***************************** main.h **********************************/

//-------------------------------------------------------------
// GLOBAL CONSTANTS
//-------------------------------------------------------------

#define 		XWIN   		1024 	// window x resolution
#define 		YWIN   		768   	// window y resolution
#define 		BKG    		0    	// background color (black)

// TASKS IDENTIFIER MACRO
#define		COMM_TASK		0
#define		MENU_TASK		1
#define		GAME_TASK		2
#define		AI_TASK		3
#define		GRAPHICS_TASK		4

// TASKS PERIOD MACRO
#define		COMM_PERIOD		30
#define 		MENU_PERIOD		30
#define 		GAME_PERIOD		30
#define 		AI_PERIOD		30
#define 		GRAPHICS_PERIOD	20

// TASKS DEADLINE MACRO
#define		COMM_DEADLINE		30
#define 		MENU_DEADLINE		30
#define 		GAME_DEADLINE		30
#define 		AI_DEADLINE		30
#define 		GRAPHICS_DEADLINE	20

//	mutex
pthread_mutex_t 	command_mutex 	= 	PTHREAD_MUTEX_INITIALIZER;

//-------------------------------------------------------------
// GLOBAL VARIABLES
//-------------------------------------------------------------

//Struct per la visualizzazione del selettore nelle schermate del menu
struct selector{
	int sel;			//posizione attuale del selettore
	int prev_sel;			//ultima posizione del selettore prima di quella attuale
};

bool		quit		= false;	//end flag, usciamo dal programma
bool 		menu_state	= true;	//indica se siamo o meno nel menu principale
bool 		start_state	= false;	//indica se siamo o meno nel menu di start
bool 		options_state	= false;	//indica se siamo o meno nella schermata di options
bool 		credits_state	= false;	//indica se siamo o meno nella schemata di credits
bool 		game_state	= false;	//indica se siamo o meno nella schermata di gioco
bool 		pause_state	= false;	//indica se siamo o meno in pausa

//queste variabili sono usate come una serie da 8bit di flag,
//ognuna di queste si riferisce ad un determinato tasto.
unsigned char 	flag_menu_com	= 0;
unsigned char 	flag_game_com	= 0;

unsigned char 	lvl 		= 1;		//livello di difficoltà della partita 1-3
bool		view_line	= false;	//flag per visualizzare o meno linee di supporto 
						//durante il gioco

//-------------------------------------------------------------
// TASK AND OTHER FUNCTIONS
//-------------------------------------------------------------

void *command();
void *menu();
void *game();
void *graphics();
void *AI();

void init(void);
void ptask_init(void);
void read_game_command(void);
void read_menu_command(void);
void init_grafics(void);

/***************************** END OF FILE **********************************/
/****************************************************************************/
/* 	SPACEBATTLE GAME, RTS PROJECT, DEVELOPED BY MARCO BORRACCINO        */
/****************************************************************************/
