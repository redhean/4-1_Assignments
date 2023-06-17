#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <syslog.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>

int main(void) {
	unsigned int pid;
	time_t t;
	struct tm tm;
	int fd;
	char *argv[3];
	char buf[512];
	int fd0, fd1, fd2;

	fd = open("./crontab", O_RDWR);
	pid = fork();
	
	if(pid == -1) return -1;
	if(pid != 0)
		exit(0);
	if(setsid() < 0)
		exit(0);
	if(chdir("/") < 0)
		exit(0);

	umask(0);

	close(0);
	close(1);
	close(2);

	fd0 = open("/dev/null", O_RDWR);
	fd1 = open("/dev/null", O_RDWR);
	fd2 = open("/dev/null", O_RDWR);

	t = time(NULL);
	localtime_r(&t, &tm);




	while (1)
	{
		buf[0] = '\0';	

		//insert your code

		//##  hints  ##

		//strtok_r();
		//pid = fork();
		//execl("/bin/sh", "/bin/sh", "-c", argv[2], (char*) NULL);

		
		read(fd, buf, 512);

		char*temp = buf;
		lseek(fd, 0, SEEK_SET);

		for(int i = 0; i < 3; i++){
			argv[i] = strtok_r(temp, " ", &temp);
		}

		int first = 0;
		int second = 0;

		if(argv[0][0] == '*' && argv[1][0] == '*'){
			if(fork() == 0)
				execl("/bin/sh", "/bin/sh", "-c", "argv[2]", (char*) NULL);
		}
		else if(argv[0][0] != '*' && argv[1][0] == '*'){
			first = atoi(argv[0]);
			if(first == tm.tm_min){
				if(fork() == 0)
					execl("/bin/sh", "/bin/sh", "-c", "argv[2]", (char*) NULL);
			}
		}
		else if(argv[0][0] == '*' && argv[1][0] != '*'){
			second = atoi(argv[1]);
			if(second == tm.tm_hour){
				if(fork() == 0)
					execl("/bin/sh", "/bin/sh", "-c", "argv[2]", (char*) NULL);
			}
		}
		else{
			first = atoi(argv[0]);
			second = atoi(argv[1]);
			if(first == tm.tm_min && second == tm.tm_hour){
				if(fork() == 0)
					execl("/bin/sh", "/bin/sh", "-c", "argv[2]", (char*) NULL);
			}
		}
		
		t = time(NULL);
		localtime_r(&t, &tm);

		sleep(60 - tm.tm_sec % 60);
		
	}

	return 0;
}
