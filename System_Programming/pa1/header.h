// header.h

#ifndef HEADER_H
#define HEADER_H

#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

size_t my_strlen(const char* str){
	
	// if NULL
	if(str == NULL){
		return -1;
	}

	size_t len = 0;
	while(str[len] != '\0') {
		len++;
	}

	return len;
}

int my_strcmp(const char* str1, const char* str2){
	
	// if NULL
	if(str1 == NULL || str2 == NULL){
		return -1;
	}

	size_t len1 = my_strlen(str1);
	size_t len2 = my_strlen(str2);

	if(len1 != len2){
		return len1 > len2 ? 1 : -1;
	}

	for(int i = 0; i < len1; i++){
		if(str1[i] != str2[i]){
			return str1[i] > str2[i] ? 1 : -1;
		}
	}

	return 0;
}

char* my_strchr(const char* str, int c){
	for(int i = 0; str[i] != '\0'; i++){
		if(str[i] == c){
			return (char*)(str + i);
		}
	}	
	return NULL;
}

char** my_strtok(char * str, const char* delimiter, size_t* count){
	
	char** tokens = NULL;
	size_t num_tokens = 0;

	while(*str != '\0'){
		
		while(*str == *delimiter){
			str++;
		}
		
		// break
		if(*str == '\0'){
			break;
		}
		
		// set start point
		char* token_start = str;
		
		// fine end of string
		while(*str != '\0' && *str != *delimiter){
			str++;
		}

		if(*str == *delimiter){
			*str = '\0';
			str++;
		}

		tokens = realloc(tokens, sizeof(char*) * (num_tokens + 1));
		tokens[num_tokens] = token_start;
		num_tokens++;
	}

	*count = num_tokens;
	return tokens;
}

char* my_strcat(char* dest, const char* src){
	size_t dest_len = my_strlen(dest);
	int i;
	for(i = 0; src[i] != '\0'; i++){
		dest[dest_len + i] = src[i];
	}

	dest[dest_len + i] = '\0';

	return dest;
}

void my_strcpy(char* dest, const char* src) {
	while(*src != '\0') {
		*dest = *src;
		dest++;
		src++;
	}
	*dest = '\0';
}

void itoa(int num, char *buf){
	if(num == 0){
		buf[0] = '0';
		buf[1] = '\0';
		return;
	}

	int len = 0;
	int temp = num;
	while(temp != 0){
		len++;
		temp /= 10;
	}

	for(int i = 0; i < len; i++){
		buf[len - 1 -i] = '0' + num % 10;
		num /= 10;
	}
	buf[len] = '\0';
}

void* my_memset(void* ptr, int value, size_t num){
	unsigned char* p = ptr;
	size_t i = 0;
	while(i < num){
		p[i] = (unsigned char)value;
		i++;
	}
	return ptr;
}

void to_lower(char* str){
	while(*str){
		if(*str >= 'A' && *str <= 'Z'){
			*str = *str + 32;
		}
		str++;
	}
}
#endif
