
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>

void abrirRuta (char *ruta)
{
DIR *directorioabierto = opendir(ruta)

variablenombre = readdir(directorioabierto)-> d.nombre

/*comprobar si . o .. por carpetas de sistema*/

printf(nombre)
/*
parte recursiva si es carpeta
*/
closedir(directorioabierto)
}

int main(int argc, char **argp)
{
  abrirRuta(/*argumento*/)
}
