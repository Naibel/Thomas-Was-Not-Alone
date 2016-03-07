#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_mixer.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include "functions.h"

int main(int argc, char** argv) {

  float windowWidth  = 800;
  float windowHeight = 600;
  float maxSpeed=10;
  int i, j, finish = 0, end = 0, out = 0, nbLevel = 0;
  char lvlFile[20], doorNum[20];
  
  /*---------------PARTIE VARIABLES-----------------*/
  
  //Initialisation des variables de saut, de focus et de pressage de touches
  int en_vol = 0, move_left = 0, move_right = 0, focused = 0, openDoor = 0, currentLvl = 0;
  
  /*Création de trois tableaux, le premier va stocker les personnages cubiques, le second va stocker les plate-formes
  et le dernier va stocker les portes menants aux différents niveaux.*/
  carre *square = NULL;
  carre *platform = NULL;
  carre *door = NULL;
  level l;
  int nbPersos;
  int nbPlatforms;
  int nbDoors;

  // Définition du fichier de niveau à charger
  sprintf(lvlFile, "levels/level-%d.txt", nbLevel);
  printf("%s\n", lvlFile);

  l = initializeFromFile(lvlFile, &nbPersos, &nbPlatforms, &nbDoors, &square, &platform, &door);

  printf("%d\n", nbPlatforms);
  
  /*-----------------------------------------------*/

  if(-1 == SDL_Init(SDL_INIT_VIDEO)) {
    fprintf(stderr, "Impossible d'initialiser la SDL. Fin du programme.\n");
    return EXIT_FAILURE;
  }
  
  setVideoMode(windowWidth, windowHeight,square[focused], l);

  SDL_WM_SetCaption("Thomas Was Not Alone - Belhaj/Assante Di Capillo, Copy(pas)right 2015", NULL);
  
  //Initialisation de l'API Mixer
  if(Mix_OpenAudio(32000, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 1024) == -1)
   {
      printf("%s", Mix_GetError());
   }
   Mix_AllocateChannels(32); //Allouer 32 canaux
   Mix_Music *musique; //Création du pointeur de type Mix_Music
   Mix_Chunk *jump_sound = NULL, *win_sound, *door_open;//Créer un pointeur pour stocker un .WAV
   
   musique = Mix_LoadMUS("sons/music.wav");
   Mix_VolumeMusic(MIX_MAX_VOLUME/4);
   win_sound = Mix_LoadWAV("sons/dittie.wav");
   Mix_VolumeChunk(win_sound, MIX_MAX_VOLUME/2); 
   door_open = Mix_LoadWAV("sons/door_open.wav");
   Mix_VolumeChunk(door_open, MIX_MAX_VOLUME/2);
   Mix_PlayMusic(musique, -1); //Jouer infiniment la musique (-1)
  
  GLuint textureId, mssgTextureId, doorTextureId, enterTextureId, controlsTextureId, titleTextureId;
   
  //Appel de la fonction d'initialisation des textures
  textureId = Init_Texture("images/fond.png");

  enterTextureId = Init_Texture("images/enter.png");
  controlsTextureId = Init_Texture("images/controls.png");
  titleTextureId = Init_Texture("images/title.png");

  while(!out)
  {
	  en_vol = 0, move_left = 0, move_right = 0, focused = 0, openDoor = 0, finish = 0, end = 0;
	  currentLvl = nbLevel;
	  printf("lvl = %d\n", nbLevel);
	  // Définition du fichier de niveau à charger
	  sprintf(lvlFile, "levels/level-%d.txt", nbLevel);
	  printf("%s\n", lvlFile);

	  l = initializeFromFile(lvlFile, &nbPersos, &nbPlatforms, &nbDoors, &square, &platform, &door);

	  printf("%d\n", nbPlatforms);

	  printf("finish %d end %d \n", finish, end);

	  int loop = 1;
	  while(loop) {

		/* temps au début de la boucle */
		Uint32 startTime = SDL_GetTicks();
		
		/*---------------PARTIE DESSIN-----------------*/
		
		//glEnable(GL_LINE_SMOOTH);
		//glEnable(GL_POLYGON_SMOOTH);
		glClear(GL_COLOR_BUFFER_BIT);
		glEnable(GL_BLEND);
		
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glClearColor(0,0,0,0);
		//glEnable(GL_MULTISAMPLE);
			
		//Appel de la fonction d'affichage du Background
		draw_BG(textureId);
		
		if(currentLvl ==0){
			glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, controlsTextureId);
					glBegin(GL_QUADS);
						glColor3f(1, 1, 1);
			
						glTexCoord2f(0, 0);
						glVertex2f(700, 1375.);
						
						glTexCoord2f(1, 0);
						glVertex2f(1000.0, 1375.);
						
						glTexCoord2f(1, 1);
						glVertex2f(1000.0, 1050.);
						
						glTexCoord2f(0, 1);
						glVertex2f(700.0, 1050.);

					glEnd();
				glBindTexture(GL_TEXTURE_2D, 0);
				
				glBindTexture(GL_TEXTURE_2D, titleTextureId);
						glBegin(GL_QUADS);
							glColor3f(1, 1, 1);
				
							glTexCoord2f(0, 0);
							glVertex2f(100, 1300.);
							
							glTexCoord2f(1, 0);
							glVertex2f(600.0, 1300.);
							
							glTexCoord2f(1, 1);
							glVertex2f(600.0, 1150.);
							
							glTexCoord2f(0, 1);
							glVertex2f(100.0, 1150.);

						glEnd();
				glBindTexture(GL_TEXTURE_2D, 0);
			glDisable(GL_TEXTURE_2D);
		}
			
		//-----Boucle d'affichage------
		
		if (!end) 
		{
			//Dessin des portes
			for(i=0 ; i<nbDoors ; i++){
				glPushMatrix();
				 	sprintf(doorNum, "images/door-%d.png", door[i].toLvl);
				 	doorTextureId = Init_Texture(doorNum);

					//drawShadow2(door[i], 0.6);
					drawDoor(doorTextureId, door[i]);
					  glDeleteTextures(1, &doorTextureId);
				glPopMatrix();
			}

			for(i=0 ; i<nbPersos ; i++){
				//Affichage des ombres
				glPushMatrix();
					drawShadow2(square[i], 0.15); //Affichage des ombres des carrés
				glPopMatrix();
				
				//Dessin des rectangles
				glPushMatrix();
					//Si les personnages rencontrent leurs formes respectives, ils deviennent blancs.
					if(collision_form(square[i]))
						glColor3ub(255, 255, 255);
					else
						glColor3ub(square[i].color_R, square[i].color_V, square[i].color_B);
						
					glTranslatef(square[i].x, square[i].y, 0);
					glScalef(square[i].width, square[i].height, 0);
					dessinRectangle(1);
				glPopMatrix();
				
				//Dessin des silhouettes de rectangles
				glPushMatrix();
					glColor3ub(255, 255, 255);
					glTranslatef(square[i].x_fin, square[i].y_fin, 0);
					glScalef(square[i].width, square[i].height, 0);
					dessinRectangle(0);
				glPopMatrix();
				
			}
		}

			for(i=0 ; i<nbPlatforms ; i++){
				//On ne dessine des ombres que sur les plates-formes auxquelles on a décidé d'en ajouter une
				if(platform[i].shadowed==1)
					drawShadow2(platform[i], 0.6);
				
				//Dessin des plates-formes
				glPushMatrix();
					if(platform[i].type == 0){//Dessin des plates-formes solides
						glColor3ub(platform[i].color_R, platform[i].color_V, platform[i].color_B);
						glTranslatef(platform[i].x, platform[i].y, 0);
						glScalef(platform[i].width, platform[i].height, 0);
						dessinRectangle(1);
					}
					else if(platform[i].type == 1){//Dessin des zones d'eau
						glColor4f(255, 255, 255, 0.3);
						glTranslatef(platform[i].x, platform[i].y, 0);
						glScalef(platform[i].width, platform[i].height, 0);
						dessinRectangle(1);
					}
					else if (platform[i].type == 2)
						drawDoor(doorTextureId, platform[i]);
				glPopMatrix();
			}

		//Affichage de la flèche
		if (!finish && !end)
		{
			glPushMatrix();
				glColor4f(255.0, 255.0, 255.0, 0.8);
				drawFleche(square[focused]);
			glPopMatrix();
		}

		if (end)
		{
			Mix_PlayChannel(3, win_sound, 0);//Joue le son 1 sur le canal 1 ; le joue une fois (0 + 1)
			mssgTextureId = Init_Texture("images/end.png");
			glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, mssgTextureId);
					glBegin(GL_QUADS);
						glColor3f(1, 1, 1);
			
						glTexCoord2f(0, 0);
						glVertex2f(square[focused].x - 200.0, square[focused].y + 300.);
						
						glTexCoord2f(1, 0);
						glVertex2f(square[focused].x + 200.0, square[focused].y + 300.);
						
						glTexCoord2f(1, 1);
						glVertex2f(square[focused].x + 200.0, square[focused].y + 100.);
						
						glTexCoord2f(0, 1);
						glVertex2f(square[focused].x - 200.0, square[focused].y + 100.);

					glEnd();
				glBindTexture(GL_TEXTURE_2D, 0);
			glDisable(GL_TEXTURE_2D);
		}
		
		if(openDoor){
			glEnable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, enterTextureId);
					glBegin(GL_QUADS);
						glColor3f(1, 1, 1);
			
						glTexCoord2f(0, 0);
						glVertex2f(square[focused].x - 150.0, square[focused].y + 145.);
						
						glTexCoord2f(1, 0);
						glVertex2f(square[focused].x + 150.0, square[focused].y + 145.);
						
						glTexCoord2f(1, 1);
						glVertex2f(square[focused].x + 150.0, square[focused].y + 100.);
						
						glTexCoord2f(0, 1);
						glVertex2f(square[focused].x - 150.0, square[focused].y + 100.);

					glEnd();
				glBindTexture(GL_TEXTURE_2D, 0);
			glDisable(GL_TEXTURE_2D);
		}
		
		glEnd();
		
		SDL_GL_SwapBuffers();
		/* ****** */    
		
		/*---------------PARTIE PHYSIQUE-----------------*/
		
		finish = 1;
		openDoor = 0;

		for (i = 0; i < nbPersos; ++i)
		{
			if (collision_form(square[i]) && square[i].dx == 0)
				square[i].x = square[i].x_fin;
			else
				finish = 0;
		}

		if (finish == 1)
		{	
			for (i = 0; i < nbPersos; ++i)
			{

					square[i].width *= 0.95;
					square[i].height *= 0.95;
					square[i].dx = 0;
					square[i].dy = 0;
			}
		}
		else if (square[focused].width <= 10 || (square[focused].height <= 10 && finish))
		{
			if (end)
			{
				int delay = time(0) + 3;
				while(time(0)<delay){
					loop=1;
				}
				nbLevel = 0;
				loop = 0;
			}
			end = 1;
		}

		//-------COLLISION ENTRE CARRES----------

		for(j=0 ; j<nbPersos ; j++){
			for(i=0 ; i<nbPersos ; i++){
				if (i != j && collision(square[j], square[i])==1){
						// collision droite
						if(square[j].x + maxSpeed >= square[i].x + square[i].width && square[focused].dx < 0 && square[i].onSquare != j) {
							square[j].x = square[i].x + square[i].width;
							square[j].dx *= 0;
							if(square[j].onSquare != -1)
							{
								square[square[j].onSquare].underSquare = 0;
								square[square[j].onSquare].onGround = 0;
								square[j].onSquare = -1;
								square[j].onGround = 0;
							}
						}
						// collision gauche
						else if(square[j].x + square[j].width - maxSpeed <= square[i].x && square[focused].dx > 0 && square[i].onSquare != j) {
							square[j].x = square[i].x - square[j].width;
							square[j].dx *= 0;
							if(square[j].onSquare != -1)
							{
								square[square[j].onSquare].underSquare = 0;
								square[square[j].onSquare].onGround = 0;
								square[j].onSquare = -1;
								square[j].onGround = 0;
							}
						}
						// collision haut
						else if(square[j].y + square[j].height <= square[i].y && square[j].onSquare == i) {
							square[j].y = square[i].y - square[j].height;
							square[j].dy *= 0;
						}
						// collision bas
						else if(square[j].y + 2*square[j].jumpPower >= square[i].y + square[i].height) {
							square[j].y = square[i].y + square[i].height;
							if (square[j].onSquare != -1)
							{
								square[square[j].onSquare].underSquare = 0;
							}
							square[j].onSquare = i;
							square[j].onPlatform = -1;
							square[j].onGround = 1;
							square[j].dy *= 0;
							square[square[j].onSquare].underSquare = 1;
						}
				}
				if (square[j].onSquare != -1 && square[square[j].onSquare].underSquare == 1)
				{
					if ((square[j].x + square[j].width <= square[square[j].onSquare].x)
					||  (square[j].x >= square[square[j].onSquare].x + square[square[j].onSquare].width))
					{
						square[square[j].onSquare].underSquare = 0;
						square[j].onSquare = -1;
						square[j].onGround = 0;
					}
				}
			}
		
		//-------COLLISION AVEC LES PLATES-FORMES----------
				
			for(i=0 ; i<nbPlatforms ; i++){
				if(collision(square[j], platform[i])==1){
					// collision droite
					if(square[j].x + maxSpeed >= platform[i].x + platform[i].width && square[j].dx < 0) {
						square[j].x = platform[i].x + platform[i].width;
						square[j].dx *= 0;
						if (square[j].onSquare != -1)
						{
							square[square[j].onSquare].underSquare = 0;
							square[square[j].onSquare].onGround = 0;
							square[j].onSquare = -1;
							square[j].onGround = 0;
						}
					}
					// collision gauche
					else if(square[j].x + square[j].width - maxSpeed <= platform[i].x && square[j].dx > 0) {
						square[j].x = platform[i].x - square[j].width;
						square[j].dx *= 0;
						if (square[j].onSquare != -1)
						{
							square[square[j].onSquare].underSquare = 0;
							square[square[j].onSquare].onGround = 0;
							square[j].onSquare = -1;
							square[j].onGround = 0;
						}
					}
					// collision haut
					else if(square[j].y <= platform[i].y) {
						if (square[j].height <= platform[i].height || platform[i].type == 0)
						{
							square[j].y = platform[i].y - square[j].height;
							square[j].dy *= 0;
							if (square[j].onSquare != -1)
							{
								int currentSquare = j;
								while (square[currentSquare].onSquare != -1)
								{
									square[square[currentSquare].onSquare].dx *= 0;
									square[square[currentSquare].onSquare].y =  square[currentSquare].y - square[square[currentSquare].onSquare].height;
									currentSquare = square[currentSquare].onSquare;
									
								}
							}
						}
					}
					// collision bas
					else if(square[j].y + square[j].height >= platform[i].y + platform[i].height) {
						if(platform[i].type == 0){//On vérifie si la plateforme est solide
							square[j].dy *= 0;
							if (square[j].onSquare != 1)
							{
								square[j].y = platform[i].y + platform[i].height;
							}
						}
						else{//La plate-forme est alors une zone aquatique
							/* Vérifier que le cube ne soit pas plus haut que l'eau */
							if(square[j].y <= platform[i].y + platform[i].height - square[j].height/2){
								if (square[j].height <= platform[i].height)
									square[j].y = platform[i].y + platform[i].height -square[j].height/2;
							}
							square[j].dy *= 0.5;
						}
						square[j].onGround = 1;
						if (square[j].onSquare != -1)
						{
							square[square[j].onSquare].underSquare = 0;
						}
							
						square[j].onSquare = -1;
						square[j].onPlatform = i;
					}
				}
			}		
			if (square[j].onPlatform != -1)
			{
				if ((square[j].x + square[j].width <= platform[square[j].onPlatform].x)
					|| (square[j].x >= platform[square[j].onPlatform].x + platform[square[j].onPlatform].width))
				{
					square[j].onPlatform = -1;
					square[j].onGround = 0;
				}
			}

			for (i = 0; i < nbDoors; ++i)
			{
				if(collision(square[j], door[i])==1){
					openDoor = 1;
					nbLevel = door[i].toLvl;
				}
			}
		}
		
		if(move_left==1){
			//Déplacement vers la gauche
			if(square[focused].dx<(0.5-maxSpeed))
				square[focused].dx=maxSpeed*(-1);
			else
				square[focused].dx+=(-0.5);
		}
		
		if(move_right==1){
			//Déplacement vers la droite
			if(square[focused].dx>(maxSpeed-0.5))
				square[focused].dx=maxSpeed;
			else
				square[focused].dx+=0.5;
		}
		
		if(move_left==0 && move_right==0){
			//Lorsque la touche de gauche ou de droite est levée
			if (square[focused].dx > 0.5) {
				
				if(square[focused].dx < 0.5)
					square[focused].dx = 0;
				else
					square[focused].dx+=(-0.5);
					
			} 
			else {
				
				if(square[focused].dx>(-0.5))
					square[focused].dx=0;
				else
					square[focused].dx+=0.5;
					
			}
		}
		
		/*S'il y a focus sur un cube, on recentre la caméra sur ce cube*/
		reshape(windowWidth, windowHeight, square[focused], l);
		
		//Fonction de saut
		if(en_vol==1){
			
			if(square[focused].width<=20){//On adapte le bruit en fonction de la taille du carré
				 jump_sound = Mix_LoadWAV("sons/Retro_jump0.wav"); //Chargement du bruitage aigu
			}
			else if (square[focused].width > 20 && square[focused].width <= 30){
				 jump_sound = Mix_LoadWAV("sons/Retro_jump2.wav"); //Chargement du bruitage moyen
			}
			else{
				 jump_sound = Mix_LoadWAV("sons/Retro_jump3.wav"); //Chargement du bruitage grave
			}	
			
			Mix_VolumeChunk(jump_sound, MIX_MAX_VOLUME/2); //Mettre un volume pour ce wav
			Mix_PlayChannel(1, jump_sound, 0);//Joue le son 1 sur le canal 1 ; le joue une fois (0 + 1)
			en_vol = 0;
			square[focused].dy=square[focused].jumpPower*1.5;
			square[focused].onGround = 0;
			
			if (square[focused].onSquare != -1)
				square[square[focused].onSquare].underSquare = 0;
				
			square[focused].onSquare = -1;
		}
		else
			square[focused].dy+=-0.3;

		/*Le personnage sur lequel on a le focus est le seul capable de se déplacer, car
		on lui a attribué des caractéristiques de vitesse.*/
		if (!finish)
		{
			square[focused].x+=square[focused].dx;
			square[focused].y+=square[focused].dy; 
		}
		else
			reshape(windowWidth, windowHeight, square[focused], l);
		
		for (i = 0; i < nbPersos; ++i)
		{
			if(i != focused)
			{
				if ((square[i].onGround == 0) || (square[i].onSquare == -1 && square[i].onGround == 0))
				{
					square[i].dy += -0.5;
					square[i].y += square[i].dy;
				}
				
				else if(square[i].onSquare != -1 && square[i].onGround == 1)
				{
					square[i].dx = square[square[i].onSquare].dx;
					if (square[i].dx < 1.5 && square[i].dx > -1.5)
					{
						square[i].dx = 0;
					}
					square[i].y = square[square[i].onSquare].y + square[square[i].onSquare].height;
					square[i].x += square[i].dx;
				}
			}
		}
		
		/*-------------PARTIE EVENTS-------------*/
		
		
		SDL_Event e;
		
		while(SDL_PollEvent(&e)) {
		  if(e.type == SDL_QUIT) {
			loop = 0;
			out = 1;
			break;
		  }
		  
		  switch(e.type) { 
			case SDL_VIDEORESIZE:
			  windowWidth  = e.resize.w;
			  windowHeight = e.resize.h;
			  break;
				
			case SDL_KEYDOWN:
			  switch(e.key.keysym.sym){
				case SDLK_LEFT : 
					move_left=1;
				break;
				case SDLK_RIGHT : 
					move_right=1;
				break;

				case SDLK_UP :
					if (openDoor)
					{
						Mix_PlayChannel(3, door_open, 0);
						loop = 0;
					}
				break;
				
				case SDLK_SPACE:
						if (square[focused].onGround == 0 || square[focused].underSquare){
							/*S'il n'y a collision  nulle part, on empêche
							le cube de resauter en plein vol*/
							en_vol=0;
						}
						else
							en_vol=1;
				break;
				
				case SDLK_TAB: 
					if(nbLevel!=0){
						focused++;
						if(focused == nbPersos){
							focused = 0;
						}
						square[focused].dy *= 0;
						square[focused].dx *= 0;
					}
				break;

				
				case SDLK_ESCAPE : 
				  if (currentLvl != 0)
				  {
				  	loop = 0;
				  	nbLevel = 0;
				  }

				  else {
				  	loop = 0;
				  	out = 1;
				  }
				  
				  
				break;
				
				default : break;

			  }
			  break;
			 
			 case SDL_KEYUP: //Lorsque l'on relache les touches directionnelles, on fait cesser le mouvement du cube
			  switch(e.key.keysym.sym){
				case SDLK_LEFT : 
					move_left=0;
				break;
				case SDLK_RIGHT : 
					move_right=0;
				break;
				default: break;
			  }
			 break;   
		  }
		}
		
		Uint32 elapsedTime = SDL_GetTicks() - startTime;
		if(elapsedTime < FRAMERATE_MILLISECONDS) {
		  SDL_Delay(FRAMERATE_MILLISECONDS - elapsedTime);
		}
	  }

	  free(square);
	  square = NULL;
	  free(door);
	  door = NULL;
	  free(platform);
	  platform = NULL;
  }

  glDeleteTextures(1, &textureId);
  glDeleteTextures(1, &mssgTextureId);
  glDeleteTextures(1, &doorTextureId);
  glDeleteTextures(1, &enterTextureId);
  glDeleteTextures(1, &controlsTextureId);
  glDeleteTextures(1, &titleTextureId);
  
  Mix_FreeMusic(musique); //Libération de la musique
  if (jump_sound != NULL)
  {
  	Mix_FreeChunk(jump_sound);//Libération du bruitage
  }
  Mix_FreeChunk(win_sound);
  Mix_FreeChunk(door_open);

  Mix_CloseAudio(); //Fermeture de l'API
  SDL_Quit();
  
  return EXIT_SUCCESS;
}
