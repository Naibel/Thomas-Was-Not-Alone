#ifndef __FUNCTIONS__H
#define __FUNCTIONS__H

static const unsigned int BIT_PER_PIXEL = 32;
static const Uint32 FRAMERATE_MILLISECONDS = 1000 / 60;

typedef struct carre{
	int width;
	int height;
	int color_R;
	int color_V;
	int color_B;
	int x_fin;
	int y_fin;
	int x;
	int y;
	float dx;
	float dy;
	int onGround;
	int onSquare;
	int underSquare;
	int onPlatform;
	int jumpPower;
	int shadowed;
	int type;
	int toLvl;
}carre;

typedef struct level{
	float scale;
}level;

carre createCarre(int w, int h, int x, int y, int x_f, int y_f, float jump_power, int r, int v, int b);

carre createPlatform(int w, int h, int x, int y, int shadowed, int type);

carre createDoor(int w, int h, int x, int y, int shadowed, int tL);

void reshape(int winWidth, int winHeight, carre c, level l);

void setVideoMode(int winWidth, int winHeight, carre c, level l);

GLuint Init_Texture(char * path);

void draw_BG(GLuint textureId);

void dessinRectangle(int filled);

void dessinRepere();

void drawDoor (GLuint doorTex, carre door);

void drawFleche(carre box);

void drawShadow(carre box_ombre, carre box_sol, int decal_gauche, int decal_droite, float opacity);

void drawShadow2(carre box_ombre, float opacity);

void drawShadowcarre(carre box_ombre, int decal_gauche, int decal_droite, float opacity);

int collision (carre box1, carre box2);

int collision_form(carre box);

/* Format du fichier txt
  * Platforms : 'Nombre_de_platformes'
  * 'width' 'height' 'x' 'y' 'shadow' 'type' #type[0 = sol, 1 = eau]
  * Doors : 'Nombre de portes'
  *  'width' 'height' 'x' 'platform' 'shadow' 'toLvl'
  * Squares : 'Nombres de personnages'
  * 'width' 'height' 'x' 'beginPlatform' 'finalPlatform' 'offset' 'jump_power' 'r' 'v' 'b'*/
level initializeFromFile(const char* path, int *nbPersos, int *nbPlatforms, int *nbDoors, carre **squares, carre **platforms, carre **doors);
#endif
