#include <stdio.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <signal.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#define MAXSTRING 256
#define MSGKEY 1234


#define SHMKEY 4321
#define TRUE 1
#define FALSE 0

typedef struct{
	long mtype;
	char mtext[MAXSTRING];
	} mymsg_t;
	
char mynumber;

void *print(void *msg);

int main(int argc, char *argv[]){
	void *shared_memory = (void *)0;
	char *message;
	int shmid;
	int error;
	char buf[MAXSTRING];
	mymsg_t mymsg;
	key_t msg_key;
	pthread_t tid;
	
	if(argc != 2){
		fprintf(stderr, "input client number please...\n");
		return 1;
	}

	mynumber = *argv[1];
	
	if((shmid = shmget((key_t)SHMKEY, sizeof(char)*MAXSTRING, 0)) == -1){
		perror("shmget failed");
		exit(0);
	}
	
	if((shared_memory = shmat(shmid, (void *)0, IPC_CREAT|0666)) == (void *)-1){
		perror("shamat failed");
		exit(0);
	}
	
	message = (char *)shared_memory;
	
	if((msg_key = msgget((key_t)MSGKEY, IPC_CREAT|0666)) == -1){
		perror("msgget error:");
		exit(0);
	}
	
	memset(mymsg.mtext, 0x00, MAXSTRING);

	mymsg.mtype = 1;

	if(error = pthread_create(&tid, NULL, (void*)print, message)){
		fprintf(stderr, "Failed to create thread: %s\n", strerror(error));
		exit(1);
	}
	
	while(1){
		fgets(buf, MAXSTRING, stdin);
		if(!strcmp(buf, "quit"))
			return 1;
		
		*mymsg.mtext = mynumber;
		memcpy(mymsg.mtext+1, buf, MAXSTRING-2);
		
		if(msgsnd(msg_key, (void *)&mymsg, sizeof(mymsg_t), IPC_NOWAIT) == -1){
			perror("msgsnd error:");
			exit(0);
		}
	}
	return 0;
}

void *print(void *msg){
	char *flag;
	char *message;
	char compare = -1;
	char ch;
	char buf[MAXSTRING];
	int temp;
	char number;

	memset(buf, 0x00, MAXSTRING);
	while(1){
		flag = msg;
		ch = *(flag);
		number = *(flag+1);
		temp = strcmp(buf, msg);
		
		if((ch != compare) && temp && (number != mynumber)){
			message = msg + 4;
			printf("%s", message);
			memcpy(buf, msg, MAXSTRING-5);
		}
	}
}
	
	
