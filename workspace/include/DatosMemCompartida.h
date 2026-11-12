#pragma once 
#include "Esfera.h"
#include "Raqueta.h"
//Ruta del fichero proyectado en memoria
#define RUTA_MEMORIA "/tmp/datosBot"
class DatosMemCompartida{
public: 
Esfera esfera;
Raqueta raqueta1;
int accion; //1 arriba, 0 nada, -1 abajo
};
