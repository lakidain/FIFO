#Script para actualizar todos los elementos de la plataforma de produccion
gcc controladora.c -o controladora -lpthread;
gcc atornillado.c -o atornillado -lpthread;
gcc pintura.c -o pintura -lpthread;
gcc remachado.c -o remachado -lpthread;
gcc soldadura.c -o soldadura -lpthread;
gcc limpieza.c -o limpieza -lpthread;
