/***************************** menu.c **********************************/
/*
	In questo file sono contenute le funzioni che gestiscono la 
	schermata del menu principale, complete di grafica e gestione 
	dei comandi da tastiera.
*/

#include "menu.h"

//Funzione che stampa l'immagine e le opzioni del menu principale
void start_menu(void){

	clear_to_color(screen, BKG);
	blit(planet, screen, 0,0,0,0, planet->w-1,planet->h-1);
	
	textout_centre_ex(screen, font, "START GAME", XWIN/2, Y_startgame, white, -1);
	textout_centre_ex(screen, font, "OPTIONS", XWIN/2, Y_options, white, -1);
	textout_centre_ex(screen, font, "CREDITS", XWIN/2, Y_credits, white, -1);
	textout_centre_ex(screen, font, "QUIT", XWIN/2, Y_quit, white, -1);
	textout_centre_ex(screen, font, "Press [ENTER] to select", XWIN-110, Y_quit, white, -1);
	
	clear_keybuf();
}

//Funzione che disegna la posizione del selettore 
void draw_selector(int sel){

	int height = text_height(font);
	int width;

	switch(sel){
		
		case 1:		//selettore su start game
			width = text_length(font, "START GAME");	
			triangle(screen, XWIN/2-width/2-15, Y_startgame+height-2, XWIN/2-width/2-15, Y_startgame-2, XWIN/2-width/2-8, Y_startgame+height/2-2 , white);
			break;
			
		case 2:		//selettore su options
			width = text_length(font, "OPTIONS");
			triangle(screen, XWIN/2-width/2-15, Y_options+height-2, XWIN/2-width/2-15, Y_options-2, XWIN/2-width/2-8, Y_options+height/2-2 , white);
			break;
			
		case 3:		//selettore su credits
			width = text_length(font, "CREDITS");
			triangle(screen, XWIN/2-width/2-15, Y_credits+height-2, XWIN/2-width/2-15, Y_credits-2, XWIN/2-width/2-8, Y_credits+height/2-2 , white);
			break;
			
		case 4:		//selettore su quit
			width = text_length(font, "QUIT");
			triangle(screen, XWIN/2-width/2-15, Y_quit+height-2, XWIN/2-width/2-15, Y_quit-2, XWIN/2-width/2-8, Y_quit+height/2-2 , white);
			break;
		
		case 5:		//selettore per la difficoltà
			width = text_length(font, "level1");
			triangle(screen, 
			XWIN*2/3-width/2-15, YWIN*2/3+20+height-2, 
			XWIN*2/3-width/2-15, YWIN*2/3+20-2, 
			XWIN*2/3-width/2-23, YWIN*2/3+20+height/2-2,
			white);
			
			triangle(screen, XWIN*2/3+width/2+15, YWIN*2/3+20+height-2, XWIN*2/3+width/2+15, YWIN*2/3+20-2, XWIN*2/3+width/2+23, YWIN*2/3+20+height/2-2 , white);
			break;
		default:
			break;	
	}	
}

//funzione che cancella il selettore sulla posizione precedente
void delete_selector(int prev_sel){

	int height;
	int width;

	switch(prev_sel){
		
		case 1:		//cancello selettore su start game
			height = text_height(font);
			width = text_length(font, "START GAME");
			blit(planet, screen, XWIN/2-width/2-15, Y_startgame-2,XWIN/2-width/2-15, Y_startgame-2, 8, height+1);
			break;
			
		case 2:		//cancello selettore su options
			height = text_height(font);
			width = text_length(font, "OPTIONS");
			blit(planet, screen, XWIN/2-width/2-15, Y_options-2,XWIN/2-width/2-15, Y_options-2, 8, height+1);
			break;
			
		case 3:		//cancello selettore su credits
			height = text_height(font);
			width = text_length(font, "CREDITS");
			blit(planet, screen, XWIN/2-width/2-15, Y_credits-2,XWIN/2-width/2-15, Y_credits-2, 8, height+1);
			break;
			
		case 4:		//cancello selettore su quit
			height = text_height(font);
			width = text_length(font, "QUIT");
			blit(planet, screen, XWIN/2-width/2-15, Y_quit-2,XWIN/2-width/2-15, Y_quit-2, 8, height+1);
			break;
		default:
			break;	
	}	
}

//Funzione che gestisce il comando ENTER in base alla posizione del selettore
struct selector enter_function(struct selector s,unsigned char flag){
	
	if (start_state){		//se sono nella schermata di start game, allora 
		game_state = true;	//passo nella schermata di gioco
		start_state = false;
	}
	if(menu_state){		//se sono nel menu, navigo in base alla posizione 	
		switch(s.sel){		//del selettore
			case 4:
				quit=true;
				break;
			case 1:
				start_screen();
				start_state=true;
				menu_state=false;
				break;
			case 2:
				options_screen();
				options_state=true;
				menu_state=false;
				break;				
			case 3:
				credits_screen();
				credits_state=true;
				menu_state=false;
				break;
			default:
				break;		
		}
	}
	pthread_mutex_lock(&command_mutex);
	flag &= 0b11110111;
	flag_menu_com &= 0b11110111;		//ripristino il flag	
	pthread_mutex_unlock(&command_mutex);
	return s;
}

//Funzione che gestisce il comando A
void A_function(struct selector s,unsigned char flag){
	
	if(start_state){		
		if(lvl>1) lvl--;		//abbasso il livello
		draw_difficulty(lvl);		
	}
	
	pthread_mutex_lock(&command_mutex);
	flag &= 0b01111111;
	flag_menu_com &= 0b01111111;		//ripristino il flag		
	pthread_mutex_unlock(&command_mutex);
}

//Funzione che gestisce il comando S
struct selector S_function(struct selector s,unsigned char flag){

	if(menu_state){			//aggiorno la posizione del selettore
	if(s.sel<4){
		s.prev_sel=s.sel;
		s.sel++;
	}
	draw_selector(s.sel);
	delete_selector(s.prev_sel);
	
	pthread_mutex_lock(&command_mutex);
	flag &= 0b10111111;
	flag_menu_com &= 0b10111111;		//ripristino il flag		
	pthread_mutex_unlock(&command_mutex);
	}
	return s;
}

//Funzione che gestisce il comando D
void D_function(struct selector s,unsigned char flag){

	if(start_state){
		if(lvl<3) lvl++;		//aumento il livello
		draw_difficulty(lvl);		
	}

	pthread_mutex_lock(&command_mutex);
	flag &= 0b11011111;
	flag_menu_com &= 0b11011111;		//ripristino il flag		
	pthread_mutex_unlock(&command_mutex);
}

//Funzione che gestisce il comando W
struct selector W_function(struct selector s,unsigned char flag){

	if(menu_state){			//aggiorno la posizione del selettore
	if(s.sel>1){
		s.prev_sel=s.sel;
		s.sel--;	
	}
	draw_selector(s.sel);
	delete_selector(s.prev_sel);
	
	pthread_mutex_lock(&command_mutex);
	flag &= 0b11101111;
	flag_menu_com &= 0b11101111;		//ripristino il flag		
	pthread_mutex_unlock(&command_mutex);
	}
	return s;
}

//Funzione che gestisce il comando ESC
void ESC_function(struct selector s,unsigned char flag){
				
	if(options_state||credits_state||start_state){	//ritorno nel menu principale
		start_menu();
		draw_selector(s.sel);
		menu_state=true;
		start_state=false;
		options_state=false;
		credits_state=false;
	}
	pthread_mutex_lock(&command_mutex);
	flag &= 0b11111110;
	flag_menu_com &= 0b11111110;		//ripristino il flag		
	pthread_mutex_unlock(&command_mutex);
}

//Funzione che gestisce la schermata di OPTIONS
void options_screen(void){

	clear_to_color(screen, BKG);
	blit(options, screen, 0,0,0,0, options->w-1,options->h-1);
	
}
//Funzione che gestisce la schermata di CREDITS
void credits_screen(void){

	clear_to_color(screen, BKG);
	blit(credits, screen, 0,0,0,0, credits->w-1,credits->h-1);
	
}
//Funzione che gestisce la schermata di START GAME
void start_screen(void){
	
	clear_to_color(screen, BKG);
	blit(start, screen, 0,0,0,0, planet->w-1,planet->h-1);
	
	textout_centre_ex(screen, font, "Select difficulty:", XWIN*2/3, YWIN*2/3, white, -1);
	
	draw_difficulty(lvl);
	
	draw_selector(5);
}

//Funzione che stampa la difficoltà nella schermata di START GAME
void draw_difficulty(int lvl){
	
	int height = text_height(font);
	int width = text_length(font, "Levelx");
	blit(planet, screen, XWIN*2/3-width/2, YWIN*2/3+20,XWIN*2/3-width/2, YWIN*2/3+20, width, height+1);
	
	switch(lvl){
		case 1:
			textout_centre_ex(screen, font, "Easy", XWIN*2/3, YWIN*2/3+20, green, -1);
			break;
		case 2:
			textout_centre_ex(screen, font, "Normal", XWIN*2/3, YWIN*2/3+20, white, -1);
			break;
		case 3:
			textout_centre_ex(screen, font, "Expert", XWIN*2/3, YWIN*2/3+20, red, -1);
			break;
		default:
			break;
	}
}

/***************************** END OF FILE **********************************/
/****************************************************************************/
/* 	SPACEBATTLE GAME, RTS PROJECT, DEVELOPED BY MARCO BORRACCINO        */
/****************************************************************************/
