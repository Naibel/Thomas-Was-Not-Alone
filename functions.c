#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include <string.h>

#include "functions.h"

#define NB_TOTAL_PTS 5
#define M_PI 3.14159265358979323846

carre createCarre(int w, int h, int x, int y, int x_f, int y_f, float jump_power, int r, int v, int b){
	carre c; 
	
    c.width = w;
	c.height = h;
	c.color_R = r;
	c.color_V = v;
	c.color_B = b;
	c.x = x;
	c.y = y;
	c.x_fin = x_f;
	c.y_fin = y_f;
	c.onGround = 0;
	c.onSquare = -1;
	c.underSquare = 0;
	c.onPlatform = -1;
	c.shadowed = 1;
	c.type = 1; //1 = solid square
	c.dx = 0;
	c.dy = 0;
	c.jumpPower = jump_power;
	c.toLvl = -1;
	
    return c;
}

carre createPlatform(int w, int h, int x, int y, int shadowed, int type){
	carre p;
	 
    p.width = w;
	p.height = h;
	p.color_R = 0;
	p.color_V = 0;
	p.color_B = 0;
	p.x = x;
	p.y = y;
	p.x_fin = 0;
	p.y_fin = 0;
	p.onGround = 1;
	p.onSquare = -1;
	p.underSquare = 0;
	p.onPlatform = -1;
	p.shadowed = shadowed;
	p.type = type; //0 =  solid platform, 1 = water platform
	p.dx = 0;
	p.dy = 0;
	p.jumpPower = 0;
	p.toLvl = -1;
	
    return p;
}

carre createDoor(int w, int h, int x, int y, int shadowed, int tL){
	carre d;
	 
    d.width = w;
	d.height = h;
	d.color_R = 0;
	d.color_V = 0;
	d.color_B = 0;
	d.x = x;
	d.y = y;
	d.x_fin = 0;
	d.y_fin = 0;
	d.onGround = 1;
	d.onSquare = -1;
	d.underSquare = 0;
	d.onPlatform = -1;
	d.shadowed = shadowed;
	d.type = 0; //0 =  solid platform, 1 = water platform
	d.dx = 0;
	d.dy = 0;
	d.jumpPower = 0;
	d.toLvl = tL;
	
    return d;
}

void reshape(int winWidth, int winHeight, carre c, level l){
  glViewport(0, 0, winWidth, winHeight);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glRotatef(5, 1., 1., 1.);
  gluOrtho2D(c.x - l.scale * 400, c.x + l.scale * 400, c.y - l.scale * 300 + 70, c.y + l.scale * 300 + 70);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void setVideoMode(int winWidth, int winHeight, carre c, level l) {
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES,4);
  if(NULL == SDL_SetVideoMode(winWidth, winHeight, BIT_PER_PIXEL, SDL_OPENGL | SDL_RESIZABLE)) {
    fprintf(stderr, "Impossible d'ouvrir la fenetre. Fin du programme.\n");
    exit(EXIT_FAILURE);
  }
  reshape(winWidth,winHeight, c, l);
}

GLuint Init_Texture(char * path){
	SDL_Surface* image = IMG_Load(path);
    if(image == NULL) {
        fprintf(stderr, "Impossible de charger l'image %s\n", path);
        return EXIT_FAILURE;
    }
    
    GLuint textureId;
	
    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    
    GLenum format;
    switch(image->format->BytesPerPixel) {
        case 1:
            format = GL_RED;
            break;
        case 3:
            format = GL_RGB;
            break;
        case 4:
            format = GL_RGBA;
            break;
        default:
            fprintf(stderr, "Format des pixels de l'image %s non pris en charge\n", path);
            return EXIT_FAILURE;
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image->w, image->h, 0, format, GL_UNSIGNED_BYTE, image->pixels);
    
    glBindTexture(GL_TEXTURE_2D, 0);
    
    SDL_FreeSurface(image);
    
    return textureId;
}

void draw_BG(GLuint textureId){
	glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, textureId);
			glBegin(GL_QUADS);
				glColor3f(1, 1, 1);
	
				glTexCoord2f(0, 0);
				glVertex2f(0, 0);
				
				glTexCoord2f(1, 0);
				glVertex2f(3000, 0);
				
				glTexCoord2f(1, 1);
				glVertex2f(3000, 3000);
				
				glTexCoord2f(0, 1);
				glVertex2f(0, 3000);

			glEnd();
		glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);
}

void dessinRectangle(int filled){
	if (filled==1){ 
		glBegin(GL_QUADS);
			glVertex2f(0, 1);
			glVertex2f(1, 1);
			glVertex2f(1, 0);
			glVertex2f(0, 0);
		glEnd();
	}
	else{
		glBegin(GL_LINE_LOOP);
			glVertex2f(0, 1);
			glVertex2f(1, 1);
			glVertex2f(1, 0);
			glVertex2f(0, 0);
		glEnd();
	}
}

void drawDoor (GLuint doorTex, carre door)
{
	glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, doorTex);
			glBegin(GL_QUADS);
				glColor3f(1, 1, 1);
	
				glTexCoord2f(0, 1);
				glVertex2f(door.x, door.y);
				
				glTexCoord2f(1, 1);
				glVertex2f(door.x+door.width, door.y);
				
				glTexCoord2f(1, 0);
				glVertex2f(door.x+door.width, door.y+door.height);
				
				glTexCoord2f(0, 0);
				glVertex2f(door.x, door.y+door.height);

			glEnd();
		glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);
}

void dessinRepere(){
	glBegin(GL_LINES);
		glColor3ub(255, 0, 0);
			glVertex2f(0, 0);
			glVertex2f(0, 1);
		glColor3ub(0, 255, 0);
			glVertex2f(0, 0);
			glVertex2f(1, 0);
	glEnd();
}

void drawFleche(carre box){
	//Affichage de la flèche
	glBegin(GL_TRIANGLES);
		glVertex2f(box.x + box.width/2 - 10, box.y + box.height + 20);
		glVertex2f(box.x + box.width/2 + 10, box.y + box.height + 20);
		glVertex2f(box.x + box.width/2, box.y + box.height + 10);
	glEnd();
}

void drawShadow(carre box_ombre, carre box_sol, int decal_gauche, int decal_droite, float opacity){
	//Affichage des ombres des plate-formes
	glBegin(GL_POLYGON);
		glPushMatrix();
			glColor4f(0.0, 0.0, 0.0, opacity);
				glVertex2f(box_ombre.x - decal_gauche, box_sol.y + box_sol.height);
				glVertex2f(box_ombre.x, box_ombre.y + box_ombre.height);
				glVertex2f(box_ombre.x + box_ombre.width, box_ombre.y + box_ombre.height);
				glVertex2f(box_ombre.x + box_ombre.width, box_ombre.y);
				glVertex2f(box_ombre.x + box_ombre.width - decal_droite, box_sol.y + box_sol.height);
		glPopMatrix();
	glEnd();
}

void drawShadow2(carre box_ombre, float opacity){
	float i;
	glBegin(GL_LINE_STRIP);
		glPushMatrix();
			for(i=0;i<200;i++){
				glColor4f(0.0, 0.0, 0.0, opacity-(i/100));
				glVertex2f(box_ombre.x - i, (box_ombre.y + box_ombre.height)- i);
				glVertex2f(box_ombre.x - i, box_ombre.y- i);
				glVertex2f((box_ombre.x + box_ombre.width)- i, box_ombre.y - i);
			}
		glPopMatrix();
	glEnd();
}

void drawShadowcarre(carre box_ombre, int decal_gauche, int decal_bas, float opacity){
	//Affichage des ombres des plate-formes
	glBegin(GL_POLYGON);
		glPushMatrix();
			glColor4f(0.0, 0.0, 0.0, opacity);
				glVertex2f(box_ombre.x - decal_gauche, box_ombre.y - decal_bas);
				glVertex2f(box_ombre.x - decal_gauche, box_ombre.y + box_ombre.height - decal_bas);
				glVertex2f(box_ombre.x + box_ombre.width - decal_gauche, box_ombre.y + box_ombre.height - decal_bas);
				glVertex2f(box_ombre.x + box_ombre.width - decal_gauche, box_ombre.y - decal_bas);
		glPopMatrix();
	glEnd();
}


int collision_form(carre box){
	//Collision avec les empruntes de carré
	if((box.y >= box.y_fin + 1)
	|| (box.y + box.height <= box.y_fin)
	|| (box.x + box.width <= box.x_fin + (box.width/2)) 
	|| (box.x  >= box.x_fin + box.width - (box.width/2))){
		return 0;
	}
	else{
		return 1;
	}
}

int collision (carre box1, carre box2){
	//Variables des côtés des deux boîtes
	int left_box1, left_box2;
	int right_box1, right_box2;
	int top_box1, top_box2;
	int bottom_box1, bottom_box2;
	
	//Calcul des côtés de box1
	left_box1 = box1.x;
	right_box1 = box1.x + box1.width;
	bottom_box1 = box1.y;
	top_box1 = box1.y + box1.height;
	
	//Calcul des côtés de box2
	left_box2 = box2.x;
	right_box2 = box2.x + box2.width;
	bottom_box2 = box2.y;
	top_box2 = box2.y + box2.height;
	
	if(((bottom_box1 >= top_box2)   // La partie basse de Box 1 est au dessus de la partie haute de Box 2
    || (top_box1 <= bottom_box2)    // La partie haute de Box 1 est en dessous de la partie basse de Box 2
    || (right_box1 <= left_box2)   // La partie gauche de Box 1 est à droite de la partie droite de Box 2
    || (left_box1 >= right_box2))) // La partie droite de Box 1 est à gauche de la partie gauche de Box 2
    {
		return 0; 
	}
	else
	{
        return 1; 
	}
}

level initializeFromFile(const char* path, int *nbPersos, int *nbPlatforms, int *nbDoors, carre **squares, carre **platforms, carre **doors){
	
	level l ;
	l.scale = 1;
	
	FILE *file = NULL;
	
    file = fopen(path, "r");


    if (file == NULL)
    {
    	printf("Le fichier de niveau n'a pas pu être ouvert\n");
    	exit(1);
    }

    int i = 0;
    int w, h, r, v, b, x, y, dec, shadow, type, toLvl, beginPlatform, finalPlatform, doorPlatform;
    float jump_power, level_scale;
	
	if(fscanf(file, "%*s : %f", &level_scale)==1){
		l.scale = level_scale;
	}
	else
		printf("Failed to assign the right inputs.\n");
		
    if(fscanf(file, "%*s : %d", nbPlatforms)==1){
		*platforms = (carre *) malloc(*nbPlatforms * sizeof(carre));
		if (platforms == NULL)
		{
			printf("NOT ENOUGH MEMORY");
			exit(1);
		}

	}
	else
		printf("Failed to assign the right inputs.\n");

    for (i = 0; i < *nbPlatforms; ++i)
    {
    	if(fscanf(file, "%d %d %d %d %d %d", &w, &h, &x, &y, &shadow, &type)==6){
			(*platforms)[i] = createPlatform(w, h, x, y, shadow, type);
		}
		else
			printf("Failed to assign the right inputs.\n");
    }

    if(fscanf(file, "%*s : %d", nbDoors)==1){
		*doors = (carre *) malloc(*nbDoors * sizeof(carre));
		if (doors == NULL)
		{
			printf("NOT ENOUGH MEMORY");
			exit(1);
		}
	}
	else
		printf("Failed to assign the right inputs.\n");

    for (i = 0; i < *nbDoors; ++i)
    {
    	if(fscanf(file, "%d %d %d %d %d %d", &w, &h, &x, &doorPlatform, &shadow, &toLvl)==6){
			(*doors)[i] = createDoor(w, h, x, (*platforms)[doorPlatform].y + (*platforms)[doorPlatform].height, shadow, toLvl);
		}
		else
			printf("Failed to assign the right inputs D .\n");
    }

    //printf("Number of platforms : %d\n", *nbPlatforms);
    
    if(fscanf(file, "%*s : %d", nbPersos)==1){ 
		*squares = (carre*) malloc(*nbPersos * sizeof(carre));
		if (squares == NULL)
		{
			printf("NOT ENOUGH MEMORY");
			exit(1);
		}
	}
	else
		printf("Failed to assign the right inputs.\n");
	
	//printf("Number of characters : %d\n", *nbPersos);

    for (i = 0; i < *nbPersos; ++i)
    {
    	if(fscanf(file, "%d %d %d %d %d %d %f %d %d %d", &w, &h, &x, &beginPlatform, &finalPlatform, &dec, &jump_power, &r, &v, &b)==10){
			(*squares)[i] = createCarre(w, h, x, (*platforms)[beginPlatform].y + (*platforms)[beginPlatform].height, (*platforms)[finalPlatform].x + dec, (*platforms)[finalPlatform].y + (*platforms)[finalPlatform].height, jump_power, r, v, b);
			printf("%d\n", (*squares)[i].height);
		}
		else
			printf("Failed to assign the right inputs.\n");
    }
    
    fclose(file);
    
    return l;
}
