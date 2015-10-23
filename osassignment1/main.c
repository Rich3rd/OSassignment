//
//  main.c
//  osassignment1
//
//  Created by Richard Ng on 12/10/2015.
//  Copyright (c) 2015 Richard Ng. All rights reserved.
//
// OS ASSIGNMENT 1
// NG TER JING
// 0313766
// 23 OCTOBER 2015
// TIME: 15.47

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
char fileBuffer[buffsize];


void process1(char* path);
void parent(char* path,char* argv[]);
void process2(char* path);
void process3(char* path);


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
        FILE *process1Log = fopen("Process1Log.txt","w");
        
        fprintf(process1Log,"Pid is: %d \nPPid is: %d\n\n",getpid(),getppid());
        fflush(process1Log);
        
        printf("Process1 reading.......\n");
        fflush(stdout);
        while(read(fildes[0],buffer,buffsize))
        {
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
        printf("Process1 complete.\n");
        fflush(stdout);
        fclose(process1Log);
        exit(0);
    }
}

void process2(char* path)
{
    int fd;
    
    time_t t;
    time(&t);
    char timeStr[100];  //TIMESTAMP
    strftime(timeStr,100,"%X %x // ", localtime(&t));
    
    close(fildes2[1]);
    
    FILE *process2Log = fopen("Process2Log.txt","w");
    
    fprintf(process2Log,"Pid is: %d \nPPid is: %d\n\n",getpid(),getppid());
    fflush(process2Log);
    
    printf("Process2 reading......\n");
    fflush(stdout);
    
    while(read(fildes2[0],buffer,buffsize))
    {
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
            write(fd, &buffer, buffsize);
        }
    }
    printf("Process2 complete.\n");
    fflush(stdout);
    fclose(process2Log);
    exit(0);
}

void process3(char *path)
{
    time_t t;
    time(&t);
    char timeStr[100];  //TIMESTAMP
    strftime(timeStr,100,"%X %x // ", localtime(&t));
    
    int fd;
    
    FILE *process3Log = fopen("Process3Log.txt","w");
    fd = open(path, O_RDONLY);
    close(fildes3[0]);
    
    fprintf(process3Log,"Pid is: %d \nPPid is: %d\n\n",getpid(),getppid());
    fflush(process3Log);
    
    printf("Process3 reading.......\n");
    fflush(stdout);
    
    while(read(fd, buffer, buffsize))
    {
        if(buffer[0] == '3')
        {
            fprintf(process3Log,"%s // STORED  // %s",timeStr,buffer);
            fflush(process3Log);
        }
        
        else
        {
            fprintf(process3Log,"%s // FORWARD // %s",timeStr,buffer);
            fflush(process3Log);
            
            write(fildes3[1], &buffer, buffsize);
        }
    }
    printf("Process3 complete.\n");
    fflush(stdout);
    fclose(process3Log);
    close(fd);
    exit(0);
}



void parent(char* path,char** argv)
{
    time_t t;
    time(&t);
    char timeStr[100];  //TIMESTAMP
    strftime(timeStr,100,"%X %x // ", localtime(&t));
    
    int pid;
    
    close(fildes[0]);
    
    //FILE* fileReader = fopen("sampletext.txt","r");
    FILE* fileReader  = fopen(argv[1],"r");
    FILE* parentLog = fopen("ParentLog.txt","w");
    printf("Parent reading.......\n");
    fflush(stdout);
    while(fgets(fileBuffer,buffsize,fileReader))
    {
        write(fildes[1],fileBuffer,buffsize);
    }
    printf("Parent read complete.\n");
    fflush(stdout);
    
    pipe(fildes3);
    pid = fork();
    
    if (pid==0)
        process3(path);
    
    fprintf(parentLog,"Pid is: %d \nPPid is: %d\n\n",getpid(),getppid());
    fflush(parentLog);
    
    printf("Parent reading again......\n");
    fflush(stdout);
    
    close(fildes3[1]);
    while(read(fildes3[0],buffer,buffsize))
    {
        fprintf(parentLog,"%s // STORED  // %s",timeStr,buffer);
        fflush(parentLog);
    }
    printf("Parent return complete.\n");
    fflush(stdout);
    fclose(parentLog);
}


int main(int argc, char** argv)
{
    pid_t pid;
    
    int named;
    char *path = "fifo";
    unlink(path);
    named = mkfifo(path, 0666);
    
    printf("Note, program won't terminate automatically, please terminate manually.\n");
    fflush(stdout);
    
    pipe(fildes);
    pid = fork();
    
    if (pid == 0)

        process1(path);
    else
        parent(path,argv);
    
    printf("All process complete.\n");
    fflush(stdout);
}

