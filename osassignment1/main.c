//
//  main.c
//  osassignment1
//
//  Created by Richard Ng on 12/10/2015.
//  Copyright (c) 2015 Richard Ng. All rights reserved.
//

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>



#define buffsize 128
int fildes[2]; //connect parent to process 1
int fildes2[2]; //connect process 1 to process 2
int fildes3[2]; //connect process 3 to parent
char buffer[buffsize];
char tempBuffer[130];
char fileBuffer[buffsize];
char printToFileBuffer[buffsize];
int linesInFile = 0;




void error(void);
void process1(char* path);
void parent(char* path);
void process2(char* path);
void process3(char* path);
void timestamp();

void error(void)
{
    printf("unable to create new child");
    
}

void process1(char* path)
{
    time_t t;
    time(&t);
    int pid;
    char timeStr[100];  //TIMESTAMP
    strftime(timeStr,100,"%X %x // ", localtime(&t));
    
    //create process2 from process1
    pipe(fildes2);
    pid = fork();
    
    if (pid == 0)
        process2(path);
    else
    {
        close(fildes[1]);
        FILE *process1Log = fopen("/Users/Richard/Documents/Xcode/osassignment1/Process1Log.txt","w");
        
        //for(i=0;i<linesInFile;i++)
        while(read(fildes[0],&buffer,128))
        {
            memset(tempBuffer,0,buffsize);
            if(buffer[0] == '1')
            {
                fprintf(process1Log,"%s // STORED  // %s",timeStr,buffer);
                fflush(process1Log);
            }
            
            else
            {
                fprintf(process1Log,"%s // FORWARD // %s",timeStr,buffer);
                fflush(process1Log);
                
                close(fildes2[0]);
                write(fildes2[1],buffer,buffsize);
            }
        }
        printf("\n\nclosefile\n\n");
        fflush(stdout);
        fclose(process1Log);
    }
}

void process2(char* path)
{
    int pidi,j,m;
    int fd;
    time_t t;
    time(&t);
    char timeStr[100];  //TIMESTAMP
    strftime(timeStr,100,"%X %x // ", localtime(&t));
    
    close(fildes2[1]);
    
    FILE *process2Log = fopen("/Users/Richard/Documents/Xcode/osassignment1/Process2Log.txt","w");
    
    
    //for(i=0;i<linesInFile;i++)
    while(read(fildes2[0],&buffer,128))
    {
        //pm = read(fildes2[0],&buffer,128);
        memset(tempBuffer,0,buffsize); //clear char array
        if(buffer[0] == '2')
        {
            fprintf(process2Log,"%s // STORED  // %s",timeStr,buffer);
            fflush(process2Log);
        }
        
        else
        {
            fprintf(process2Log,"%s // FORWARD // %s",timeStr,buffer);
            fflush(process2Log);
            
            fd = open(path, O_WRONLY);
            write(fd, buffer, 128);
        }
    }
    printf("\n\nclosefile\n\n");
    fflush(stdout);
    fclose(process2Log);
}

void process3(char *path)
{
    time_t t;
    time(&t);
    char timeStr[100];  //TIMESTAMP
    strftime(timeStr,100,"%X %x // ", localtime(&t));
    
    int i;
    int fd;
    
    FILE *process3Log = fopen("/Users/Richard/Documents/Xcode/osassignment1/Process3Log.txt","w");
    fd = open(path, O_RDONLY);
    close(fildes3[0]);
    
    while(read(fd, &buffer, buffsize))
    {
        //read(fd, &buffer, buffsize);
        if(buffer[0] == '3')
        {
            fprintf(process3Log,"%s // STORED  // %s",timeStr,buffer);
            fflush(process3Log);
        }
        
        else
        {
            fprintf(process3Log,"%s // FORWARD // %s",timeStr,buffer);
            fflush(process3Log);
            
            write(fildes3[1], buffer, 128);
        }
    }
    printf("\n\nclose file\n");
    fflush(stdout);
    fclose(process3Log);
    close(fd);
}



void parent(char* path)
{
    //initial parent process
    
    time_t t;
    time(&t);
    char timeStr[100];  //TIMESTAMP
    strftime(timeStr,100,"%X %x // ", localtime(&t));
    
    int pid, pm;
    close(fildes[0]);
    
    FILE*fileReader = fopen("/Users/Richard/Documents/Xcode/osassignment1/sampletext.txt","r");
    FILE* parentLog = fopen("/Users/Richard/Documents/Xcode/osassignment1/parentLog.txt","w");

    while(fgets(fileBuffer,130, fileReader))
      {
          pm = write(fildes[1],fileBuffer,128);
      }
    

    pipe(fildes3);
    pid = fork();
    
    if (pid==0)
        process3(path);
    else
        ;
    close(fildes3[1]);
    while(read(fildes3[0],buffer,128))
    {
            fprintf(parentLog,"%s // STORED  // %s",timeStr,buffer);
            fflush(parentLog);
    }
    printf("\n\nclose file\n");
    fflush(stdout);
    fclose(parentLog);
    close(fildes3);

}


int main(void) {
    int pipes;
    pid_t pid;
    
    int named;
    char *path = "/tmp/fifo";
    unlink(path);
    named = mkfifo(path, 0666);
    printf("named pipe creation: %d",named);
    fflush(stdout);
    
    pipe(fildes);
    pid = fork();
    
    if (pid == 0)

        process1(path);
    else
        parent(path);
}

