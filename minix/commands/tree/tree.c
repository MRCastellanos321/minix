
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>

void abrirRuta (char *ruta, int nivel)
{
 DIR *directorioabierto = opendir(ruta);
 if (directorioabierto == NULL)
  {
  printf("Error\n");
  return;
  }
  struct dirent *nombreActual;
while ((nombreActual = readdir(directorioabierto)) != NULL)
{
char *nombre = nombreActual->d_name;
 if (nombre[0] == '.')
  {
     continue;
  }
 for (int i = 0; i < nivel; i++) 
  {
     printf("   ");
  }
  
 char rutaCompleta[1024];
 snprintf(rutaCompleta, sizeof(rutaCompleta), "%s/%s", ruta, nombre);

 struct stat rutacomprobacion;
  if (lstat(rutaCompleta, &rutacomprobacion) == -1)
    {
      continue;
    }
  if (S_ISLNK(rutacomprobacion.st_mode))
    {
      printf("%s\n", nombre);
      continue;
    }

 DIR *prueba = opendir(rutaCompleta);
  if (prueba != NULL) 
  {  closedir(prueba);
     printf("%s\n",nombre);
     abrirRuta(rutaCompleta, nivel +1);
  } 
  else 
  {
     printf("%s\n", nombre);
  } 

}
closedir(directorioabierto);
}

int main(int argc, char **argv)
{
 char *primeraRuta = ".";
 if(argc > 1)
  {
   primeraRuta = argv[1]; 
  }
  printf("%s\n",primeraRuta);
  abrirRuta(primeraRuta, 0);
  return 0;
 }
