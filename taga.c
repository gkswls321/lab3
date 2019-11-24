#include <stdio.h>
#include <termios.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/time.h>
#define PASSWORDSIZE 12 
static inline long myclock()
{
	struct timeval tv;
	gettimeofday (&tv, NULL);
	return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}
int count = 0;

int main(void) {
	long t, dt; // 시간변수
	int fd;
	int nread, cnt = 0, errcnt = 0;
	char ch, text[] = "I'm want no practice but want A+";
	
	struct termios init_attr, new_attr;
	
	fd = open(ttyname(fileno(stdin)), O_RDWR);
	tcgetattr(fd, &init_attr);
	new_attr = init_attr;
	new_attr.c_lflag &= ~ICANON;
	new_attr.c_lflag &= ~ECHO; /* ~(ECHO | ECHOE | ECHOK | ECHONL);*/
	new_attr.c_cc[VMIN] = 1;
	new_attr.c_cc[VTIME] = 0;
	if(tcsetattr(fd, TCSANOW, &new_attr) != 0){
		fprintf(stderr, "터미널 속성을 설정할 수 없음.");
	}
	printf("다음 문장을 똑같이 입력하세요. \n%s\n", text);
	t = myclock();
	
	while((nread = read(fd, &ch, 1)) > 0 && ch != '\n'){
		if(ch == text[cnt++]){
			write(fd, &ch, 1);
			count++;
		}
		
		else{
			write(fd, "*", 1);
			errcnt++;
		}
	} 
	
	printf("타이핑 오류의 횟수 %d\n", errcnt);
	
	dt = myclock() - t;
	
	printf("분당 평균 타자 수 : %d // %d sec \n", count, dt/1000);
	
	tcsetattr(fd, TCSANOW, &init_attr);
	
	close(fd);
	
	return 0;
}
