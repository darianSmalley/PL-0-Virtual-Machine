//
//  util.c
//  0 VM
//
//  Created by Darian Smalley on 7/1/15.
//  Copyright (c) 2015 Darian Smalley. All rights reserved.
//

#include "util.h"

FILE* openFile(char* fileName) {
    FILE* file;
    
    if (!(file = fopen(fileName, "r" ))) {
        perror("input.txt");
        exit(1);
    }
    
    return file;
}

char* fillInputStream(char* fileName) {
    FILE *file = openFile(fileName);
    int c;
    int i = 0;
    fseek(file, 0, SEEK_END);
    long fsize = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    char *inputStream = malloc(fsize + 1);
    
    while ((c = fgetc(file)) != EOF) {
        inputStream[i] = c;
        i++;
    }
    
    fclose(file);
    
    inputStream[i] = '\0';
    
    return inputStream;
}