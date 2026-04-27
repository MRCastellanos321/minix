
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
DIR *directorioabierto = opendir(ruta);
if (directorioabierto == NULL)
{
  printf(Error)
  return;
}
struct dirent *nombreActual;
while ((nombreActual = readdir(directorioabierto))=! NULL)
{
char *nombre = nombreActual-> d_nombre;
if (strcmp(nombre, "..") == 0 || strcmp(nombre, ".") == 0) 
{continue;
}

char rutaCompleta[1024];
snprintf(rutaCompleta, sizeof(rutaCompleta), "%s/%s", ruta, nombre);

 DIR *prueba = opendir(rutaCompleta);
  if (prueba != NULL) 
  { closedir(prueba);
    printf(nombre);
  abrirRuta(rutaCompleta);
  } 
  else 
  {
   printf(nombre);
  } 

}
closedir(directorioabierto)
}

int main(int argc, char **argp)
{
char *primeraRuta = ".";
if(argc > 1){
 primeraRuta = argv[1]; 
 }
 printf(primeraRuta);
  abrirRuta(primeraRuta);
  return 0;
}
