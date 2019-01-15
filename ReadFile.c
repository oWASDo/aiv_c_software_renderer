#include "aiv_rasterizer.h"
#include <stdio.h>


char * readFile(const char * filename, size_t *file_size){
    FILE *fhandle = fopen(filename, "rb");
    if(!fhandle){
        return NULL;
    }   
    fseek(fhandle, 0, SEEK_END);
    *file_size = ftell(fhandle);
    fseek(fhandle, 0, SEEK_SET);

    char *data = malloc(*file_size);
    if(!data){
        fcloseall(fhandle);

        return NULL;
    }
    fread(data, 1, *file_size, fhandle);
    fclose(fhandle);

    return data;
}

int ReadObj(const char fileName, Triangle_t* arrayOfTringle, size_t sizeOfringleArray){

    return 0;
}