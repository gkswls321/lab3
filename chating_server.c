#include <stdio.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <signal.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define MAXSTRING 256
#define MSGKEY 1234
#define SHMKEY 4321
#define FALSE 0
#define TRUE 1

typedef struct{
        long mtype;
        char mtext[MAXSTRING];
} mymsg_t;

int main(){
    void *shared_memory = (void *)0;
    char *message;
	int shmid;
	mymsg_t mymsg;
	key_t msg_key, shm_key;
	char flag = 0;

	if((shmid = shmget((key_t)SHMKEY, sizeof(char)*MAXSTRING, IPC_CREAT|0666)) == -1)	{
		perror("shmget failed");
		exit(0);
	}
	
	if((shared_memory = shmat(shmid, (void *)0, 0)) == (void *)-1){
		perror("shmat failed");
		exit(0);
	}

	message = (char *)shared_memory;

	if((msg_key = msgget(MSGKEY, IPC_CREAT|0666)) < 0){
		perror("msgget error:");
		exit(0);
	}

	while(1){
		if(msgrcv(msg_key, (void *)&mymsg, sizeof(mymsg_t), 1, 0) == -1)
	{
	perror("msgrcv error:");
	exit(0);
	}
	
	printf("message sending...\n");
	flag++;

	if(flag > 50)
		flag = 0;
	
	memcpy(message, &flag, 1);
	memcpy(message+1, mymsg.mtext, 1);
	memcpy(message+4 , mymsg.mtext+1, MAXSTRING-5);
	
	}
	return 0;
}
