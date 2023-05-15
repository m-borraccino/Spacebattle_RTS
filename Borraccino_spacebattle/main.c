/***************************** main.c **********************************/

#include 	<stdlib.h>
#include 	<stdio.h>
#include 	<pthread.h>
#include 	<sched.h>
#include 	<allegro.h>
#include 	<stdbool.h>
#include 	<math.h>
#include 	<time.h>

#include 	"main.h"
#include	"ptask.h"
#include 	"menu.h"
#include 	"menu.c"
#include 	"game.h"
#include 	"game.c"
#include 	"graphics.h"
#include 	"graphics.c" 

/******************************************************************************/
/******************************* MAIN FUNCTION ********************************/
/******************************************************************************/

int main(){

	init();
	//command task creation
	task_create(command,COMM_TASK,COMM_PERIOD,COMM_DEADLINE,20);	

	while(quit==false){
		
		//menu task creation
		task_create(menu,MENU_TASK,MENU_PERIOD,MENU_DEADLINE,22);
		pthread_join(tid[MENU_TASK],NULL);
			
		if(game_state){
			pause_state = true;
			
			//game tasks creation
			task_create(game,GAME_TASK,GAME_PERIOD,GAME_DEADLINE,22);
			task_create(AI,AI_TASK,AI_PERIOD,AI_DEADLINE,22);
			task_create(graphics,GRAPHICS_TASK,GRAPHICS_PERIOD,GRAPHICS_DEADLINE,22);

			pthread_join(tid[GAME_TASK],NULL);
			pthread_join(tid[AI_TASK],NULL);
			pthread_join(tid[GRAPHICS_TASK],NULL);
			
			//display gameover screen
			gameover();
			
			while((readkey() >> 8) != KEY_ESC);
			
		}
	}	
		
	pthread_join(tid[COMM_TASK],NULL);
	allegro_exit();
	return 0;
}
       
//////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------
//            		 Funzioni di inizializzazione 
//---------------------------------------------------------------------------

//Inzializzo tastiera e funzioni grafiche    
void init(void){

	init_grafics();
	install_keyboard();
}

//Inizializza allegro e lo schermo per la grafica  
void init_grafics(void){

	allegro_init();
	
	set_color_depth(24); 
	set_gfx_mode(GFX_AUTODETECT_WINDOWED, XWIN, YWIN, 0, 0);
	clear_to_color(screen, BKG);
	
	//carico i bitmap da visualizzare
	surface = create_bitmap(XWIN, YWIN);
	planet = load_bitmap(PLANET_PATH, NULL);
	start = load_bitmap(START_PATH, NULL);
	options = load_bitmap(CREDITS_PATH, NULL);
	credits = load_bitmap(OPTIONS_PATH, NULL);
	enemy = load_bitmap(ENEMY_PATH, NULL);
	player = load_bitmap(PLAYER_PATH, NULL);
	bullet_blue = load_bitmap(BULLET1_PATH, NULL);
	bullet_red = load_bitmap(BULLET2_PATH, NULL);
	gameover_screen = load_bitmap(GAMEOVER_PATH, NULL);
	
	green = makecol(0,255,0);
	white = makecol(255,255,255);
	red   = makecol(255,0,0);
}

//---------------------------------------------------------------------------
//            		Task per la lettura dei comandi da tastiera
//---------------------------------------------------------------------------

//funzione del task di interpretazione dei comandi 
void *command(){

    set_activation(COMM_TASK);

    while (quit == false){
    
    		if(menu_state||options_state||credits_state||start_state)
		read_menu_command();	//lettura della pressione e rilascio di un tasto
		
		if(game_state)
		read_game_command();	//lettura dello stato della tastiera
		
        if (deadline_miss(COMM_TASK) == 1) printf("DEADLINE MISS USER COMMAND\n"); //soft real time
        wait_for_activation(COMM_TASK);
    }
}  

//Funzione per la lettura dei comandi all'interno del menu
void read_menu_command(void){

	char key;		//variabile locale in cui salvo il tasto premuto
	if(keypressed()){
		key = readkey() >> 8;
		pthread_mutex_lock(&command_mutex);
		//aggiorno il flag dei tasti disponibili nel menu
		switch (key) {
			case KEY_ESC:
				flag_menu_com |= 0b00000001;
				break;
			case KEY_A:
				flag_menu_com |= 0b10000000;
				break;
			case KEY_LEFT:
				flag_menu_com |= 0b10000000;
				break;
			case KEY_S:
				flag_menu_com |= 0b01000000;
				break;
			case KEY_DOWN:
				flag_menu_com |= 0b01000000;
				break;				
			case KEY_D:
				flag_menu_com |= 0b00100000;
				break;
			case KEY_RIGHT:
				flag_menu_com |= 0b00100000;
				break;			
			case KEY_W: 
				flag_menu_com |= 0b00010000;
				break;
			case KEY_UP: 
				flag_menu_com |= 0b00010000;
				break;
			case KEY_ENTER:
				flag_menu_com |= 0b00001000;
				break;
			default:
				break;
		}
		pthread_mutex_unlock(&command_mutex);
	}
	return;
}

//Funzione per la lettura dei comandi durante il gioco
void read_game_command(void){

	char key_new;		//variabile locale in cui salvo il tasto premuto
	
	pthread_mutex_lock(&command_mutex);
	////aggiorno il flag dei tasti disponibili durante il gioco
	if(key[KEY_A])
		flag_game_com |= 0b10000000;
	if(key[KEY_S])
		flag_game_com |= 0b01000000;
	if(key[KEY_D])
		flag_game_com |= 0b00100000;
	if(key[KEY_W])
		flag_game_com |= 0b00010000;
	if(key[KEY_J])
		flag_game_com |= 0b00001000;
	if(key[KEY_ESC]){
		game_state=false;
		menu_state=true;
	}
	
	if(keypressed()){
		key_new = readkey() >> 8;
		if(key_new == KEY_P)
			pause_state = !pause_state;
		if(key_new == KEY_0)
		view_line = !view_line;
		if(key_new == KEY_1)
			lvl = 1;
		if(key_new == KEY_2)
			lvl = 2;
		if(key_new == KEY_3)
			lvl = 3;
		if(key_new == 65 	 && friction < MAX_FRICTION)	//tasto KEY_PLUS
			friction += INC_FRICTION;
		if(key_new == KEY_MINUS && friction > MIN_FRICTION)
			friction -= INC_FRICTION;	
	}
	
	pthread_mutex_unlock(&command_mutex);
	return;  
}

//---------------------------------------------------------------------------
//            		Task per la gestione del menu
//---------------------------------------------------------------------------

//Funzione del task della gestione del menu
void *menu(){

	struct selector s={1,0}; 
	//s.sel = 1;			// menu selector [1 2 3 4]
	//s.prev_sel = 0;		// last selector position
	
	unsigned char flag;
	start_menu();			//disegno la grafica del menu e del selettore
	draw_selector(s.sel);
	
	set_activation(MENU_TASK);
	
	while (quit == false && game_state == false) {
		
		flag = 0;
		//leggo il flag dei comandi da tastiera
		pthread_mutex_lock(&command_mutex);
		if(flag_menu_com){
			flag = flag_menu_com;
		}
		pthread_mutex_unlock(&command_mutex);
			
		if (flag) {
			
			if (flag & 0b10000000)			// tasto A
				A_function(s, flag);	
			if (flag & 0b01000000)			// tasto S
				s = S_function(s, flag);
			if (flag & 0b00100000)			// tasto D
				D_function(s, flag);
			if (flag & 0b00010000)			// tasto W
				s = W_function(s, flag);
			if (flag & 0b00001000)			// tasto ENTER
				s = enter_function(s,flag);	
			if (flag & 0b00000001)			// tasto ESC
				ESC_function(s,flag);			
		}
		if (deadline_miss(MENU_TASK) == 1) printf("DEADLINE MISS GENERATORE\n");
		wait_for_activation(MENU_TASK);
	}	
}

//---------------------------------------------------------------------------
//            		Task per la gestione del flusso di gioco
//---------------------------------------------------------------------------

//Funzione generale del task di gioco
void *game(){
	//ad inizio partita inizializzo le strutture delle spaceship e dei proiettili
	init_spaceship();	
	init_bullet();		
	
	set_activation(GAME_TASK);
	while (game_state){				//fin quando stiamo giocando..
		
		if(!pause_state){			//se non sono in pausa
			//aggiorno lo stato delle navicelle e controllo se ci sono nuovi comandi
			update_spc_state();
			
			//aggiorno lo stato dei proiettili e controllo se ci sono nuovi comandi  
			update_bullet_state();
			
			//propago la posizione dei proiettili
			propagate_bullet1();
			propagate_bullet2();
			
			//aggiorno le posizioni delle navicelle
			move_spaceship();
			
			//ripristino periodicamente l'energia
			recharge();
		}
		if (deadline_miss(GAME_TASK) == 1) printf("DEADLINE MISS GAME\n");	//soft real time
		wait_for_activation(GAME_TASK);
		
	}
	
}    

//---------------------------------------------------------------------------
//            		Task per la gestione della grafica
//---------------------------------------------------------------------------

//Funzione generale del task che controlla la grafica
void *graphics(){
	
	//questo task viene creato all'inizio di ogni partita, per cui come prima funzione
	//c'è quella che gestisce il countdown di inizio gioco
	countdown();
	
	int millisec = 0;		//inizializzo una variabile per un timer
		
	set_activation(GRAPHICS_TASK);
	while (game_state){		//fin quando sono nella fase di gioco...
		
		
		
		//aggiorno la grafica
		update_graphics();
		
		//Stampo GO solo nei primi secondi dopo l'inizio
		draw_GO(millisec);
		
		//unico blit totale sullo screen
		blit(surface, screen, 0,0,0,0, 1023,767);
							
		if (deadline_miss(GRAPHICS_TASK) == 1) printf("DEADLINE MISS GRAPHICS\n");
		
		wait_for_activation(GRAPHICS_TASK);
		
		millisec += GRAPHICS_PERIOD;	
	}
}

//---------------------------------------------------------------------------
//            		Task per la gestione dell'AI avversaria
//---------------------------------------------------------------------------

//Funzione del task di Intelligenza Artificiale della navicella avversaria
void *AI(){ 
	
	set_activation(AI_TASK);
	while (game_state){		//fin quando sono nella fase di gioco...
		
		if(!pause_state){	//se il gioco non è in pausa...
			
			//il nemico casualmente decide di sparare
			enemy_shot();
			
			//il nemico decide di spostarsi
			new_pos_des();
			
			//compie un'azione se è sottotiro
			under_fired_alert();
		
			//aggiorna lo stato della navicella
			update_enemy_state();
				
			//contatori per la temporizzazione e la generazione di numero random
			count++;
			AI_i++;
		}	
			
			
		
	if (deadline_miss(AI_TASK) == 1) printf("DEADLINE MISS AI\n");
				
	wait_for_activation(AI_TASK);
	}			
}

/***************************** END OF FILE **********************************/
/****************************************************************************/
/* 	SPACEBATTLE GAME, RTS PROJECT, DEVELOPED BY MARCO BORRACCINO        */
/****************************************************************************/


