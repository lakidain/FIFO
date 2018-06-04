#include <stdio.h>  //librerias que vamos a utilizar
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define FIFO_NAME0 "lecturaControlador"     //Todas los robots comparten esta FIFO para enviar informacion a la controladora
#define FIFO_NAME1 "escrituraAtornillado"   //Cada robot tendra una fifo a la que escribiremos independientemente
#define FIFO_NAME2 "escrituraPintura"
#define FIFO_NAME3 "escrituraRemachado"
#define FIFO_NAME4 "escrituraSoldadura"
#define FIFO_NAME5 "escrituraLimpieza"

#define PETICIONES 10 //Peticiones maximas que va a tener la cola

void atenderPeticiones();   //definiciones de los metodos del programa
void escribirEnFifo(int ,int ,int ,int, char[],int *);

//PRINCIPIO METODOS COLA
typedef struct Node //Estructura nodo
{
    char data[3];   //se compone de un dato que seran los 3 bytes que componen una palabra, las de los robot
    struct Node* next;  //una referencia apuntando al siguiente nodo
} Node; 

typedef struct Queue    //estructura de cola
{
    int size;   //entero con el tamaño
    int limit;  //el limite de la cola
    struct Node* first; //puntero al primer Nodo guardado
    struct Node* last;  //puntero al ultimo Nodo guardado
} Queue;


Queue *ConstructQueue(int limit);   //definiciones de los metodos de la cola
int Enqueue(Queue *pQueue, Node *item);
Node *Dequeue(Queue *pQueue);
int isEmpty(Queue* pQueue);
Node *front(Queue *pQueue);

Queue * ConstructQueue(int limit)   //metodo constructor
{
    Queue *queue = (Queue*) malloc (sizeof(Queue)); //asignar el espacio en memoria de la cola
    if(limit < 0)   //si el limite que recibimos es negativo por lo que sea lo ponemos en positivo
    {
        limit = -limit;
    }
    queue->limit = limit;   //inicializamos los valores de la cola 
    queue->size = 0;   
    queue->first = NULL;    
    queue->last = NULL; 

    return queue;   //devolvemos la cola
}

int Enqueue(Queue *pQueue, Node *item)  //metodo para encolar
{

    if ((pQueue == NULL) || (item == NULL)) //si no son validos los parametros no los introducimos
    {
        return 0;
    }

    if (pQueue->size >= pQueue->limit)  //si encolas un elemento que no cabe lo pierdes, que no este llena la cola
    {
        return 0;
    }

    item->next = NULL;  //como has introducido un elemento el siguiente Nodo esta vacio

    if (pQueue->size == 0)  //si la cola esta vacia el primero y el ultimo de la cola es ese elemento, no tiene siguiente
    {
        pQueue->first = item;
        pQueue->last = item;

    }
    else    //caso tipico el anterior hacemos que apunte al nuevo
    {
        pQueue->last->next = item;
        pQueue->last = item;
    }   

    pQueue->size++; //aumentamos en 1 el tamaño de la cola
    return 1;   //devolvemos exito
}

Node * Dequeue(Queue *pQueue)   //metodo para desencolar
{
    Node *item; //declaras un nodo auxiliar
    if (isEmpty(pQueue))    //si la cola esta vacia no devolvemos nada
        return NULL;
    item = pQueue->first;   //aqui consigues la referencia del primero
    pQueue->first = (pQueue->first)->next;  //mueves el siguiente a donde estaba el anterior, al usar nodos no tienes que moverlos porque usan referencias ;) estructuras de datos
    pQueue->size--; //restamos 1 a la cola
    return item;    //devuelve el nodo
}

Node * front(Queue *pQueue) //metodo para mostrar el primero
{
    if(isEmpty(pQueue)) //si esta vacia no muestra nada
        return NULL;
    return pQueue->first;   //si no, devuelve el primer nodo sin desencolarlo, para mostrarlo
}

int isEmpty(Queue* pQueue)  //metodo para comprobar si esta vacia la cola
{
    if (pQueue == NULL) //si es nula devuelve un 0
    {
        return 0;
    }
    if (pQueue->size == 0)  //si tiene mas de un item devuelve un 1 porque no esta vacia
    {
        return 1;
    }
    else    //si no tienes mas de un item es porque estara vacia
    {
        return 0;
    }
}

//FIN METODOS COLA

int main(int argc, char* argv[])    //Inicializacion de los procesos robots
{
    pid_t pidSoldadura; //iniciamos el pid_t de este robot concreto
    fflush(stdout); //para evitar errores de escritura con printf hacemos fflus, para limpiar el buffer
    pidSoldadura = fork();  //creamos el proceso hijo

    if (pidSoldadura == -1) {   //si se ha producido un error al crear el hijo finalizamos
        printf("Error, child has not been created\n"); 
        exit(-1); 
    } 
    if (pidSoldadura == 0){     //para el hijo lanzamos el programa en .c que se encarga de ejecutar el robot
        printf("Arrancando la plataforma...\n");
        execlp("./soldadura","soldadura",NULL);   
        exit(0);
    } 

    else { //vamos repitiendo el mismo proceso de arriba en el padre para cada robot, al hacerlo en el padre no hay problema, la representación seria un nodo padre con muchos hijos
        pid_t pidPintura; //aqui se guarda el pid de este robot concreto
        fflush(stdout); //el fflush es para limpiar el buffer de escritura del padre antes del fork, si lo dejas hay problemas con el printf
        pidPintura = fork();    //hacemos el fork() para separar los procesos padre e hijo
 
        if (pidPintura == -1) { //si hay error creando el hijo avisamos 
            printf("Error, child has not been created\n"); 
            exit(-1); 
        } 
        if (pidPintura == 0){   //si es el hijo abre con execlp el robot, es importante hacerlo en un hijo porque esto copia la imagen de ese proceso en el actual, si no dividieramos con fork() nos cargariamos el padre   
            execlp("./pintura","pintura",NULL); //el execlp es como el execl pero sin la ruta, directamente la coge del propio directorio  
            exit(0);
        } 

        else { //el padre sigue creando hijos
            pid_t pidAtornillado;
            fflush(stdout);
            pidAtornillado = fork();
 
            if (pidAtornillado == -1) { 
                printf("Error, child has not been created\n"); 
                exit(-1); 
            } 
            if (pidAtornillado == 0){  
                execlp("./atornillado","atornillado",NULL);   
                exit(0); 
            } 

            else { 
                pid_t pidRemachado;
                fflush(stdout);
                pidRemachado = fork();
 
                if (pidRemachado== -1) { 
                    printf("Error, child has not been created\n"); 
                    exit(-1); 
                } 
                if (pidRemachado == 0){  
                    execlp("./remachado","remachado",NULL);   
                    exit(0); 
                } 

                else { //una vez ha creados todos los hijos (robots) empieza a atender peticiones y cuando acaba avisa de que ha finalizado
                    pid_t pidLimpieza;
                	fflush(stdout);
                    pidLimpieza = fork();
 
                	if (pidLimpieza== -1) { 
                    		printf("Error, child has not been created\n"); 
                    		exit(-1); 
               		 } 
                	if (pidLimpieza == 0){  
                    		execlp("./limpieza","limpieza",NULL);   
                    		exit(0); 
                } 
			else{
				atenderPeticiones();    //una vez hemos activado todos los robots lanzamos la peticion para atender a todos los robots
                    	printf("Se han acabado todas las operaciones de todos los robot. FIN DE PROGRAMA\n");
                	} 
		}
            } 
        } 
    } 
}

void atenderPeticiones(){
    char string[3]; //array auxiliar donde guardaremos el string que nos llega en cada lectura, las lecturas las hacemos de 3 bytes en 3 bytes por eso le ponemos este tamaño
    int fq,fw,fe,fr,ft,fy;

    int ultimoAtendido=0,ultimaInsercion=0,ocupado=0,contador=0,limpieza=0;   //ocupado es una variable que nos indica si actualmente se esta atendiendo una peticion 0 indica que no 1 que si. 

    Queue *solicitud = ConstructQueue(PETICIONES);  //declaración de la cola, que atendera las peticiones que tenemos definidas como variable global
    

    mknod(FIFO_NAME0, S_IFIFO | 0666, 0);   //creamos las fifo por si no estuvieran creadas con los permisos necesarios, si ya estan creadas no hará nada
    mknod(FIFO_NAME1, S_IFIFO | 0666, 0);
    mknod(FIFO_NAME2, S_IFIFO | 0666, 0);
    mknod(FIFO_NAME3, S_IFIFO | 0666, 0);
    mknod(FIFO_NAME4, S_IFIFO | 0666, 0);

    fq = open(FIFO_NAME0, O_RDONLY);    //abrimos las fifo
    fw = open(FIFO_NAME1, O_WRONLY); 
    fe = open(FIFO_NAME2, O_WRONLY);
    fr = open(FIFO_NAME3, O_WRONLY);
    ft = open(FIFO_NAME4, O_WRONLY); 
	fy = open(FIFO_NAME5, O_WRONLY);
 
    printf("CTL: esperando solicitud\n");  //Indicamos que estamos esperando la solicitud
    while(contador<4){  //Tenemos una señal especial que el robot envia cuando el robot ha finalizado de realizar todas las ejecuciones necesarias, cuando nos llega esa señal aumentamos en 1 el contador y cuando nos lleguen todas paramos de leer porque sabemos que no nos van a llegar mas mensajes
        if ((read(fq, string, 3)) == -1)    //leemos el dato y lo metemos en buffer que tendra 3 de tamaño
	        perror("read");
	    else {
		if(strcmp(string,"LIM")==0){    //miramos si la solicitud es de ATO para darle el OK
        		if ((write(fy, string, strlen(string))) == -1)  //iniciamos la operacion de escritura, esto equivale a pedir permiso a la controladora para que atienda la peticion
            			perror("write");
    		
                else{
                    printf("CTL:Recibida peticion de acceso LIM, procedemos a la limpieza de la maquina\n");
                }
        }
		else if(strcmp(string,"FIL")==0){
            printf("CTL:Recibido FIN de limpieza, maquina limpia, puede ejecutar el PIN\n");
			limpieza=1;
		}
		else if(strcmp(string,"FIN")==0){    //comprobamos si nos ha llegado un FIN como cadena desde uno de los robot, si es que si
                	ocupado=0;  //decimos que ya no esta ocupado
                	printf("CTL: recibido FIN de %s... ",Dequeue(solicitud)->data);     //Como la primera parte de la frase cuando recibimos un FIN es generica la ponemos y despues añadimos lo que haga falta
                if(isEmpty(solicitud)){ //si esta vacia la cola no hacemos nada y esperamos a que alguien mande una solicitud
                    printf("nadie en cola\n");
                } 
                else{   //si hay elementos en la cola
                    printf("desencolando %s\n",front(solicitud)->data); //desbloqueamos la siguiente peticion que este en cola, no la desbloqueamos hasta que termine para poder hacerle el print correspondiente, aqui solo le hacemos el front y arriba es donde la desencolamos, cuando nos llega su FIN 
                    strcpy(string,front(solicitud)->data);  //copiamos la peticion en cola al string
                    escribirEnFifo(fw,fe,fr,ft,string,&limpieza); //mandamos a la FIFO que corresponda un OK de que puede empezar la actividad, da igual lo que mandamos, estamos aprovechando los semaforos internos que tienen las FIFO
                    ocupado=1;  //como estamos atendiendo una peticion nueva ponemos a 1, estamos ocupados de nuevo
                } 
            }
            else if(strcmp(string,"END")==0){   //cuando los robot finalicen el numero de operaicones asignadas mandan un END que nos sirven para que el programa principal no se quede esperando 
                    contador++;  
            }
            else{     //Aqui llegamos si no se ha recibido un FIN, ni un END         
                Node *n = (Node*) malloc(sizeof (Node));    //Creación de el nodo auxiliar que va en la cola
                strcpy(n->data,string); //Copia el string en el Nodo que encolaremos
                Enqueue(solicitud,n);   //Encolamos la peticion

	            if(ocupado==0){ //si no esta ocupado entonces podemos atender la peticion
                    printf("CTRL: recibida petición de acceso (%s)... permitida\n",string);   //imprimimos que vamos a atender la peticion 
                    ocupado=1;  //ponemos ocupado a 1
                    escribirEnFifo(fw,fe,fr,ft,string,&limpieza); //mandamos a la FIFO que corresponda un OK de que puede empezar la actividad, da igual lo que mandamos, estamos aprovechando los semaforos internos que tienen las FIFO                  
                } 
                else{
                    printf("CTRL: recibida petición de acceso (%s)... encolada\n",string); //si estamos ocupados con una actividad entonces encolamos la que nos llega                             
                }  
            }        
	    }
    }
    sleep(1);   //todos los sleep del programa son para que quede mejor visualmente en los printf
}

void escribirEnFifo(int fw,int fe,int fr,int ft,  char string[],int *limpieza){    //metodo de escritura, cada uno de los robots tiene una FIFO de escritura personalizada para saber a quien escribirle
    if(strcmp(string,"ATO")==0){    //miramos si la solicitud es de ATO para darle el OK
        if ((write(fw, string, strlen(string))) == -1)  //iniciamos la operacion de escritura, esto equivale a pedir permiso a la controladora para que atienda la peticion
            perror("write");              
    }
    if(strcmp(string,"PIN")==0){    //miramos si la solicitud es de ATO para darle el OK
        if ((write(fe, string, strlen(string))) == -1 && *limpieza==1){  //iniciamos la operacion de escritura, esto equivale a pedir permiso a la controladora para que atienda la peticion
            perror("write");
        }
		limpieza=0;
    }
    if(strcmp(string,"REM")==0){    //miramos si la solicitud es de ATO para darle el OK
        if ((write(fr, string, strlen(string))) == -1)  //iniciamos la operacion de escritura, esto equivale a pedir permiso a la controladora para que atienda la peticion
            perror("write");
    }
    if(strcmp(string,"SOL")==0){    //miramos si la solicitud es de ATO para darle el OK
        if ((write(ft, string, strlen(string))) == -1)  //iniciamos la operacion de escritura, esto equivale a pedir permiso a la controladora para que atienda la peticion
            perror("write");
    }
}

