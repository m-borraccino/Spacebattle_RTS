/***************************** graphics.h **********************************/

/*
	In questo file sono contenute le funzioni che gestiscono la grafica di gioco.
*/

//funzione che si occupa di stampare su schermo il countdown all'inizio di ogni partita
void countdown(void){
		
	blit(planet, surface, 0,0,0,0, planet->w-1,planet->h-1);
	draw_spaceship();
	draw_health();
	draw_charge();
	
	blit(surface, screen, 0,0,0,0, 1023,767);
	
	textout_centre_ex(screen, font, "READY...", XWIN/2, UPPER_DIST_H, red, -1);
	textout_centre_ex(screen, font, "3", XWIN/2, UPPER_DIST_H+20, red, -1);
	sleep(1);	//pausa di 1 secondo
	
	textout_centre_ex(screen, font, "READY...", XWIN/2, UPPER_DIST_H, makecol(255,255,0), -1);
	blit(planet, screen, XWIN/2-10, UPPER_DIST_H+20,XWIN/2-5, UPPER_DIST_H+20, 10, 10);
	textout_centre_ex(screen, font, "2", XWIN/2, UPPER_DIST_H+20, makecol(255,255,0), -1);
	sleep(1);	//pausa di 1 secondo
	
	textout_centre_ex(screen, font, "READY...", XWIN/2, UPPER_DIST_H, green, -1);
	blit(planet, screen, XWIN/2-10, UPPER_DIST_H+20,XWIN/2-5, UPPER_DIST_H+20, 10, 10);
	textout_centre_ex(screen, font, "1", XWIN/2, UPPER_DIST_H+20, green, -1);
	sleep(1);	//pausa di 1 secondo
	
	pause_state = false;
}

//funzione che in base al tempo da inizio gioco stampa su schermo la scritta GO!
void draw_GO(int millisec){

	if(millisec <= 1000)
			textout_centre_ex(surface, font, "GO!", XWIN/2, UPPER_DIST_H, 						makecol(0,255,0), -1);
}

// funzione generale che aggiorna le immagini sullo schermo
void update_graphics(void){
			
	blit(planet, surface, 0,0,0,0, planet->w-1,planet->h-1);	//disegna lo sfondo
	
	draw_spaceship();		//disegna le navicelle
	
	draw_bullet();			//disegna i proiettili
	
	draw_health();			//disegna le barre della salute
	draw_charge();			//disegna le barre dell'energia
	
	if(pause_state)
		draw_pause_screen();	//scrive Pausa se siamo in pausa
	
	draw_lvl_speed();		//scrive livello e attrito in basso	
}

//disegna le navicelle su schermo
void draw_spaceship(void){
	float vel_angle;
	
	pthread_mutex_lock(&player_spc.mtx);
	//se la modalità view line è attiva allora stampo linee guida aggiuntive
	if(view_line){
		vel_angle = atan2(-player_spc.vely,player_spc.velx);
		//cerchio attorno alla navicella 
		circle(surface,player_spc.x, player_spc.y, 50,makecol(0,0,100));
		//linee per la direzione della mira
		line(surface,player_spc.x,player_spc.y,
		     player_spc.x+300*cos(player_spc.angle+0.1),	
		     player_spc.y-300*sin(player_spc.angle+0.1),makecol(0,0,100));
		line(surface,player_spc.x,player_spc.y,
		     player_spc.x+300*cos(player_spc.angle-0.1),
		     player_spc.y-300*sin(player_spc.angle-0.1),makecol(0,0,100));
		//linea con direzione e intensità del vettore velocità
		line(surface,
		     player_spc.x+50*cos(vel_angle),
		     player_spc.y-50*sin(vel_angle),
		     player_spc.x+50*cos(vel_angle)+3*player_spc.velx,
		     player_spc.y-50*sin(vel_angle)+3*player_spc.vely, makecol(180,180,180));
	}
	//disegno le navicelle
	pivot_scaled_sprite(surface, player, player_spc.x, player_spc.y, 100,102,
                    itofix(angle_float_to_256(player_spc.angle)),SCALE_FACTOR);
	pthread_mutex_unlock(&player_spc.mtx);
	
	pthread_mutex_lock(&enemy_spc.mtx);
	pivot_scaled_sprite(surface, enemy, enemy_spc.x, enemy_spc.y, 100,102,
                     itofix(angle_float_to_256(enemy_spc.angle)), SCALE_FACTOR);
	if(view_line){
	
		vel_angle = atan2(-enemy_spc.vely,enemy_spc.velx);
		//cerchio attorno alla navicella 
		circle(surface,enemy_spc.x, enemy_spc.y, 50,makecol(100,0,0));
		//linee per la direzione della mira    		
		line(surface,enemy_spc.x,enemy_spc.y,true_x,true_y, makecol(100,0,0));
		//cerchio sulla posizione desiderata
		circle(surface,x_des, y_des, 30, white);
	} 
	pthread_mutex_unlock(&enemy_spc.mtx);	
}

//funzione che disegna i proiettili attivi sullo schermo
void draw_bullet(void){
	
	int i;

	for(i=0; i <= BULLET1_MAX; i++){
		if(bullet_1[i].state == true)
		pivot_scaled_sprite(surface, bullet_blue, bullet_1[i].x, bullet_1[i].y, 5,4,
				     itofix(angle_float_to_256(bullet_1[i].angle)),1.5*SCALE_FACTOR);
	}
	
	for(i=0; i <= BULLET2_MAX; i++){
		if(bullet_2[i].state == true)
		pivot_scaled_sprite(surface, bullet_red, bullet_2[i].x, bullet_2[i].y, 5,4,
				     itofix(angle_float_to_256(bullet_2[i].angle)),1.5*SCALE_FACTOR);
	}	
}

//funzione che disegna la barra della salute
void draw_health(void){
	
	int col, health;
	char s[10];
			
	pthread_mutex_lock(&player_spc.mtx);
	health = player_spc.health;
	pthread_mutex_unlock(&player_spc.mtx);

	health_points_player[2]= LAT_DIST_H + health*LENGHT_RECT_H/START_HEALTH;
	col=makecol(255*(1-health/START_HEALTH),255*health/START_HEALTH,0);
	
	rectfill(surface,
		 health_points_player[0],health_points_player[1],
	 	 health_points_player[2],health_points_player[3],
	 	 col);
 	rect(surface,
 	     LAT_DIST_H,UPPER_DIST_H+HEIGHT_RECT_H,
	     LAT_DIST_H+LENGHT_RECT_H,UPPER_DIST_H,
	     col);	
 	sprintf(s,"%d",health);
 	textout_centre_ex(surface, font, s, LAT_DIST_H+LENGHT_RECT_H+15, UPPER_DIST_H+2, col, -1);

	
	pthread_mutex_lock(&enemy_spc.mtx);
	health = enemy_spc.health;
	pthread_mutex_unlock(&enemy_spc.mtx);
		
	health_points_enemy[0]= XWIN-LAT_DIST_H - health*LENGHT_RECT_H/START_HEALTH;
	col=makecol(255*(1-health/START_HEALTH),255*health/START_HEALTH,0);
	
	rectfill(surface,
		 health_points_enemy[0],health_points_enemy[1],
	 	 health_points_enemy[2],health_points_enemy[3], 
	 	 col);
	rect(surface,
	     XWIN-LENGHT_RECT_H-LAT_DIST_H,UPPER_DIST_H+HEIGHT_RECT_H,
             XWIN-LAT_DIST_H,UPPER_DIST_H,
             col);
             
	sprintf(s,"%d",health);
 	textout_centre_ex(surface, font, s, XWIN-LAT_DIST_H-LENGHT_RECT_H-15, UPPER_DIST_H+2,col, -1);
}

//funzione che disegna la barra della carica
void draw_charge(void){
	
	int col, charge;
	char s[10];
	col=makecol(0,0,255);
			
	pthread_mutex_lock(&player_spc.mtx);
	charge = player_spc.charge;
	pthread_mutex_unlock(&player_spc.mtx);

	charge_points_player[2]= LAT_DIST_C + charge*LENGHT_RECT_C/START_CHARGE;
	
	rectfill(surface,
		 charge_points_player[0],charge_points_player[1],
	 	 charge_points_player[2],charge_points_player[3],
	 	 col);
 		
	pthread_mutex_lock(&enemy_spc.mtx);
	charge = enemy_spc.charge;
	pthread_mutex_unlock(&enemy_spc.mtx);
		
	charge_points_enemy[0]= XWIN-LAT_DIST_C - charge*LENGHT_RECT_C/START_CHARGE;

	rectfill(surface,
		 charge_points_enemy[0],charge_points_enemy[1],
	 	 charge_points_enemy[2],charge_points_enemy[3], 
	 	 col);
}

//funzione che disegna informazioni in basso riguardo il livello e l'attrito
void draw_lvl_speed(void){
	
	char s[20];
	int width;
	
	
	textout_ex(surface, font, "To view something press [0]", 20, YWIN-60, white, -1);	
	textout_ex(surface, font, "To set LEVEL press:", 20, YWIN-40, white, -1);
	width = text_length(font, "To set LEVEL press:");
	textout_ex(surface, font, " [1]   Easy"   , width+30, YWIN-40, white, -1);
	textout_ex(surface, font, " [2]  Normal"  , width+30, YWIN-30, white, -1);
	textout_ex(surface, font, " [3] Difficult", width+30, YWIN-20, white, -1);
	
	width = text_length(font, "To PAUSE game press [P]          ");
	textout_ex(surface, font, "To PAUSE game press [P]          ", XWIN-width-10, YWIN-60, white, -1);
	width = text_length(font, "To set FRICTION press: [+] +0.05 ");
	textout_ex(surface, font, "To set FRICTION press:", XWIN-width-10, YWIN-40, white, -1);
	width = text_length(font, " [+] +0.05 ");
	textout_ex(surface, font, " [+] +0.05 "  , XWIN-width-10, YWIN-40, white, -1);
	textout_ex(surface, font, " [-] -0.05 "  , XWIN-width-10, YWIN-30, white, -1);
		
	textout_centre_ex(surface, font, "LEVEL", XWIN/2-50, YWIN-35, white, -1);
	
	switch(lvl){
		case 1:
			textout_centre_ex(surface, font, "Easy",
					   XWIN/2-50, YWIN-25, green, -1);
			break;
		case 2:
			textout_centre_ex(surface, font, "Normal",
					   XWIN/2-50, YWIN-25, white, -1);
			break;
		case 3:
			textout_centre_ex(surface, font, "Expert",
					   XWIN/2-50, YWIN-25, red, -1);
			break;
		default:
			break;
	}
	
	sprintf(s,"%.2f",friction);
	textout_centre_ex(surface, font, "FRICTION", XWIN/2+50, YWIN-35 , makecol(255,255,255), -1);
 	textout_centre_ex(surface, font, s, XWIN/2+50, YWIN-25 , makecol(255,255,255), -1);	
}

//Questa funzione stampa la parola Pausa
void draw_pause_screen(void){

	textout_centre_ex(surface, font, "PAUSE", XWIN/2, YWIN/2, red, -1);
}

//Funzione generale della schermata di gameover
void gameover(void){
	
	gameover_screen = load_bitmap(GAMEOVER_PATH, NULL); //carica lo sfondo di gameover
	blit(gameover_screen, surface, 0,0,0,0, gameover_screen->w-2,gameover_screen->h-2);
	
	//scrive le statistiche
	draw_statistics();
	
	//unico blit finale
	blit(surface, screen, 0,0,0,0, 1023,767);
}


//Questa funzione stampa le statistiche a fine partita sulla schermata di gameover
void draw_statistics(void){
	
	char s[36];

	sleep(1);
		
	if(enemy_spc.health == 0)		
		textout_centre_ex(surface, font, "YOU WIN", XWIN/2 , 333 , green, -1);
	else
		textout_centre_ex(surface, font, "YOU LOSE", XWIN/2 , 333 , red, -1);
	
	textout_centre_ex(surface, font, "Player...", XWIN/4 , 400 , white, -1);
	textout_centre_ex(surface, font, "Enemy...", XWIN*3/4 , 400 , white, -1);
	sprintf(s,"Health        ->  %d/100",player_spc.health);
 	textout_ex(surface, font, s, XWIN/4-80 ,430 , white, -1);
 	sprintf(s,"Bullets Fired ->  %d    ",shot_bul1);
 	textout_ex(surface, font, s, XWIN/4-80 ,490 , white, -1);
 	sprintf(s,"Bullets Hit   ->  %d    ",hit_bul1);
 	textout_ex(surface, font, s, XWIN/4-80 ,520 , white, -1);
 	sprintf(s,"Health        ->  %d/100",enemy_spc.health);
 	textout_ex(surface, font, s, XWIN*3/4-80 ,430 , white, -1);
 	sprintf(s,"Bullets Fired ->  %d    ",shot_bul2);
 	textout_ex(surface, font, s, XWIN*3/4-80 ,490 , white, -1);
 	sprintf(s,"Bullets Hit   ->  %d    ",hit_bul2);
 	textout_ex(surface, font, s, XWIN*3/4-80 ,520 , white, -1);
 	textout_centre_ex(surface, font, "Press ESC to continue", XWIN/2 , 650 , white, -1);
	
	switch(lvl){
		case 1:
			textout_centre_ex(surface, font, "Level: Easy",
					   XWIN/2, 550, green, -1);
			break;
		case 2:
			textout_centre_ex(surface, font, "Level: Normal",
					   XWIN/2, 550, white, -1);
			break;
		case 3:
			textout_centre_ex(surface, font, "Level: Expert",
					   XWIN/2, 550, red, -1);
			break;
		default:
			break;
	}
}

//trasforma gli angoli da rad a un numero fino a 256 per le funzioni di grafica
unsigned int angle_float_to_256(float angle) {
    return (64 - (unsigned int)round(128 * (angle / M_PI))) % 256;	
}

/***************************** END OF FILE **********************************/
/****************************************************************************/
/* 	SPACEBATTLE GAME, RTS PROJECT, DEVELOPED BY MARCO BORRACCINO        */
/****************************************************************************/
