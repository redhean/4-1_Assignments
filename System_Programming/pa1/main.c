#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include "header.h"


int main(int argc, char* argv[]){
	
	// if there are no right input, then exit
	if(argc != 2){
		exit(1);
	}
	
	// read input file
	int fd1;
	fd1 = open(argv[1], O_RDONLY);
	char buffer[513];

	while(1){
		char * ans = (char*)malloc(sizeof(char) * 100000);
		char alphabet[513];
		char * temp = (char*)malloc(sizeof(char) * 512);
		my_memset(temp, 0, sizeof(char)*512);

		// read word
		ssize_t nread = read(0, buffer, sizeof(buffer) - 1);
		if(buffer[nread - 1] == '\n'){
			buffer[nread - 1] = '\0';
		}
		
		// terminate program
		if(my_strcmp(buffer,"PA1EXIT") == 0){
			break;
		}
		// make lowercase
		else{
			to_lower(buffer);
		}
		
		// Searching simple regular expressing keyword locations
		if(my_strchr(buffer, '*') != NULL){
			char** arg;
			size_t count = 0;
			arg = my_strtok(buffer, "*",&count);

			int cnt = 0;
			int line = 1;
			int start = 0;
			int middle = 0;
			int end = 0;
			char line_num[100] = "";

			while(read(fd1, alphabet, 1) > 0){
				if(alphabet[0] == ' '){
					temp[cnt] = '\0';

					if(my_strcmp(temp,arg[0]) == 0) start++;
					if(my_strcmp(temp, arg[1]) == 0 && start != 0) end++;
					if(start != 0 && end == 0 && my_strcmp(temp, arg[0]) != 0 && my_strcmp(temp, arg[1]) != 0 ){
						middle++;
					}
					
					my_memset(temp, 0, sizeof(char) * 512);
					cnt = 0;
				}
				else if(alphabet[0] == '\n'){
					temp[cnt] = '\0';

					if(my_strcmp(temp, arg[1]) == 0) end++;
					
					if(start != 0 && middle != 0 && end != 0){
						itoa(line, line_num);
						my_strcat(ans, line_num);
						my_strcat(ans, " ");
						my_memset(temp, 0, sizeof(char) * 512);
						cnt = 0;
						line++;
					}
					else{
						my_memset(temp, 0, sizeof(char) * 512);
						cnt = 0;
						line++;
					}
					start = 0;
					middle = 0;
					end = 0;
				}
				else{
					to_lower(alphabet);
					temp[cnt] = alphabet[0];
					cnt++;
				}
			}
		}
		// Searching several consecutive words locations
		else if(buffer[0] == '"'){
			char** arg;
			for(int i = 1; i < nread - 2; i++){
				temp[i - 1] = buffer[i];
			}
			size_t count = 0;
			arg = my_strtok(temp, " ", &count);

			int* arg_lens = (int*)malloc(sizeof(int) * count);
			for(int i = 0; i < count; i++){
				arg_lens[i] = my_strlen(arg[i]);
			}
			
			char** arg_copy = (char**)malloc(sizeof(char*) * count);
			for(int i = 0; i < count; i++){
				arg_copy[i] = (char*)malloc(sizeof(char) * (arg_lens[i] + 1));
				my_strcpy(arg_copy[i], arg[i]);
				arg[i] = arg_copy[i];
			}

			int cnt = 0;
			int line = 1;
			int location = 0;
			char line_num[100] = "";
			char location_num[100] = "";
			// save start location
			int start = 0;
			char* chk = (char*)malloc(sizeof(char) * count);
			my_memset(chk, 0, sizeof(char) * count);
			
			int cnt_word = 0;
			while(read(fd1, alphabet, 1) > 0){
				if(alphabet[0] == ' '){
					temp[cnt] = '\0';
						
					for(int i = 0; i < count; i++){
						if(my_strcmp(temp, arg_copy[i]) == 0){
							if(i == 0){
								if(start == 0)
									start = location - cnt;
							       	chk[i] = 1;
								cnt_word = 1;
							}
							else{
								if(chk[i - 1] == 1){
									chk[i] = 1;
									cnt_word = 1;
								}
							}
						}
					}
					
					int temp_chk = 0;
					for(int i = 0; i < count; i++){
						if(chk[i] != 0) temp_chk++;
					}	

					if(temp_chk != count && cnt_word == 0){
						my_memset(chk, 0, sizeof(char) * count);
						start = 0;
					}

					my_memset(temp, 0, sizeof(char) * 512);
					cnt = 0;
					cnt_word = 0;
					location++;
				}
				else if(alphabet[0] == '\n'){
					temp[cnt] = '\0';

					for(int i = 0; i < count; i++){
						if(my_strcmp(temp, arg_copy[i]) == 0){
							if(i == 0){
								if(start == 0)
									start = location - cnt;
							       	chk[i] = 1;
								cnt_word = 1;
							}
							else{
								if(chk[i - 1] == 1){
									chk[i] = 1;
									cnt_word = 1;
								}
							}
						}		
					}
					int temp_chk = 0;
					for(int i = 0; i < count; i++){
						if(chk[i] != 0) temp_chk++;
					}

					if(cnt_word == 0 && temp_chk != count){
						my_memset(chk, 0, sizeof(char) * count);
						start = 0;
					}

					if(temp_chk == count){
						itoa(line, line_num);
						itoa(start, location_num);
						my_strcat(ans,line_num);
						my_strcat(ans, ":");
						my_strcat(ans, location_num);
						my_strcat(ans, " ");
						my_memset(temp, 0, sizeof(char) * 512);
						my_memset(chk, 0, sizeof(char) * count);
						cnt = 0;
						location = 0;
						start = 0;
					}
					else{
						my_memset(temp, 0, sizeof(char) * 512);
						my_memset(chk, 0, sizeof(char) * count);
						cnt = 0; 
						location = 0;
						start = 0;
					}
					line++;
				}
				else{
					to_lower(alphabet);
					temp[cnt] = alphabet[0];
					cnt++;
					location++;
				}								
			}
		}
		// Searching several words locations
		else if(my_strchr(buffer, ' ') != NULL){
			char ** arg;
			size_t count = 0;
			arg = my_strtok(buffer, " ", &count);
			
			int cnt = 0;
			int line = 1;
			char line_num[100] = "";
			char* chk = (char*)malloc(sizeof(char)*count);
			my_memset(chk, 0, sizeof(char) * count);

			while(read(fd1, alphabet, 1) > 0){
				if(alphabet[0] == ' '){
					temp[cnt] = '\0';

					for(int i = 0; i < count; i++){
						if(my_strcmp(temp, arg[i]) == 0){
							chk[i] = 1;
						}
					}
					
					my_memset(temp, 0, sizeof(char) * 512);
					cnt = 0;
				}
				else if(alphabet[0] == '\n'){
					temp[cnt] = '\0';

					for(int i = 0; i < count; i++){
						if(my_strcmp(temp, arg[i]) == 0){
							chk[i] = 1;
						}
					}
					
					int temp_chk = 0;
					for(int i = 0; i < count; i++){
						if(chk[i] != 0) temp_chk++;
					}

					if(temp_chk == count){
						itoa(line, line_num);
						my_strcat(ans, line_num);
						my_strcat(ans, " ");
						my_memset(temp, 0, sizeof(char) * 512);
						my_memset(chk, 0, sizeof(char) * count);
						cnt = 0;
						line++;
					}
					else{
						my_memset(temp, 0, sizeof(char) * 512);
						my_memset(chk, 0, sizeof(char) * count);
						cnt = 0;
						line++;
					}
				}
				else{
					to_lower(alphabet);
					temp[cnt] = alphabet[0];
					cnt++;	
				}	
			}
		}
		// Searching single word locations
		else{
			int line = 1;
			int location = 0;
			int cnt = 0;
			char line_num[100] = "";
			char location_num[100] = "";

			while(read(fd1,alphabet,1) > 0){
				
				if(alphabet[0] == '\n'){
					if(my_strcmp(buffer,temp) == 0){
						itoa(line, line_num);	
						itoa(location - cnt, location_num);
						my_strcat(ans, line_num);
						my_strcat(ans, ":");
						my_strcat(ans, location_num);
						my_strcat(ans, " ");

						my_memset(temp, 0, sizeof(char) * 512);
						line++;
						location = 0;
						cnt = 0;
					}
					else{
						my_memset(temp, 0, sizeof(char) * 512);
						line++;
						location = 0;
						cnt = 0;
					}
				}
				else if(alphabet[0] == ' '){
					temp[cnt] = '\0';

					if(my_strcmp(buffer,temp) == 0){
						itoa(line, line_num);
						itoa(location - cnt, location_num);
						my_strcat(ans, line_num);
						my_strcat(ans, ":");
						my_strcat(ans, location_num);
						my_strcat(ans, " ");
						
						// initialize
						my_memset(temp, 0, sizeof(char) * 512);
						location++;
						cnt = 0;
					}
					else{
						my_memset(temp, 0, sizeof(char) * 512);
						location++;
						cnt = 0;
					}
				}
				else{	
					to_lower(alphabet);
					temp[cnt] = alphabet[0];
					cnt++;
					location++;
				}
			}
		}
		// move to front
		lseek(fd1, 0, SEEK_SET);		

		write(1, ans, my_strlen(ans));
		write(1, "\n", 1);
	}	
	return 0;
}
