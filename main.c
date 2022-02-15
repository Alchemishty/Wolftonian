#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>

#define PI 3.1415926535
#define P2 PI/2
#define P3 3*PI/2
#define DR 0.0174533

float playerX, playerY, pdx, pdy, pAngle;

int mapX = 8, mapY = 8, mapS = 64;
int map[] = 
{
	1, 1, 1, 1, 1, 1, 1, 1,
	1, 0, 0, 1, 0, 0, 0, 1,
	1, 0, 0, 1, 0, 1, 0, 1,
	1, 0, 1, 1, 0, 0, 0, 1,
	1, 0, 1, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 1,
	1, 0, 0, 0, 0, 0, 0, 1,
	1, 1, 1, 1, 1, 1, 1, 1,
};

void drawMap2D() {
	int x, y, xo, yo;
	for (y = 0; y < mapY; y++) {
		for (x = 0; x < mapX; x++) {
			if (map[y * mapY + x] == 1) {
				glColor3f(0,0,0);
			} else {
				glColor3f(1,1,1);
			}
			xo = x * mapS;
			yo = y * mapS;
			glBegin(GL_QUADS);
			glVertex2i(xo    + 1, yo     +1);
			glVertex2i(xo    + 1, yo+mapS-1);
			glVertex2i(xo+mapS-1, yo+mapS-1);
			glVertex2i(xo+mapS-1, yo     +1);
			glEnd();
		}
	}
}

void drawPlayer() {
	glColor3f(1,1,0);
	glPointSize(8);
	glBegin(GL_POINTS);
	glVertex2i(playerX, playerY);
	glEnd();
	
	glLineWidth(3);
	glBegin(GL_LINES);
	glVertex2i(playerX, playerY);
	glVertex2i(playerX + pdx * 5, playerY + pdy * 5);
	glEnd();
}

float dist(float x1, float y1, float x2, float y2, float ang) {
	return sqrt((x2-x1) * (x2 - x1) + (y2 - y1) * (y2 - y1));
}

void drawRays2D() {
	//glColor3f(0,1,1); glBegin(GL_QUADS); glVertex2i(526,  0); glVertex2i(1006,  0); glVertex2i(1006,160); glVertex2i(526,160); glEnd();	
 	//glColor3f(0,0,1); glBegin(GL_QUADS); glVertex2i(526,160); glVertex2i(1006,160); glVertex2i(1006,320); glVertex2i(526,320); glEnd();
 
	int r, mx, my, mp, dof;
	float rayX, rayY, rayA, xo, yo, distT;
	
	rayA = pAngle - DR * 30;
	if (rayA < 0) {
		rayA += 2 * PI;
	}
	if (rayA > 2 * PI) {
		rayA -= 2 * PI;
	}
	
	for (r = 0; r < 60; r++) {
		//Horizontal Collision Check
		dof = 0;
		float distH = 99999999, hx = playerX, hy = playerY;
		float aTan = -1/tan(rayA);
		
		if (rayA > PI) { //looking down
			rayY = (((int)playerY >> 6) << 6) - 0.0001;
			rayX = (playerY - rayY) * aTan + playerX;
			yo = -64;
			xo = -yo * aTan;
		}
		
		if (rayA < PI) { //looking up
			rayY = (((int)playerY >> 6) << 6) + 64;
			rayX = (playerY - rayY) * aTan + playerX;
			yo =  64;
			xo = -yo * aTan;
		}
		
		if (rayA == 0 || rayA == PI) { //looking straight left or right
			rayX = playerX;
			rayY = playerY;
			dof = 8;
		}
		
		while (dof < 8) {
			mx = (int) (rayX) >> 6;
			my = (int) (rayY) >> 6;
			mp = my * mapX + mx;
			
			if (mp > 0 && mp < mapX * mapY && map[mp] == 1) { //wall hit
				hx = rayX;
				hy = rayY;
				distH = dist(playerX, playerY, hx, hy, rayA);
				dof = 8;
			}
			else {
				rayX += xo;
				rayY += yo;
				dof += 1;
			}
		}

		//Vertical Collision Check
		dof = 0;
		float distV = 99999999, vx = playerX, vy = playerY;
		float nTan = -tan(rayA);
		
		if (rayA > P2 && rayA < P3) { //looking left
			rayX = (((int)playerX >> 6) << 6) - 0.0001;
			rayY = (playerX - rayX) * nTan + playerY;
			xo = -64;
			yo = -xo * nTan;
		}
		
		if (rayA < P2 || rayA > P3) { //looking right
			rayX = (((int)playerX >> 6) << 6) + 64;
			rayY = (playerX - rayX) * nTan + playerY;
			xo =  64;
			yo = -xo * nTan;
		}
		
		if (rayA == 0 || rayA == PI) { //looking straight up or down
			rayX = playerX;
			rayY = playerY;
			dof = 8;
		}
		
		while (dof < 8) {
			mx = (int) (rayX) >> 6;
			my = (int) (rayY) >> 6;
			mp = my * mapX + mx;
			
			if (mp > 0 && mp < mapX * mapY && map[mp] == 1) { //wall hit
				vx = rayX;
				vy = rayY;
				distV = dist(playerX, playerY, vx, vy, rayA);
				dof = 8;
			}
			else { //next block line
				rayX += xo;
				rayY += yo;
				dof += 1;
			}
		}
		
		if (distV < distH) { //Vertical Wall Hit First
			rayX = vx;
			rayY = vy;
			distT = distV;
			glColor3f(0.9,0,0);
		} 
		if (distV > distH){  //Horizontal Wall Hit First
			rayX = hx;
			rayY = hy;
			distT = distH;
			glColor3f(0.7,0,0);
		}
		
		glColor3f(1,0,0);
		glLineWidth(3);
		glBegin(GL_LINES);
		glVertex2i(playerX, playerY);
		glVertex2i(rayX, rayY);
		glEnd();
		
		//Draw 3D Walls
		float cosineA = pAngle - rayA;
		if (cosineA < 0) {
			cosineA += 2 * PI;
		}
		if (cosineA > 2 * PI) {
			cosineA -= 2 * PI;
		}
		distT *= cos(cosineA); // fix fisheye effect
		
		float lineH = (mapS * 320) / distT;
		if (lineH > 320) {
			lineH = 320;
		}
		float lineO = 160 - (lineH/2);
		
		glLineWidth(8);
		glBegin(GL_LINES);
		glVertex2i(r * 8 + 530, lineO);
		glVertex2i(r * 8 + 530, lineO + lineH);
		glEnd();
		
		rayA += DR;
		
		if (rayA < 0) {
			rayA += 2 * PI;
		}
		if (rayA > 2 * PI) {
			rayA -= 2 * PI;
		}
		
	}
}

void display() {
 glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 drawMap2D();
 drawPlayer();
 drawRays2D();
 glutSwapBuffers();
}

void init() {
	glClearColor(0.3, 0.3, 0.3, 0);
	gluOrtho2D(0, 1024, 512, 0);
	playerX = 300, playerY = 300, pAngle = 0;
	pdx = cos(pAngle) * 5;
	pdy = sin(pAngle) * 5;
}

void controls(unsigned char key, int x, int y) {
	if (key == 'a') {
		pAngle -= 0.1;
		if (pAngle < 0) {
			pAngle += 2 * PI;
		}
		pdx = cos(pAngle) * 5;
		pdy = sin(pAngle) * 5;
	}
	if (key == 'd') {
		pAngle += 0.1;
		if (pAngle > 2 * PI) {
			pAngle -= 2 * PI;
		}
		pdx = cos(pAngle) * 5;
		pdy = sin(pAngle) * 5;
	}
	if (key == 'w') {
		playerX += pdx;
		playerY += pdy;
	}
	if (key == 's') {
		playerX -= pdx;
		playerY -= pdy;
	}
	glutPostRedisplay();
}

int main(int argc, char** argv) { 
 glutInit(&argc, argv);
 glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
 glutInitWindowSize(1024,512);
 glutCreateWindow("Wolftonian");
 init();
 glutDisplayFunc(display);
 glutKeyboardFunc(controls);
 glutMainLoop();
 return 0;
}
