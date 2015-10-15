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

#define buffsize 128
int fildes[2]; //connect parent to process 1
int fildes2[2]; //connect process 1 to process 2
int fildes3[2]; //connect process 3 to parent
char buffer[buffsize];
//int processNum = 0;

void error(void);
void process1(void);
void parent(void);
void process2(void);
void process3(char *path);

void error(void)
{
    printf("unable to create new child");
    
}

void process1(void)
{
    int pm,pipe2,pid ;
    printf("\n\nProcess1\n");
    close(fildes[1]);
    printf("Process1 created with pid = %d\n", getpid());
    printf("Process1's parent pid is %d\n",getppid());
    
    pm = read(fildes[0],buffer,128);
    printf("message received (%s)\n" , buffer);
    printf("Process1 received %d chars from pid:%d",pm,getppid());
    fflush(stdout);
    close(fildes[0]);
    
    //fflush(stdout);
    //sleep(1);
    
    pipe2 = pipe(fildes2);
    pid = fork();
    
    if (pid == 0)
        process2();
    else
    {
        printf("\n\n Process1 works\n");
        close(fildes2[0]);
        printf("Process1 created with pid = %d\n", getpid());
        printf("Process1's parent pid is %d\n",getppid());
        pm = write(fildes2[1],"test message from process 1",128);
        printf("Process1 sent %d chars", pm);
        fflush(stdout);
        //printf("message received %s\n" , buffer);
        //printf("process%d received %d chars from child, (process number %d)",processNum,pm,processNum);
        wait(NULL);
        exit(0);
    }
    
}

void process2(void)
{
    int pid,pm;
    printf("\n\nProcess2 \n");
    close(fildes2[1]);
    printf("Process2 created with pid = %d\n", getpid());
    printf("Process2's parent pid is %d\n",getppid());
    
    pm = read(fildes2[0],&buffer,128);
    //printf("confirm message after read\n");
    printf("Process2 message received:  (%s)\n" , buffer);
    printf("Process2 received %d chars from process 1", pm);
    fflush(stdout);
    wait(NULL);
    
    
    
    //fflush(stdout);
    //sleep(1);
    
    //exit(0);
}

void process3(char *path){
    
    //sleep(3);
    printf("\n\n\nConfirmation from process 3");
    printf("\nProcess3 created with pid = %d\n", getpid());
    printf("Process3's parent pid is %d",getppid());
    
    int fd;
    int pm;
    fd = open(path, O_RDONLY);
    read(fd, buffer, 128);
    printf("\nProcess3 receive: %s\n", buffer);
    fflush(stdout);
    close(fd);
    
    //sleep(1);
    //send message back to parent
    //int pipe3;
    //pipe3 = pipe(fildes3);
    //printf("\n pipe3 creation confirmation: %d",pipe3);
    //fflush(stdout);
    
    close(fildes3[0]);
    pm = write(fildes3[1], "messsage sent from process 3", 128);
    printf(" \n(%d) of chars sent from process3 to parent", pm);
    fflush(stdout);
    //exit(0);
}


void parent(void)
{
    int pid, pm;
    printf("parent starts working\n");
    printf("parent with pid of %d\n",getpid());
    close(fildes[0]);
    pm = write(fildes[1],"test message",128);
    printf("number of char written by (parent) %d\n",pm);
    //wait(NULL);
    fflush(stdout);
    
    wait(NULL);
    
    //create process3
   // char *path = "/Users/Richard/Desktop";
    
    
    char *path = "myfifo";
    unlink(path);
    mkfifo(path, 0600);
    
    int pipe3;
    pipe3 = pipe(fildes3);
    pid = fork();
    
    if (pid==0)
        process3(path);
    else
    {
        int fd,fm;
        fd = open(path, O_WRONLY);
        fm = write(fd, "message sent from process2 to process3", 128);
        //fflush(stdout);
        close(fd);
        printf("\n%d char sent by named pipe from process 2 to process3",fm);
        fflush(stdout);
        //close(fd);
        //printf("Child send: %d\n", );
        //wait(NULL);
        
        //sleep(3);
        //read message sent by process 3 to parent
        //pipe3 = pipe(fildes3);
        close(fildes3[1]);
        pm = read(fildes3[0],buffer,128);
        printf("\n\nMessage received from process3 in parent: %s",buffer);
        printf("\n(%d) char sent from process 3",pm);
        fflush(stdout);
        exit(0);
    }
}


int main(void) {
    int pipes;
    pid_t pid;
    
    pipes = pipe(fildes);
    printf("pipe created with pp = %d \n", pipes);
    pid = fork();
    
    if (pid == 0)
        process1();
    else
        parent();
}
