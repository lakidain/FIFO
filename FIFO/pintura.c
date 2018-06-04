#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define FIFO_NAME0 "lecturaControlador"
#define FIFO_NAME1 "escrituraPintura"

#define robot "PIN"
#define NUMOPERACIONES 5

int main(int argc, char* argv[])
{
    printf("%s... OK\n",robot);
    char s[2];
    int num, fq,fw;
    
    mknod(FIFO_NAME0, S_IFIFO | 0666, 0);
    mknod(FIFO_NAME1, S_IFIFO | 0666, 0);

    fq = open(FIFO_NAME0, O_WRONLY);
    fw = open(FIFO_NAME1, O_RDONLY);

    printf("%s: inicio jornada (PID %d)\n",robot,getpid());

    srand(48459389);
    for(int i=0;i<NUMOPERACIONES;i++){  //hacemos un for con el numero de operaciones que vamos a realizar
        sleep(1);        
        int random = rand()%4+2;    //aqui sacamos el numero de segundos que vamos a esperar para realizar la peticion, rand()%4 genera un numero entre 0 y 4 y al sumarle 2 lo hace entre 2 y 6
        printf("%s: pausa %d segundos\n",robot,random);    //indicamos la pausa que va a realizar el robot antes de realizar la peticion
        fflush(stdout); //esta limpieza la hacemos para evitar problemas, hay veces que la transicion entre printf y sleep se hace tan rapido que el printf no aparece si no está esta orden
        sleep(random);  //realizamos el sleep

        if ((num = write(fq, robot, strlen(robot))) == -1)  //iniciamos la operacion de escritura, esto equivale a pedir permiso a la controladora para que atienda la peticion
	        perror("write");
	    else{   
            if ((num = read(fw, s, 3)) == -1)   //si la peticion se ha realizado correctamente esperamos la confirmacion por parte de la controladora
	            perror("read");
	        else {  //una vez recibimos la confirmacion podemos empezar a trabajar
                random = rand()%5+1;    //generamos un random para decidir cuanto tiempo va a tardar en realizar el trabajo
                printf("%s: inicio trabajo (%d segundos) \n", robot,random);   //hacemos la salida por pantalla de que se ha iniciado el trabajo
                fflush(stdout); 
                sleep(random); //el sleep simula el trabajo
                printf("%s: fin trabajo. Notifico a CTL\n",robot);  //indicamos que ha finalizado el trabajo
                if ((num = write(fq, "FIN", 3)) == -1){ //enviamos a la controladora el aviso de que hemos finalizado de trabajar
	                perror("write");              
                }
            }
        }
    }
    if(write(fq,"END",3)==-1){  //Escribimos un END para indicar que hemos finalizado toda la jornada
        perror("write");
    }
    else{
        printf("%s: fin jornada (PID %d)\n",robot,getpid());    //imprimimos el fin de jornada    
    }
}
