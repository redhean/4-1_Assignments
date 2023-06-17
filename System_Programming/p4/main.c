#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

void swap(char * x, char *y){
	char t = *x;
	*x = *y;
	*y = t;
}

char* reverse(char *buffer, int i, int j){
	while(i < j){
		swap(&buffer[i++], &buffer[j--]);
	}
	return buffer;
}

char* itoa(int value, char* buffer, int base){

	if(base < 2 || base > 32){
		return buffer;
	}

	int i = 0;
	int n = value;
	while(n){
		int r = n % base;

		if(r >= 10){
			buffer[i++] = 65 + (r - 10);
		}
		else{
			buffer[i++] = 48 + r;
		}
		n = n / base;
	}
	if(i == 0){
		buffer[i++] = '0';
	}

	if(value < 0 && base == 10){
		buffer[i++] = '-';
	}

	buffer[i] = '\0';
	
	return reverse(buffer, 0, i - 1);
	
}

int get_len(char* arr){
	int cnt = 1;
	char n = arr[1];
	while(n != '\0'){
		cnt++;
		n = arr[cnt];
	}
	return cnt;
}

int main(int argc, char* argv[]){
	if(argc != 2){
		exit(1);
	}
	
	int fd1;
	int fd2;

	char buffer[1024];
	fd1 = open(argv[1], O_RDONLY);
	fd2 = open("Aladdin_num.txt",O_RDWR | O_CREAT, S_IRUSR | S_IRGRP);
	
	int cnt = 0;
	char num[33];
	itoa(cnt + 1, num, 10);
	int len = get_len(num);
	write(fd2, num ,len);
	write(fd2, " | ", 3);

	while(read(fd1,buffer,1) > 0){
		int chk = 0;
		if(buffer[0] == '\n'){
			cnt++;
			itoa(cnt + 1, num, 10);
			len = get_len(num);
			write(fd2, buffer, 1);
			write(fd2, num ,len);
			write(fd2, " | ", 3);
		}else
			write(fd2, buffer,1);
	
	}
	
	close(fd1);
	close(fd2);	
}
