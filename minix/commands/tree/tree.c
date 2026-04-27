
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
  printf("Error");
  return;
}
struct dirent *nombreActual;
while ((nombreActual = readdir(directorioabierto)) != NULL)
{
char *nombre = nombreActual->d_name;
if (strcmp(nombre, "..") == 0 || strcmp(nombre, ".") == 0) 
{continue;
}

char rutaCompleta[1024];
snprintf(rutaCompleta, sizeof(rutaCompleta), "%s/%s", ruta, nombre);

 DIR *prueba = opendir(rutaCompleta);
  if (prueba != NULL) 
  { closedir(prueba);
    printf("%s\n",nombre);
  abrirRuta(rutaCompleta);
  } 
  else 
  {
   printf("%s\n",nombre);
  } 

}
closedir(directorioabierto);
}

int main(int argc, char **argv)
{
char *primeraRuta = ".";
if(argc > 1){
 primeraRuta = argv[1]; 
 }
 printf("%s\n",primeraRuta);
  abrirRuta(primeraRuta);
  return 0;
}
