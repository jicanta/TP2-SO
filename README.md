# BurinShell

BurinShell es un proyecto que provee una consola de comandos que demuestra el funcionamiento del Kernel.

## Prerrequisitos

Para poder hacer uso de nuestra consola de comandos, deberá tener instalado el emulador Qemu x86 (vea más adelante los pasos necesarios para su instalación en la sección Instalación de Qemu x86)
De no contar con el archivo qcow2 , deberá tener instalado Docker (vea más adelante los pasos necesarios para su instalación en la sección Instalación de Docker). El archivo qcow2 es la imagen de disco duro virtual usada por QEMU, y que será de donde se levante el SO a memoria cuando QEMU se inicie. Dicho archivo es generado dentro de la carpeta Image
Por último, algunos dispositivos o computadoras requieren tener instalado y ejecutándose en segundo plano XLaunch para la visualización de la consola de comando.

## Instalación de dependencias

### Instalación de qemu x86

Ejecute el comando  ```sudo apt install qemu‐system‐x86 qemu‐utils```

### Instalación de Docker

- **Instalación del programa**
Ejecute el comando ```sudo apt install docker.io```

- **Descarga de la imagen** 
Ejecute el comando ```docker pull agodio/itba‐so:1.0```

### Instalación de XLaunch
Descarguelo desde el siguiente link:  https://sourceforge.net/projects/vcxsrv/

## Iniciar la shell

En caso de no contar con los permisos necesarios para algun archivo ejecutable, debe correr (posicionado dentro de la carpeta del proyecto TPE-Arqui) el comando ```chmod 777 *```

### Sin el archivo qcow2 generado
Teniendo Docker correctamente instalado, debe ejecutar el archivo compile.sh. Para esto, párese dentro de la carpeta de la shell (llamada TPE-Arqui) e ingrese el comando ```./compile.sh```.

### Con el archivo qcow2 ya generado/descargado
Una vez que posea el archivo qcow2, debe ejecutar el archivo run.sh. Para esto párese dentro de la carpeta de la Shell (TPE-Arqui) e ingrese el comando ```./run.sh``` . Luego de su ejecución, se abrirá el emulador qemu y verá una pantalla brindándole la bienvenida y proporcionándole ayuda inicial.

## Guía de comandos

### help
Despliega un mensaje informando los comandos válidos y una breve descripción de la funcionalidad de cada uno de estos.

### divbyzero
Ejecuta un programa que hace trigger de la excepción asociada a la división por cero. Esto permite visualizar cómo el Kernel maneja dicha excepción. Al final, se le permite recuperarse y seguir con la shell de forma normal.

### invalidopcode
Ejecuta un programa que realiza un trigger de la excepción asociada al código de operación inválido. Al igual que la excepción anterior, muestra cómo se maneja dicha excepción y permite recuperarse.

### zoomin
De ser posible, aumenta el tamaño del texto en pantalla. Si no fue posible hacer un zoom, se imprimirá un mensaje en pantalla informándolo.

### zoomout
Realiza, si es posible, una reducción del texto en pantalla. Si no fue posible hacer un zoom se imprimirá un mensaje en pantalla informándolo.

### time
Imprime un mensaje en pantalla con la hora actual en formato HH:MM:SS

### date
Imprime un mensaje en pantalla con la fecha actual en formato DD:MM:YY, transformado al horario local

### clear
Borra el contenido de toda la pantalla

### registers
Imprime los registros del procesador desde la última vez que fueron guardados. En caso de no haberse realizado ningún guardado previo, la consola pedirá que lo haga antes de continuar. 

## Atajos
### Guardado del valor de los registros
Al presionar la tecla ALT, se guardarán los valores actuales (al momento de la presión) de los registros. Dichos valores podrán posteriormente ser consultados con el comando registers descrito anteriormente

### Aumento y disminución del tamaño de la fuente
Presionando las teclas CTRL + I, podrá realizar un aumento rápido de la fuente de la consola, del mismo modo que podrá reducirla al presionar CTRL + O. Se puede realizar en medio de la escritura o fuera de la misma.

### Almacenamiento de comandos previos
La shell se encarga de almacenar los comandos que el usuario envía. Presionando la flecha hacia arriba de su teclado podrá acceder a los comandos anteriores, mientras que con la flecha hacia abajo podrá ver los comandos más recientes. 

### Escritura en la Shell y recepción de comandos
BurinShell maneja varios caracteres imprimibles recibidos por teclado. Para acceder a más caracteres, solo requiere de combinar la tecla SHIFT con alguna otra tecla. Por ejemplo, SHIFT + Letra dará como resultado la letra en mayúscula, SHIFT + 1 dará como resultado un signo de admiración, y así con otras combinaciones.
	La Shell está preparada para recibir comandos sin considerar si las letras son mayúsculas o minúsculas. Esto quiere decir que tanto ‘HELP’ como ‘help’ serán interpretadas como el mismo comando, al igual que todas sus combinaciones.
