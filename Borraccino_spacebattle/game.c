/***************************** game.c **********************************/
/*
	In questo file sono contenute le funzioni che gestiscono l'intero 
	flusso delle funzioni in modalità gioco.
	L'ordine di esecuzione è: (funzione game())
			
	//aggiorno lo stato delle navicelle e controllo se ci sono nuovi comandi
	update_spc_state();
	
	//aggiorno lo stato dei proiettili e controllo se ci sono nuovi comandi  
	update_bullet_state();
	
	//propago la posizione dei proiettili
	propagate_bullet1();
	propagate_bullet2();
	
	//aggiorno le posizioni delle navicelloe
	move_spaceship();
	
	//ripristino periodicamente l'energia
	recharge();
*/

/*---------------------------------------------------------------------*/
//		Funzioni relative allo stato della navicella
/*---------------------------------------------------------------------*/

// inizializzo le navicelle all'inizio della partita
void init_spaceship(void){
	player_spc.x		= XWIN/4;		//assegno la posizione iniziale
	player_spc.y		= YWIN/2;
	player_spc.velx	= 0;			//si parte da fermi
	player_spc.vely	= 0;
	player_spc.angle	= 0;
	player_spc.health 	= START_HEALTH;	//assegno la salute iniziale
	player_spc.charge 	= START_CHARGE;	//assegno l'energia iniziale
	pthread_mutex_init(&player_spc.mtx, NULL);	//inizializzo il semaforo della struct
	
	enemy_spc.x		= XWIN*3/4;
	enemy_spc.y		= YWIN/2;
	enemy_spc.velx		= 0;
	enemy_spc.vely		= 0;
	enemy_spc.angle	= 3.14;
	enemy_spc.health 	= START_HEALTH;
	enemy_spc.charge 	= START_CHARGE;
	pthread_mutex_init(&enemy_spc.mtx, NULL);	
}

//Questa funzione legge il flag dei comandi e genera le azioni in base ai tasti premuti
void update_spc_state(void){

	unsigned char 	flag = 0;
	
	pthread_mutex_lock(&command_mutex);
	if(flag_game_com){
		flag = flag_game_com;				//leggo i dati dal flag dei comandi
	}
	pthread_mutex_unlock(&command_mutex);
    	
    	if (flag & 0b11110000){			//A S D W premuti? devo muovere la navicella
    		
    		pthread_mutex_lock(&player_spc.mtx);
    		if (flag & 0b10000000){				//tasto A
			rotate_spc(PLAYER_SPC, UNIT_ANGLE_ROTATION);	//ruoto a sinistra 
			flag &= 0b01111111;}
			
		if (flag & 0b01000000){				//tasto S
			move_forward(PLAYER_SPC,BACKWARD);		//mi muovo indietro
			flag &= 0b10111111;}
			
		if (flag & 0b00100000){				//tasto D
			rotate_spc(PLAYER_SPC, -UNIT_ANGLE_ROTATION);	//ruoto a destra 
			flag &= 0b11011111;}
		
		if (flag & 0b00010000){				//tasto W
			move_forward(PLAYER_SPC,FORWARD);		//mi muovo in avanti
			flag &= 0b11101111;}	
		pthread_mutex_unlock(&player_spc.mtx);
		
		pthread_mutex_lock(&command_mutex);
		flag_game_com &= flag;			//aggiorno in base alle azioni compiute
		pthread_mutex_unlock(&command_mutex);
    	}    	
}

//Funzione che modifica l'orientamento della navicella del giocatore
void rotate_spc(char spc, float angle){
	if(spc == PLAYER_SPC)	
	player_spc.angle += angle;
}

//Funzione per muovere la navicella del giocatore
void move_forward(char spc, char dir){
	
	float new_velx, new_vely;	//variabili locali per il calcolo della velocità
	
	if(spc == PLAYER_SPC){
		
		new_velx = player_spc.velx;
		new_vely = player_spc.vely;

			if(dir==FORWARD){			//avanti
				if(new_velx < MAX_VELOCITY && new_velx > -MAX_VELOCITY)
					new_velx += INC_VELOCITY*cos(player_spc.angle);
				if(new_vely < MAX_VELOCITY && new_vely > -MAX_VELOCITY)
					new_vely += -INC_VELOCITY*sin(player_spc.angle);
			}
			if(dir==BACKWARD){			//indietro
				if(new_velx < MAX_VELOCITY && new_velx > -MAX_VELOCITY)
					new_velx -= INC_VELOCITY*cos(player_spc.angle);
				if(new_vely < MAX_VELOCITY && new_vely > -MAX_VELOCITY)
					new_vely -= -INC_VELOCITY*sin(player_spc.angle);
			}
			player_spc.velx = new_velx;
			player_spc.vely = new_vely;	
	}	
}

//questa funzione invece aggiorna la posizione delle navicelle nello spazio
void move_spaceship(void){
	
	//aggiorno la posizione in base alla velocità
	pthread_mutex_lock(&player_spc.mtx);
	player_spc.x += round(player_spc.velx * DELTA_T);
	player_spc.y += round(player_spc.vely * DELTA_T);
	
	//controllo che non vadano fuori dai bordi della finestra di gioco 
	if(player_spc.x < MARGIN_BOUND) 		player_spc.x = MARGIN_BOUND;
	if(player_spc.x > XWIN-MARGIN_BOUND)		player_spc.x = XWIN-MARGIN_BOUND;
	if(player_spc.y < UPPER_MARGIN_BOUND)		player_spc.y = UPPER_MARGIN_BOUND;
	if(player_spc.y > YWIN-(UPPER_MARGIN_BOUND))	player_spc.y = YWIN-(UPPER_MARGIN_BOUND);
	
	//applico l'attrito modificando la velocità
	if(player_spc.velx > 1.5*friction) 
		player_spc.velx -= friction;
		else if(player_spc.velx < -1.5*friction) 
			player_spc.velx += friction;
			else player_spc.velx = 0;
	
	if(player_spc.vely > 1.5*friction) 
		player_spc.vely -= friction;
		else if(player_spc.vely < -1.5*friction) 
			player_spc.vely += friction;
			else player_spc.vely = 0;
 	pthread_mutex_unlock(&player_spc.mtx);
 		
 	//aggiorno la posizione in base alla velocità
 	pthread_mutex_lock(&enemy_spc.mtx);	
	enemy_spc.x += round(enemy_spc.velx * DELTA_T);
	enemy_spc.y += round(enemy_spc.vely * DELTA_T);
	
	//controllo che non vadano fuori dai bordi della finestra di gioco 	
	if(enemy_spc.x < MARGIN_BOUND) 		enemy_spc.x = MARGIN_BOUND;
	if(enemy_spc.x > XWIN-MARGIN_BOUND)		enemy_spc.x = XWIN-MARGIN_BOUND;
	if(enemy_spc.y < UPPER_MARGIN_BOUND)		enemy_spc.y = UPPER_MARGIN_BOUND;
	if(enemy_spc.y > YWIN-(UPPER_MARGIN_BOUND))	enemy_spc.y = YWIN-(UPPER_MARGIN_BOUND);	

	//applico l'attrito modificando la velocità
	if(enemy_spc.velx > 2*friction) 
		enemy_spc.velx -= friction;
		else if(enemy_spc.velx < -2*friction) 
			enemy_spc.velx += friction;
			else enemy_spc.velx = 0;
	if(enemy_spc.vely > 2*friction) 
		enemy_spc.vely -= friction;
		else if(enemy_spc.vely < -2*friction) 
			enemy_spc.vely += friction;
			else enemy_spc.vely = 0;
 	pthread_mutex_unlock(&enemy_spc.mtx);
 	
}

/*---------------------------------------------------------------------*/
//		Funzioni relative ai proiettili
/*---------------------------------------------------------------------*/

//Questa funzione inizializza le strutture dei proiettili
void init_bullet(void){
	
	//inizializzo i semafori
	for(int i=0; i <= BULLET1_MAX; i++){
		pthread_mutex_init(&bullet_1[i].mtx, NULL);
		bullet_1[i].state=false;
	}
	for(int i=0; i <= BULLET2_MAX; i++){
		pthread_mutex_init(&bullet_2[i].mtx, NULL);
		bullet_2[i].state=false;
	}
	idx_bul1  = 0;
	shot_bul1 = 0;
	hit_bul1  = 0;
	idx_bul2  = 0;
	shot_bul2 = 0;
	hit_bul2  = 0;
}

//Funzione che controlla se ci sono nuovi colpi da creare
void update_bullet_state(void){
	
	unsigned char 	flag = 0;
	int charge;
	
	pthread_mutex_lock(&command_mutex);
	if(flag_game_com){
		flag = flag_game_com;
	}
	pthread_mutex_unlock(&command_mutex);

	if (flag & 0b00001000){		//J premuto? devo fare un nuovo colpo?
    		
    		if (flag & 0b00001000){
    			
    			pthread_mutex_lock(&player_spc.mtx);
    			charge=player_spc.charge;
    			pthread_mutex_unlock(&player_spc.mtx);
			if(charge > BUL1_CHARGE){		//c'è abbastanza energia?
				//crea un nuovo proiettile
				create_new_bullet1();
				
				pthread_mutex_lock(&player_spc.mtx);
    				player_spc.charge -= BUL1_CHARGE; //decrementa l'energia
    				pthread_mutex_unlock(&player_spc.mtx);
    			}
			flag &= 0b11110111;
		}
		
		pthread_mutex_lock(&command_mutex);
		flag_game_com &= flag;	
		pthread_mutex_unlock(&command_mutex);
    	}
}

//Questa funzione inizializza un nuovo colpo in base ai dati della navicella del giocatore,
//viene introdotto anche un certo errore sulla mira 
void create_new_bullet1(void){
	
	if(idx_bul1 < BULLET1_MAX){
		
		int i = 0;
		do{
			if(bullet_1[i].state == false){
			
				float n1 = 0;	//numero random per generare un errore
				float n2 = 0;	//nella mira della navicella
				
				srand(time(NULL)+i);
				n1= ((double)rand() / (double)RAND_MAX)-0.5;
				srand(time(NULL)+i+1);
				n2= ((double)rand() / (double)RAND_MAX)-0.5;
				
				pthread_mutex_lock(&player_spc.mtx);
				pthread_mutex_lock(&bullet_1[i].mtx);				
				
				bullet_1[i].state = true;	
				bullet_1[i].x = player_spc.x+OFFSET_BUL*cos(player_spc.angle);
				bullet_1[i].y = player_spc.y-OFFSET_BUL*sin(player_spc.angle);
				bullet_1[i].velx = BUL1_VEL*
				     cos(player_spc.angle+n1/PLY_FIRE_ANGLE_ERR)+player_spc.velx;
				bullet_1[i].vely =-BUL1_VEL*
				     sin(player_spc.angle+n2/PLY_FIRE_ANGLE_ERR)+player_spc.vely; 
				//c'è il meno perché l'asse y è rivolto verso il basso quindil'angolo è specchiato di pi
				bullet_1[i].angle = player_spc.angle;
				
				switch (lvl){
					case 1:
						bullet_1[i].damage=4;
						break;
					case 2:
						bullet_1[i].damage=2;
						break;
					case 3:
						bullet_1[i].damage=1;
						break;
				}
				pthread_mutex_unlock(&bullet_1[i].mtx);
				pthread_mutex_unlock(&player_spc.mtx);
				
				idx_bul1++;
				shot_bul1++;	//tengo conto delle statistiche
				return;
			}
			i++;
		}while (i <= BULLET1_MAX);
	}
}

//funzione che aggiorna la posizione nel tempo di un colpo o se esso esce dal campo di 
//gioco o colpisce l'altra navicella
void propagate_bullet1(void){
	
	for(int i=0; i <= BULLET1_MAX; i++){

		if(bullet_1[i].state == true){
			
			//propago la posizione dei colpi
			pthread_mutex_lock(&bullet_1[i].mtx);
			
			bullet_1[i].x += round(bullet_1[i].velx * DELTA_T);
			bullet_1[i].y += round(bullet_1[i].vely * DELTA_T);
			
			pthread_mutex_lock(&enemy_spc.mtx);
			//controllo se ho colpito l'altra navicella
			if(bullet_1[i].x > enemy_spc.x-HIT_SPC_AREA &&
			   bullet_1[i].x < enemy_spc.x+HIT_SPC_AREA &&
			   bullet_1[i].y > enemy_spc.y-HIT_SPC_AREA &&
			   bullet_1[i].y < enemy_spc.y+HIT_SPC_AREA ){
				   bullet_1[i].state=false;
				   //enemy_spc.health -= 1;
				   enemy_spc.health -= bullet_1[i].damage;
				   if (enemy_spc.health < 0)
				   	 enemy_spc.health = 0;
				   hit_bul1++;		//tengo conto delle statistiche
				   
				   //se il nemico non ha più energia concludo il gioco
				   if(enemy_spc.health == 0){
					game_state=false;
					menu_state=true;
				   }
			}
    			pthread_mutex_unlock(&enemy_spc.mtx);
			
			//controllo se il proiettile è fuori dai margini della finestra
			if(bullet_1[i].x < BUL_MARGIN_BOUND) 		bullet_1[i].state=false;
			if(bullet_1[i].x > XWIN-BUL_MARGIN_BOUND)	bullet_1[i].state=false;
			if(bullet_1[i].y < BUL_MARGIN_BOUND)		bullet_1[i].state=false;
			if(bullet_1[i].y > YWIN-BUL_MARGIN_BOUND)	bullet_1[i].state=false;
				    
			if (bullet_1[i].state == false) idx_bul1--;
			
			pthread_mutex_unlock(&bullet_1[i].mtx);
		}
	}
}

//funzione che aggiorna la posizione nel tempo di un colpo o se esso esce dal campo di 
//gioco o colpisce l'altra navicella
void propagate_bullet2(void){
	
	for(int i=0; i <= BULLET2_MAX; i++){

		if(bullet_2[i].state == true){
			
			//propago la posizione dei colpi
			pthread_mutex_lock(&bullet_2[i].mtx);
			
			bullet_2[i].x += round(bullet_2[i].velx * DELTA_T);
			bullet_2[i].y += round(bullet_2[i].vely * DELTA_T);
    			
    			pthread_mutex_lock(&player_spc.mtx);
    			
    			//controllo se ho colpito l'altra navicella
			if(bullet_2[i].x > player_spc.x-HIT_SPC_AREA &&
			   bullet_2[i].x < player_spc.x+HIT_SPC_AREA &&
			   bullet_2[i].y > player_spc.y-HIT_SPC_AREA &&
			   bullet_2[i].y < player_spc.y+HIT_SPC_AREA ){
				   bullet_2[i].state=false;
				   player_spc.health -= 1;
				   //player_spc.health -= bullet_2[i].damage;
				   hit_bul2++;		//tengo conto delle statistiche
				   
				   //se il giocatore non ha più energia concludo il gioco
				   if(player_spc.health == 0){
					game_state=false;
					menu_state=true;
				   }
			}
    			pthread_mutex_unlock(&player_spc.mtx);
			
			//controllo se il proiettile è fuori dai margini della finestra
			if(bullet_2[i].x < BUL_MARGIN_BOUND) 		bullet_2[i].state=false;
			if(bullet_2[i].x > XWIN-BUL_MARGIN_BOUND)	bullet_2[i].state=false;
			if(bullet_2[i].y < BUL_MARGIN_BOUND)		bullet_2[i].state=false;
			if(bullet_2[i].y > YWIN-BUL_MARGIN_BOUND)	bullet_2[i].state=false;
				    
			if (bullet_2[i].state == false) idx_bul2--;
			
			pthread_mutex_unlock(&bullet_2[i].mtx);
		}
	}
}

//questa funzione si occupa di ricaricare l'energia ogni periodo del task di gioco
void recharge(void){


	pthread_mutex_lock(&player_spc.mtx);
    	if(player_spc.charge <=START_HEALTH-PLY_RECHARGE_RATE)
    		player_spc.charge += PLY_RECHARGE_RATE;
    	pthread_mutex_unlock(&player_spc.mtx);
    	
    	pthread_mutex_lock(&enemy_spc.mtx);
    	if(enemy_spc.charge <=START_HEALTH-ENY_RECHARGE_RATE)
    		enemy_spc.charge += ENY_RECHARGE_RATE;
    	pthread_mutex_unlock(&enemy_spc.mtx);
}

/*---------------------------------------------------------------------*/
//		Funzioni dell'Intelligenza Artificiale nemica
/*---------------------------------------------------------------------*/

//funizione che casualmente genera un nuovo colpo 
void enemy_shot(void){
	
	int charge;
	srand(AI_i);
	
	float n = ((double)rand() / (double)RAND_MAX);	//genero il numero random

	if(n > 1 - SHOT_RATE){ 	
		pthread_mutex_lock(&enemy_spc.mtx);
		charge = enemy_spc.charge;
		pthread_mutex_unlock(&enemy_spc.mtx);

		if(charge > BUL2_CHARGE){
			//genero un nuovo colpo
			create_new_bullet2();				

			pthread_mutex_lock(&enemy_spc.mtx);
			enemy_spc.charge -= BUL2_CHARGE;	//decremento la carica
			pthread_mutex_unlock(&enemy_spc.mtx);
		}				
	}
}

//funzione inizializza il nuovo colpo in base alla posizione della navicella nemica
void create_new_bullet2(void){
  
	if(idx_bul2 < BULLET2_MAX){		
	int i = 0;
	float 	point_angle;
		 
	do{
		if(bullet_2[i].state == false){		
			
			//cancolo della mira				
			point_angle = compute_sight();
			
			pthread_mutex_lock(&bullet_2[i].mtx);	
			pthread_mutex_lock(&enemy_spc.mtx);	
			bullet_2[i].state = true;	
			bullet_2[i].x = enemy_spc.x+OFFSET_BUL*cos(enemy_spc.angle);
			bullet_2[i].y = enemy_spc.y-OFFSET_BUL*sin(enemy_spc.angle);
			bullet_2[i].velx = BUL2_VEL*cos(point_angle);
			bullet_2[i].vely = -BUL2_VEL*sin(point_angle);	//c'è il meno perché l'asse y è rivolto verso il basso quindil'angolo è specchiato di pi
			bullet_2[i].angle = point_angle;
			pthread_mutex_unlock(&bullet_2[i].mtx);
			pthread_mutex_unlock(&enemy_spc.mtx);

			idx_bul2++;
			shot_bul2++;	//aggiorno le statistiche
			return;
		}
		i++;
	}while (i <= BULLET2_MAX);
	}
}

//questa funzione produce l'angolo di mira tra enemy e player con un certo errore
float compute_sight(void){

	float inter_time, n;
	int p_x,p_y,e_x,e_y,p_vx,p_vy;
		
	//copio i dati che mi servono in variabili locali
	pthread_mutex_lock(&enemy_spc.mtx);
	pthread_mutex_lock(&player_spc.mtx);
	p_x = player_spc.x;
	p_y = player_spc.y;
	e_x = enemy_spc.x;
	e_y = enemy_spc.y;
	p_vx = player_spc.velx;
	p_vy = player_spc.vely;
	pthread_mutex_unlock(&enemy_spc.mtx);
	pthread_mutex_unlock(&player_spc.mtx);		
	
	//genero numeri casuali e "sporco" la posizione della navicella del giocatore
	srand(p_x+count);		
	n = ((double)rand()/(double)RAND_MAX)*2*RADAR_ERROR-RADAR_ERROR;
	true_x = p_x+n;
	srand(p_y+count);
	n = ((double)rand()/(double)RAND_MAX)*2*RADAR_ERROR-RADAR_ERROR;
	true_y = p_y+n;
	
	//se il livello è 3, calcolo una predizione della posizione nel futuro
	if(lvl == 3){
		//tempo di impatto = distanza tra le navicelle / velocità proiettile
		inter_time = sqrt((p_x-e_x)*
				(p_x-e_x)+
				(p_y-e_y)*
				(p_y-e_y))/BUL2_VEL;
		//posizione futura = velocità attuale * tempo di impatto
		true_x += round(p_vx * inter_time);
		true_y += round(p_vy * inter_time);
	}
	
	//ritorna l'angolo della retta che passa per le posizioni delle due navicelle
	return atan2(e_y-true_y,true_x-e_x);
}

//funzione che periodicamente genera un nuovo riferimento di posizione nel campo di gioco
void new_pos_des(void){

	if(ready && count > NEW_POS_RATE){
		srand(time(NULL)+AI_i);
		x_des = ((double)rand()/(double)RAND_MAX)*
			(XWIN-2*MARGIN_BOUND)+MARGIN_BOUND;
		srand(time(NULL)+AI_i+1);
		y_des = ((double)rand() / (double)RAND_MAX)*
			(YWIN-2*(UPPER_MARGIN_BOUND))+UPPER_MARGIN_BOUND;
		ready = 0;
		count = 0;
	}

}

//funzione che aggiorna l'angolo e la velocità della navicella nemica
void update_enemy_state(void){

	float new_velx, new_vely;
	
	pthread_mutex_lock(&enemy_spc.mtx);
	
	//la navicella punta sempre verso il nemico per una questione anche grafica
	pthread_mutex_lock(&player_spc.mtx);
	enemy_spc.angle = atan2(enemy_spc.y-player_spc.y,player_spc.x-enemy_spc.x);
	pthread_mutex_unlock(&player_spc.mtx);
		
	new_velx = enemy_spc.velx;
	new_vely = enemy_spc.vely;
	//se non l'ho già fatto, mi muovo verso il punto generato casualmente
	if(!ready){
		//controllore proporzionale della velocità
		new_velx = -KP_POS_CONTROLLER*(enemy_spc.x-x_des);
		new_vely = -KP_POS_CONTROLLER*(enemy_spc.y-y_des);
		
		if(enemy_spc.x < x_des+POS_DES_ERROR && enemy_spc.x > x_des-POS_DES_ERROR &&
		   enemy_spc.y < y_des+POS_DES_ERROR && enemy_spc.y > y_des-POS_DES_ERROR )
		   ready=1;
	
		if(new_velx > MAX_VELOCITY) 	new_velx = MAX_VELOCITY;
		if(new_velx < -MAX_VELOCITY) 	new_velx = -MAX_VELOCITY;
		if(new_vely > MAX_VELOCITY) 	new_vely = MAX_VELOCITY;
		if(new_vely < -MAX_VELOCITY) 	new_vely = -MAX_VELOCITY;			
	}
	enemy_spc.velx = new_velx;
	enemy_spc.vely = new_vely;
	pthread_mutex_unlock(&enemy_spc.mtx);
}

//funzione che genera un nuovo riferimento di posizione se la navicella è sotto
//tiro da quella del giocatore
void under_fired_alert(void){

	float angle,inter;
	
	//se posso spostarmi di nuovo e il livello è almeno 2...
	if(count>NEW_POS_ALERT && lvl >= 2){
		
		//calcolo dell'angolo tra navicella del giocatore e quella nemica
		//mi salvo anche l'angolo di riferimento della navicella del giocatore
		pthread_mutex_lock(&enemy_spc.mtx);
		pthread_mutex_lock(&player_spc.mtx);
		angle = atan2(player_spc.y-enemy_spc.y, enemy_spc.x - player_spc.x);
		inter=player_spc.angle;
		pthread_mutex_unlock(&enemy_spc.mtx);
		pthread_mutex_unlock(&player_spc.mtx);
		
		//normalizzo l'angolo tra +-2pi
		while (inter >= M_PI) inter -= 2*M_PI;
		while (inter < -M_PI) inter += 2*M_PI;
		
		//se il giocatore mira verso la navicella nemica entro un certo range di errore
		//allora il nemico è sotto tiro e può generare una nuova posizione di 
		//riferimento per spostarsi
		if(inter > angle-UNDER_FIRE_ANG_ERROR && inter < angle+UNDER_FIRE_ANG_ERROR){
			ready=1;
			count=NEW_POS_RATE;
		}
	}
}

/***************************** END OF FILE **********************************/
/****************************************************************************/
/* 	SPACEBATTLE GAME, RTS PROJECT, DEVELOPED BY MARCO BORRACCINO        */
/****************************************************************************/
