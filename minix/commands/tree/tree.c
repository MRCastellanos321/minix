
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

struct dirent *actual
actual = readdir(directorioabierto)-> d_nombre
while (actual =! null)
{
if (strcmp(nombre, "..") == 0 || strcmp(nombre, ".") == 0) 
{continue}
printf(nombre)
/*NO, me hace fallta ruta completa y nombre/*
/*
parte recursiva si es carpeta
*/
S_ISDIR()
abrirRuta()
closedir(directorioabierto)
}

int main(int argc, char **argp)
{
char *primeraRuta
 primeraRuta = argv[1]
  abrirRuta(primeraRuta)
  
  return 0
}
