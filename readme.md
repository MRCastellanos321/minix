# Integrantes:

-Liz Cartaya Salabarría C211
-Maya Ramón Castellanos C212

# 1. Introducción

El presente proyecto se enmarca en la asignatura Sistemas Operativos de segundo año de la carrera Ciencia de la Computación. Su objetivo principal es el aprendizaje práctico mediante la implementación, modificación y análisis de componentes reales del sistema operativo, abordando aspectos como la gestión de procesos, la planificación de la CPU, la gestión de memoria, los mecanismos de concurrencia a través de hilos y el acceso al sistema de archivos mediante llamadas al sistema.

Como plataforma de trabajo se emplea MINIX 3. Su naturaleza modular y la disponibilidad de su código fuente permiten modificar, extender y analizar componentes internos reales, como el planificador de procesos, las bibliotecas de hilos y el sistema de entrada/salida.

El trabajo se estructuró en varios hitos que integran progresivamente los conceptos mencionados. Primero, personalizamos el mensaje de bienvenida del sistema. Luego, abordamos la depuración de un bug en la implementación de `pthread_mutex_trylock`, lo que requirió comprender la relación entre la capa de compatibilidad `pthread` y la implementación nativa `mthread` de MINIX. Implementamos el comando `tree`, que recorre recursivamente el sistema de archivos utilizando llamadas al sistema como `opendir`, `readdir` y `stat`. Finalmente, modificamos el planificador de MINIX para penalizar procesos con uso intensivo de CPU.

En las siguientes secciones se documentan detalladamente las decisiones de diseño, los cambios realizados en el código, las dificultades encontradas y las pruebas de validación ejecutadas para cada uno de estos componentes.

# 2.Desarrollo y resultados por componente

## 2.1. Instalación y configuración de VirtualBox y MINIX

### 2.1.1 Software utilizado

El proyecto se desarrolló sobre una laptop física con Ubuntu 24.04 LTS (sistema anfitrión). Sobre esta, se instaló Oracle VirtualBox 7.2.6 y se descargó la imagen ISO de MINIX 3.4.0 desde el sitio oficial.

Creamos una máquina virtual a la que nombramos Project Minix con MINIX 3 como sistema operativo invitado, con una memoria Ram de 2048 megabytes y con tres CPUs.

### 2.1.2 Instalación y Configuración

Instalamos, seguimos la configuración sugerida y quitamos el Iso de instalación.

Instalamos los binarios sugeridos y para eso primero actualizamos la lista de paquetes

```bash
minix# pkgin update
```

Instalamos git para poder trabajar con repositorios

```bash
minix# pkgin install git-base
```

Y siguiendo la documentación oficial utilizamos el siguiente comando para instalar lo mínimo necesario

```bash
minix# pkgin_sets
```

Procedemos a clonar el repositorio para poder obtener el código del sistema, para ello antes realizamos los comandos:

```bash
minix# cd /usr/src/
minix# pwd
```

y ya ubicados en esta carpeta clonamos el repositorio

```bash
minix# git clone https://github.com/MRCastellanos321/minix

```

Tenemos un problema con el cerificado de SSL que reolvemos de la siguiente manera

```bash
minix# git -c http.sslVerify=false clone https://github.com/MRCastellanos321/minix

```

Mientras se clonaba tuvimos un problema que impedía que el repositorio se clonara correctamente. Esto se debía a que el tamaño virtual de nuestra máquina era de solo 2048 megabytes y resultaba insuficiente. Para eso modificamos el espacio virtual a unos 8 Gb y volvimos a clonar el repositorio esta vez con éxito.

De esta forma ya quedó todo listo para trabajar, hacer cambios y compilar.

### 2.1.3 Reenvío de puertos

Para poder acceder a la máquina virtual desde la terminal de Ubuntu de forma cómoda, se configuró el reenvío de puertos en VirtualBox, redirigiendo el tráfico del puerto 2222 del host al puerto 22 del invitado (MINIX). De esta forma, es posible conectarse por SSH sin necesidad de abrir puertos adicionales en el sistema anfitrión.
Desde la interfaz gráfica de VirtualBox fuimos a: Settings -> Network -> Adapter 1 (NAT) -> Port Forwarding
y creamos una nueva regla con nombre: ssh, protocol: tcp, Host Port: 2222 y Guest Port: 22 .

Actualizamos el gestor de paquetes y se instalaron las herramientas necesarias para la compilación y edición:

```bash
minix# pkgin update
minix# pkgin install openssh
```

Configuramos el servidor SSH dentro de MINIX para permitir conexiones remotas:

```bash
minix# cp /usr/pkg/etc/rc.d/sshd /etc/rc.d/
minix# printf 'sshd=YES\n' >> /etc/rc.conf
minix# printf 'PermitRootLogin yes\n' >> /usr/pkg/etc/ssh/sshd_config
```

Creamos una contraseña de seguridad para permitir la conexión entre ambas terminales: `minix# passwd`

Iniciamos el servidor SSH en modo depuración:

```bash
minix# /usr/pkg/sbin/sshd -d
```

Finalmente, desde la terminal de Ubuntu, nos conectamos a MINIX:

```bash
ssh root@127.0.0.1 -p 2222
```

De esta forma podemos trabajar de manera más agradable desde la terminal de Ubuntu.

### 2.1.4 Carpeta compartida

Para facilitar la transferencia de archivos entre MINIX y Ubuntu, se configuró una carpeta compartida en VirtualBox.

Desde Ubuntu creamos la carpeta: `mkdir ~/minix_compartida `
Desde la interfaz gráfica de VirtualBox fuimos a: Machine -> Settings -> Shared Folders
y creamos una nueva carpeta compartida y le agregamos la ruta de la carpeta en Ubuntu, de nombre le pusimos "compartida" y marcamos las opciones "Automontar" y "Hacer permanente".

Por último, montamos la carpeta dentro de MINIX

```bash
minix# mkdir -p /mnt/compartida
minix# mount -t vbfs -o share=compartida none /mnt/compartida
```

## 2.2 Personalización del mensaje de bienvenida

Para completar esta tarea debimos modificar el mensaje de bienvenida tanto en el sistema MINIX de la máquina virtual como en el repositorio de git.
Siguiendo las indicaciones de la orientación del proyecto modificamos el archivo /etc/motd en MINIX utilizando el editor de texto vi.

```bash
minix# vi /etc/motd
```

Eliminamos el texto contenido en el archivo utilizando, dentro de vi, el comando `dd` que borra una línea completa. Luego escribimos nuestro mensaje personalizado:

```bash
Bienvenido a MINIX 3

Facultad de Matematica y Computacion
Universidad de La Habana
```

Guardamos el mensaje con el comando `:wq `y salimos del editor.

Reiniciamos la máquina virtual escribiendo reboot en la terminal y una vez que iniciada podemos ver el mensaje anterior.
Una vez modificado el mensaje en la máquina virtual para que el cambio se mantuviera en futuras compilaciones del sistema, replicamos la modificación en /usr/src/minix/etc/motd, que es el archivo fuente del mensaje de bienvenida.

```bash
minix# cp /etc/motd /usr/src/minix/etc/motd
```

Luego recompilamos el sistema y copiamos el archivo fuente recompilado a la carpeta compartida:

```bash
minix# cp /etc/motd /mnt/compartida/
```

desde la cual copiamos dicho archivo a la carpeta del repositorio en Ubuntu, para a subir los cambios.

## 2.3 Depuración de un bug en pthread

Creamos un archivo test_mutex.c con el programa de prueba de referencia que tenemos en la orientacion del proyecto y al compilar y ejecutar este test

```bash
minix# cd /root
minix# cc -o test_mutex test_mutex.c -lmthread
minix# ./test_mutex
```

notamos que el programa se queda colgado desde la primera llamada.

Para encontrar el error localizamos el archivo pthread_compat.c

```bash
minix# find /usr/src -name "pthread_compat.c" 2>/dev/null
```

Revisamos el codigo siguiendo la ruta antes encontrada

```bash
minix# nano /usr/src/minix/minix/lib/libmthread/pthread_compat.c
```

Allí pudimos ver el código de la función pthread_mutex_trylock

```bash
int pthread_mutex_trylock(pthread_mutex_t *mutex)
{
    if (PTHREAD_MUTEX_INITIALIZER == *mutex) {
        mthread_mutex_init(mutex, NULL);
    }

    return pthread_mutex_trylock(mutex);
}
```

y notamos que la función luego de crear un mutex, si no existe, se llama a sí misma recursivamente creando una recursión infinita y haciendo que el programa quede colgado, en su lugar debería llamar al método mthread*mutex_trylock.
Esto se debe a que pthread es un estándar de programación para manejo de hilos (threads) en sistemas UNIX, pero MINIX no implementa pthread de forma nativa, sino que tiene su propio sistema de hilos llamado mthread (Minix Threads).
Para permitir la ejecución de programas escritos para el estándar pthread, MINIX incorpora una capa de compatibilidad en el archivo pthread_compat.c. Esta capa funciona como traductor, expone las funciones pthread*\_ y redirige las llamadas a las funciones equivalentes de mthread\_\_.

Cambiamos el código:

```diff
 int pthread_mutex_trylock(pthread_mutex_t *mutex)
 {
     if (PTHREAD_MUTEX_INITIALIZER == *mutex) {
         mthread_mutex_init(mutex, NULL);
     }
-    return pthread_mutex_trylock(mutex);
+    return mthread_mutex_trylock(mutex);
 }
```

Compilamos solo la biblioteca libmthread e instalamos la biblioteca compilada en el sistema (/usr/lib) para que los cambios reemplazaran versiones antiguas.

```bash
minix# cd /usr/src/minix/lib/libmthread
minix# make
minix# make install
```

Volvimos a compilar y ejecutar el test que creamos

```bash
minix# cd /root
minix# cc -o test_mutex test_mutex.c -lmthread
minix# ./test_mutex
```

y pudimos ver que obtuvimos las respuestas esperadas.

```bash
first trylock: 0 (OK)
second trylock: 11 (Resource deadlock avoided)
unlock: 0 (OK)
destroy: 0 (OK)
PASS
```

Para confirmar el significado del valor 11, consultamos el archivo /usr/include/sys/errno.h, donde comprobamos que en esta versión de MINIX la constante EDEADLK (Resource deadlock avoided) está definida con el valor 11.

## 2.4 Implementación del comando tree

Para crear el comando tree, accedemos a la carpeta commands en el código fuente de minix y creamos un nuevo directorio para contener el mismo. Creamos un archivo donde programaremos el comando en c y dentro comienza la implementación. Utilizamos las funciones que incluimos con `#include <dirent.h>`:

```bash
closedir(); recibe un *DIR y cierra el directorio abierto que apunta

opendir(); abre un directorio (recibe un texto ruta y devuelve un *DIR)

readdir(); recibe un *DIR y devuelve un struct dirent
```

En este caso nombreActual es el puntero al struct dirent llamado así por conveniencia, y es lo que da acceso a propiedades como d_name que vamos a usar para imprimir los nombres de archivos y carpetas en la terminal

La idea del programa es recorrer recursivamente a partir de la ruta dada (o "." en caso de que no haya parametro para el tree) cada carpeta y listarlas a ellas y sus archivos. El main que va a ser el punto de entrada nos permite ver la ruta decidida por el usuario a traves del argc y el argv. Según sea o no la cuenta de argumentos >1, pasamos al primer llamado de la función "abrirRuta" con el parámetro de la ruta inicial,y es esta la función que va a llamarse recursivamente hasta terminar. En ella se comprueba los archivos de inicio "." (pues son los especiales invisibles de sistema) y se tiene en cuenta si un directorio apunta a una carpeta o archivo para ver si hay que adentrarse recursivamente en este o no tras imprimir su nombre. LLevamos la variable "nivel" como cuenta para la indentación requerida al imprimir en la terminal. Se tiene en cuenta como requerido en la orientación comprobar los enlaces simbolicos. Un enlace simbólico es un puntero virtual a otro directorio que en casos especiales puede causar un bucle infinito (si apunta a la carpeta que lo contiene o un ancestro, esta se va a abrir por la función y encontrarse de nuevo con el enlace simbolico, recorriendo la misma ruta una y otra vez, por ejemplo). Para prevenir ese caso declaramos la variable tipo struct stat rutacomprobacion, donde copiamos los metadatos de rutaCompleta, para así aplicarle la función S_ISLNK(rutacomprobacion.st_mode) que nos va a devolver true en caso de que sea un enlace simbolico, permitiendonos continuar a la siguiente iteracion del ciclo mientras ignoramos el enlace.

El árbol también requiere de un archivo Makefile, el que usamos para compilar el sistema con el nuevo comando y ponerlo en funcionamiento.

## 2.5 Penalización por uso intensivo de CPU

Los diferentes procesos del sistema están en constante demanda de cpu para su funcionamiento, como pide la orientación, debemos modificar la forma en que la cpu se administra, implementando una penalización por su uso excesivo. Para ello accedimos a servers/sched en el código fuente y modificamos los archivos schedproc.h para añadir la variable quantum_count, que utilizaremos para la condición < N quantums (en este caso elegimos 3 como el ejemplo) Ahora quantum_count es accesible desde rmp en el código

Vamos al archivo sched.c donde están las funciones que vamos a modificar. Primeramente, el objetivo es aplicar una penalización gradual a los programas que alcancen los 3 quantums y superior, para ello cuanto rmp->quantum_count es mayor que 3 aumentamos en +1 su "priority", aunque en realidad a mayor número, la prioridad del sistema para este proceso es menor, o sea que estamos disminuyendo su prioridad para que no acapare cpu usando los quantums como medida. Comprobamos también que el aumento no supera el MIN_USER_Q definido por el sistema y reseteamos la cuenta de quantums a 0 una vez concluido esto. La función encargada de esta parte es `do_noquatums()`, que es la función que es llamada cuando un proceso agotó su quantum.

La función `balance_queues()`por otra parte se va a encargar de balancear, si un proceso tiene su cuenta de quantum en 0 (es decir que no se comportó como un proceso intensivo durante esa ventana) y su prioridad supera max_priority entonces disminuimos priority (o sea aumentamos su prioridad), además al concluir la ventana temporal se reinicia la cuenta de quantums a cero, de esa forma usamos el mecanismo ya existente de temporización.

Por último, `do_stop_scheduling` resetea la cuenta de los quantums a cero, y `do_start_scheduling()` la inicia ahí.

Una vez hechas estas modificaciones recompilamos el sistema con make y podemos verlo en funcionamiento con las pruebas descritas por la orientación.

Pruebas prácticas:

Para ambos casos creamos un código en c en la carpeta pruebas,y nos servimos de los comandos cc -o cpuBound cpuBound.c y ./cpuBound para compilarlo. En una segunda terminal, el comando `top () `(con | grep cpuBound para encontrar la línea) nos permitió ver el progreso de la penalizacion para la primera prueba (el loop infinito) que agota quantums sin terminar nunca y por ello es penalizado hasta priority 15. La segunda prueba que usa `sleep()` cada cierto tiempo y por tanto no es categorizado como un proceso cpu bound o intensivo para la cpu, en consecuencia no recibe ninguna penalización a su prioridad. También anadimos un printf opcional en el codigo sched.c para ver mejor el aumento de la penalización en el tiempo sin necesidad de los comandos anteriores.

# 3.Resultados globales y discusión

# 4. Conclusiones

El desarrollo del presente proyecto nos ha permitido cumplir con los objetivos propuestos, abordando principios fundamentales de los sistemas operativos como la gestión de procesos, la planificación de la CPU, la concurrencia mediante hilos y el acceso al sistema de archivos. MINIX 3 resultó ser una plataforma de estudio adecuada, gracias a la disponibilidad de su código fuente, lo que facilitó la modificación y análisis de componentes internos reales.

Cada uno de los hitos planteados se completó satisfactoriamente:

- Se **personalizó el mensaje de bienvenida:** modificando el archivo `/etc/motd` y replicando el cambio en el código fuente, logrando que el mensaje personalizado apareciera al iniciar sesión.

- Se **depuró el bug en `pthread_mutex_trylock`:** identificando que la función se llamaba recursivamente a sí misma. La corrección consistió en reemplazar la llamada recursiva por `mthread_mutex_trylock`, resolviendo así el bloqueo del programa de prueba. El programa validó el comportamiento esperado, devolviendo `0` en la primera llamada y `EDEADLK` (código 11) en la segunda.

- Se **implementó el comando `tree`:** desarrollando un comando que recorre recursivamente el sistema de archivos utilizando llamadas al sistema (`opendir`, `readdir`, `closedir`, `stat`), aplicando indentación según el nivel de profundidad y evitando seguir enlaces simbólicos para prevenir ciclos.

- Se **modificó el planificador:** incorporando un contador de quantums por proceso e implementando una política de penalización por uso intensivo de CPU, basada en ventanas de tiempo. La validación experimental mostró una clara diferencia de tratamiento entre procesos CPU-bound (penalizados hasta prioridad 15) y procesos interactivos (sin penalización), cumpliendo con los criterios establecidos.

Gracias a este proyecto aprendimos en la práctica cómo funcionan la planificación por prioridades, la concurrencia con hilos y el acceso de bajo nivel al sistema de archivos. En definitiva, la experiencia adquirida resulta de gran valor para nuestra formación en la carrera, al integrar conceptos teóricos con la modificación práctica de un sistema operativo real.

# 5. Referencias consultadas

Guerra, C. (2026). _Setup inicial de MINIX_ [Video]. Grupo de Sistemas Operativos 25-26, Universidad de La Habana.

MINIX 3. (2016). _Installing MINIX 3_. MINIX 3 Wiki. Recuperado de https://wiki.minix3.org/doku.php?id=usersguide:doinginstallation

MINIX 3. (2018). _Download_. MINIX 3 Wiki. Recuperado de https://wiki.minix3.org/doku.php?id=www:download:start

# 6. Declaración de uso de IA

Fue utilizada como:

Ayuda inicial para interpretar ciertos errores durante el proceso de instalación, como problemas de conectividad o de espacio para clonar y ejecutar el make.

Asistencia para lograr conectar las terminales de minix y Ubuntu mediante SSH.

Ayuda para conocer el uso exacto de varios comandos necesarios para la edición o supervisión de procesos en la terminal (cp, grep, top, etc, modo de compilar con cc, etc).

Para mayor comprensión de la relación entre `pthread` (capa de compatibilidad) y `mthread` (implementación nativa de MINIX).

Asistencia para resolver conflictos al hacer `git pull` y `git push`, y para manejar el token de autenticación.

En desconocimiento de qué funciones podían utilizarse para tareas como comprobar si algo se trataba de un enlace simbólico, y entender que hacían, recibían y devolvían las funciones que trabajaban con los directorios, y la biblioteca que incluir.

Para comprobar errores o la falta de ellos de sintaxis en c (ejemplo: falto un ;).

Asistencia con la interfaz de virtualbox (ejemplo: como crear un snapshot de tu estado actual para salvagardarlo).

Para entender las funciones originales de do_no_quantum y balance_queues y cuál era el archivo donde definía los campos de variable para rmp para poder definir quantum_count.

Ayuda con la redacción de las referencias para este informe, siguiendo el formato APA, así como en la revisión de estilo y corrección de errores tipográficos en el resto del informe.
