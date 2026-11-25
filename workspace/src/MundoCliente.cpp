// MundoCliente.cpp: implementation of the CMundoCliente class.
//
//////////////////////////////////////////////////////////////////////
#include <fstream>
#include "MundoCliente.h"
#include "glut.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include<fcntl.h>
#include<unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMundoCliente::CMundoCliente()
{
	Init();
}

CMundoCliente::~CMundoCliente()
{
close(fifo_coord);
close(fifo_teclas);
unlink("/tmp/teclasFIFO");
}

void CMundoCliente::InitGL()
{
	//Habilitamos las luces, la renderizacion y el color de los materiales
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);
	
	glMatrixMode(GL_PROJECTION);
	gluPerspective( 40.0, 800/600.0f, 0.1, 150);
}

void print(char *mensaje, int x, int y, float r, float g, float b)
{
	glDisable (GL_LIGHTING);

	glMatrixMode(GL_TEXTURE);
	glPushMatrix();
	glLoadIdentity();

	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0, glutGet(GLUT_WINDOW_WIDTH), 0, glutGet(GLUT_WINDOW_HEIGHT) );

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glColor3f(r,g,b);
	glRasterPos3f(x, glutGet(GLUT_WINDOW_HEIGHT)-18-y, 0);
	int len = strlen (mensaje );
	for (int i = 0; i < len; i++) 
		glutBitmapCharacter (GLUT_BITMAP_HELVETICA_18, mensaje[i] );
		
	glMatrixMode(GL_TEXTURE);
	glPopMatrix();

	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();

	glEnable( GL_DEPTH_TEST );
}
void CMundoCliente::OnDraw()
{
	//Borrado de la pantalla	
   	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Para definir el punto de vista
	glMatrixMode(GL_MODELVIEW);	
	glLoadIdentity();
	
	gluLookAt(0.0, 0, 17,  // posicion del ojo
		0.0, 0.0, 0.0,      // hacia que punto mira  (0,0,0) 
		0.0, 1.0, 0.0);      // definimos hacia arriba (eje Y)    

	/////////////////
	///////////
	//		AQUI EMPIEZA MI DIBUJO
	char cad[100];
	sprintf(cad,"Jugador1: %d",puntos1);
	print(cad,10,0,1,1,1);
	sprintf(cad,"Jugador2: %d",puntos2);
	print(cad,650,0,1,1,1);
	int i;
	for(i=0;i<paredes.size();i++)
		paredes[i].Dibuja();

	fondo_izq.Dibuja();
	fondo_dcho.Dibuja();
	jugador1.Dibuja();
	jugador2.Dibuja();
	esfera.Dibuja();

	/////////////////
	///////////
	//		AQUI TERMINA MI DIBUJO
	////////////////////////////

	//Al final, cambiar el buffer
	glutSwapBuffers();
}

void CMundoCliente::OnTimer(int value)
{	
//leo las coordenadas que manda el servidor
char cad[200];
if(read(fifo_coord, cad, sizeof(cad))<=0){
printf("El servidor ha terminado\n");
exit(0);
}
sscanf(cad,"%f %f %f %f %f %f %f %f %f %f %d %d", &esfera.centro.x,&esfera.centro.y, &jugador1.x1,&jugador1.y1,&jugador1.x2,&jugador1.y2, &jugador2.x1,&jugador2.y1,&jugador2.x2,&jugador2.y2, &puntos1, &puntos2);
pdatos->esfera = esfera;
pdatos->raqueta1=jugador1;
if (pdatos->accion == 1)
OnKeyboardDown('w',0,0);
else if (pdatos->accion==-1)
OnKeyboardDown('s',0,0);
}

void CMundoCliente::OnKeyboardDown(unsigned char key, int x, int y)
{
char cad[100];
sprintf(cad,"%c",key);
write(fifo_teclas, cad, sizeof(cad));
	switch(key)
	{
//	case 'a':jugador1.velocidad.x=-1;break;
//	case 'd':jugador1.velocidad.x=1;break;
	case 's':jugador1.velocidad.y=-4;break;
	case 'w':jugador1.velocidad.y=4;break;
	case 'l':jugador2.velocidad.y=-4;break;
	case 'o':jugador2.velocidad.y=4;break;

	}
}

void CMundoCliente::Init()
{
	Plano p;
//pared inferior
	p.x1=-7;p.y1=-5;
	p.x2=7;p.y2=-5;
	paredes.push_back(p);

//superior
	p.x1=-7;p.y1=5;
	p.x2=7;p.y2=5;
	paredes.push_back(p);

	fondo_izq.r=0;
	fondo_izq.x1=-7;fondo_izq.y1=-5;
	fondo_izq.x2=-7;fondo_izq.y2=5;

	fondo_dcho.r=0;
	fondo_dcho.x1=7;fondo_dcho.y1=-5;
	fondo_dcho.x2=7;fondo_dcho.y2=5;

	//a la izq
	jugador1.g=0;
	jugador1.x1=-6;jugador1.y1=-1;
	jugador1.x2=-6;jugador1.y2=1;

	//a la dcha
	jugador2.g=0;
	jugador2.x1=6;jugador2.y1=-1;
	jugador2.x2=6;jugador2.y2=1;

//creo la tuberia de teclas antes de abrir la de coordenadas
unlink("/tmp/teclasFIFO");
if (mkfifo("/tmp/teclasFIFO",0600)<0){
perror("No puede crearse el FIFO de teclas");
exit(1);
}
fifo_coord=open("/tmp/coordenadasFIFO", O_RDONLY);
if(fifo_coord<0){
perror("No puede abrirse el FIFO de coordenadas (lanza antes el servidor)");
exit(1);
}
fifo_teclas=open("/tmp/teclasFIFO", O_WRONLY);
if(fifo_teclas<0){
perror("No puede abrirse el FIFO de teclas");
exit(1);
}
int fd = open(RUTA_MEMORIA, O_RDWR|O_CREAT|O_TRUNC, 0666);
if (fd<0){
perror("No puede crearse el fichero de memoria");
exit(1);
}
ftruncate(fd, sizeof(DatosMemCompartida));
pdatos = (DatosMemCompartida*)mmap(NULL, sizeof(DatosMemCompartida), PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
if (pdatos == MAP_FAILED) { 
perror("Error en la proyeccion mmap");
exit(1);
}
close(fd);
}
