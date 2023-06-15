#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

int pow1(int a, int b){
	
	int ans = 1;
	while(b){
		if(b & 1)
			ans *= a;
		b >>= 1;
		a *= a;
	}

	return ans;
}

char* u_type(char* bits){
	
	// return string
	char* ans = (char*)malloc(sizeof(char) * 100);

	// 0 ~ 19
	int immediate = 0;

	// 20 ~ 24
	int rd = 0;

	// 25 ~ 31
	int opcode = 0;

	// string variables
	char* instruction;
	char rd_str[20] = "x";
	char imm_str[20] = "";
	char temp_buf1[20];

	// calculate immediate
	for(int i = 19, j = 0; i > 0, j < 19; i--, j++){
		// positive
		if(bits[0] == '0'){
			if(bits[i] == '1')
				immediate += pow1(2, j);
		}
		// negative
		else{
			if(bits[i] == '0')
				immediate += pow1(2, j);
		}
	}

	if(bits[0] == '1')
		immediate = (immediate + 1) * -1;

	immediate *= pow1(2, 12);
	sprintf(temp_buf1, "%d", immediate);
	strcat(imm_str, temp_buf1);

	//  calculate rd
	for(int i = 24, j = 0; i >= 20, j < 5; i--, j++){
		if(bits[i] == '1')
			rd += pow1(2, j);
	}

	sprintf(temp_buf1, "%d, ", rd);
	strcat(rd_str, temp_buf1);

	// calculate opcode
	for(int i = 31, j = 0; i >= 25, j < 7; i--, j++){
		if(bits[i] == '1')
			opcode += pow1(2, j);
	}

	// find instruction
	if(opcode == 55) instruction = "lui ";
	else if(opcode == 23) instruction = "auipc ";

	// make result
	strcat(ans, instruction);
	strcat(ans, rd_str);
	strcat(ans, imm_str);

	return ans;
}

char* j_type(char* bits){
	
	// return string
	char* ans = (char*)malloc(sizeof(char) * 100);

	// 0 ~ 19
	int immediate = 0;

	// 20 ~ 24
	int rd = 0;

	// string variables
	char* instruction = "jal ";
	char rd_str[20] = "x";
	char imm_str[20] = "";
	char temp_buf1[20];

	// calculate immediate
	int imm_temp[20] = {0,};

		// 20
	if(bits[0] == '1') imm_temp[0] = 1;
	else imm_temp[0] = 0;

		// 1 ~ 10
	for(int i = 1; i < 11; i++){
		if(bits[i] == '1') imm_temp[i + 9] = 1;
		else imm_temp[i + 9] = 0;
	}
	
		// 11
	if(bits[11] == '1') imm_temp[9] = 1;
	else imm_temp[9] = 0;

		// 12 ~ 19
	for(int i = 12; i < 20; i++){
		if(bits[i] == '1') imm_temp[i - 11] = 1;
		else imm_temp[i - 11] = 0;
	}

	for(int i = 19, j = 0; i > 0, j < 19; i--, j++){
		// positive
		if(bits[0] == '0'){
			if(imm_temp[i] == 1)
				immediate += pow1(2, j);
		}
		// negative
		else{
			if(imm_temp[i] == 0)
				immediate += pow1(2, j);
		}
	}

	if(bits[0] == '1')
		immediate = (immediate + 1) * -1;
	
	immediate *= 2;
	sprintf(temp_buf1, "%d", immediate);
	strcat(imm_str, temp_buf1);

	// calculate rd
	for(int i = 24, j = 0; i >= 20, j < 5; i--, j++){
		if(bits[i] == '1')
			rd += pow1(2, j);
	}

	sprintf(temp_buf1, "%d, ", rd);
	strcat(rd_str, temp_buf1);

	// make result
	strcat(ans, instruction);
	strcat(ans, rd_str);
	strcat(ans, imm_str);
	
	return ans;
}

char* i_type(char* bits){
	
	char* ans = (char*)malloc(sizeof(char) * 100);

	// 0 ~ 11
	int immediate = 0;

	// 0 ~ 6
	int immediate2 = 0;

	// 7 ~ 11
	int shamt = 0;

	// 17 ~ 19
	int funct3 = 0;

	// 12 ~ 16
	int rs1 = 0;

	// 20 ~ 24
	int rd = 0;

	// 25 ~ 31
	int opcode = 0;
	
	// string variables
	char* instruction;
	char rs1_str[20] = "(x";
	char rs1_str1[20] = "x";	// for opcode 19
	char rd_str[20] = "x";
	char imm_str[20] = "";
	char shamt_str[20] = "";
	char temp_buf1[20];
	
	// calculate funct3
	for(int i = 19, j = 0; i >= 17, j < 3; i--, j++){
		if(bits[i] == '1')
			funct3 += pow1(2, j);
	}

	// calculate immediate2
	for(int i = 6, j = 0; i >= 0, j < 7; i--, j++){
		if(bits[i] == '1')
			immediate2 += pow1(2, j);
	}

	// calculate shamt
	for(int i = 11, j = 0; i >= 7, j < 5; i--, j++){
		if(bits[i] == '1')
			shamt += pow1(2, j);
	}

	sprintf(temp_buf1, "%d", shamt);
	strcat(shamt_str, temp_buf1);

	// calculate immediate
	for(int i = 11, j = 0; i > 0, j < 11; i--, j++){
		// positive
		if(bits[0] == '0'){
			if(bits[i] == '1')
				immediate += pow1(2, j);
		}
		// negative
		else{
			if(bits[i] == '0')
				immediate += pow1(2, j);
		}
			
	}

	if(bits[0] == '1'){
		immediate  = (immediate + 1) * -1;
	}

	sprintf(temp_buf1, "%d", immediate);
	strcat(imm_str, temp_buf1);

	// calculate rs1
	for(int i = 16, j = 0; i >= 12, j < 5; i--, j++){
		if(bits[i] == '1')
			rs1 += pow1(2, j);
	}
	
	sprintf(temp_buf1, "%d)", rs1);
	strcat(rs1_str, temp_buf1);
	sprintf(temp_buf1, "%d, ", rs1);
	strcat(rs1_str1, temp_buf1);

	// calculate rd
	for(int i = 24, j = 0; i >= 20, j < 5; i--, j++){
		if(bits[i] == '1')
			rd += pow1(2, j);
	}

	sprintf(temp_buf1, "%d, ", rd);
	strcat(rd_str, temp_buf1);

	// calculate opcode
	for(int i = 31, j = 0; i >= 23, j < 7; i--, j++){
		if(bits[i] == '1')
			opcode += pow1(2, j);
	}
	
	// find instruction
	if(opcode == 103) instruction = "jalr ";
	else if(opcode == 3 && funct3 == 0) instruction = "lb ";
	else if(opcode == 3 && funct3 == 1) instruction = "lh ";
	else if(opcode == 3 && funct3 == 2) instruction = "lw ";
	else if(opcode == 3 && funct3 == 4) instruction = "lbu ";
	else if(opcode == 3 && funct3 == 5) instruction = "lhu ";
	else if(opcode == 19 && funct3 == 0) instruction = "addi ";
	else if(opcode == 19 && funct3 == 2) instruction = "slti ";
	else if(opcode == 19 && funct3 == 3) instruction = "sltiu ";
	else if(opcode == 19 && funct3 == 4) instruction = "xori ";
	else if(opcode == 19 && funct3 == 6) instruction = "ori ";
	else if(opcode == 19 && funct3 == 7) instruction = "andi ";
	else if(opcode == 19 && funct3 == 1) instruction = "slli ";
	else if(opcode == 19 && funct3 == 5 && immediate2 == 0) instruction = "srli ";
	else if(opcode == 19 && funct3 == 5 && immediate2 == 32) instruction = "srai ";

	// make result
	if(strcmp(instruction, "slli ") == 0 || strcmp(instruction, "srli ") == 0 || strcmp(instruction, "srai ") == 0){
		strcat(ans, instruction);
		strcat(ans, rd_str);
		strcat(ans, rs1_str1);
		strcat(ans, shamt_str);	
	}
	else if(opcode == 3 || opcode == 103){
		strcat(ans, instruction);
		strcat(ans, rd_str);
		strcat(ans, imm_str);
		strcat(ans, rs1_str);
	}
	else if(opcode == 19){
		strcat(ans, instruction);
		strcat(ans, rd_str);
		strcat(ans, rs1_str1);
		strcat(ans, imm_str);
	}
	

	return ans;
}

char* b_type(char* bits){
	
	// return string
	char* ans = (char*)malloc(sizeof(char) * 100);

	// 0 ~ 6, 20 ~ 24
	int immediate = 0;

	// 7 ~ 11
	int rs2 = 0;

	// 12 ~ 16
	int rs1 = 0;

	// 17 ~ 19
	int funct3 = 0;

	// string variables
	char* instruction;
	char rs1_str[20] = "x";
	char rs2_str[20] = "x";
	char imm_str[20] = "";
	char temp_buf1[20];

	// calculate immediate
	int imm_temp[12] = {0,};

		// 12
	if(bits[0] == '1') imm_temp[0] = 1;
	else imm_temp[0] = 0;

		// 5 ~ 10
	for(int i = 1; i < 7; i++){
		if(bits[i] == '1') imm_temp[i + 1] = 1;
		else imm_temp[i + 1] = 0;
	}

		// 1 ~ 4
	for(int i = 20; i < 24; i++){
		if(bits[i] == '1') imm_temp[i - 12] = 1;
		else imm_temp[i - 12] = 0;
	}

		// 11
	if(bits[0] == '1') imm_temp[1] = 1;
	else imm_temp[1] = 0;
	
	// calculate immediate
	for(int i = 11, j = 0; i > 0, j < 11; i--, j++){
		// positive
		if(bits[0] == '0'){
			if(imm_temp[i] == 1)
				immediate += pow1(2, j);
		}
		//negative
		else{
			if(imm_temp[i] == 0)
				immediate += pow1(2, j);
		}
	}
	
	if(bits[0] == '1')
		immediate = (immediate + 1) * -1;
	

	immediate *= 2;
	sprintf(temp_buf1, "%d", immediate);
	strcat(imm_str, temp_buf1);
	
	// calculate rs1
	for(int i = 16, j = 0; i >= 12, j < 5; i--, j++){
		if(bits[i] == '1')
			rs1 += pow1(2, j);
	}

	sprintf(temp_buf1, "%d, ", rs1);
	strcat(rs1_str, temp_buf1);

	// calculate rs2
	for(int i = 11, j = 0; i >= 7, j < 5; i--, j++){
		if(bits[i] == '1')
			rs2 += pow1(2, j);
	}

	sprintf(temp_buf1, "%d, ", rs2);
	strcat(rs2_str, temp_buf1);

	// calculate funct3
	for(int i = 19, j = 0; i >= 17, j < 3; i--, j++){
		if(bits[i] == '1')
			funct3 += pow1(2, j);
	}


	// find instruction
	if(funct3 == 0) instruction = "beq ";
	else if(funct3 == 1) instruction = "bne ";
	else if(funct3 == 4) instruction = "blt ";
	else if(funct3 == 5) instruction = "bge ";
	else if(funct3 == 6) instruction = "bltu ";
	else if(funct3 == 7) instruction = "bgeu ";

	// make result
	strcat(ans, instruction);
	strcat(ans, rs1_str);
	strcat(ans, rs2_str);
	strcat(ans, imm_str);
	
	return ans;
}

char* s_type(char* bits){
	
	// return stirng
	char* ans = (char*)malloc(sizeof(char) * 100);

	// 0 ~ 6, 20 ~ 24
	int immediate = 0;

	// 7 ~ 11
	int rs2 = 0;

	// 12 ~ 16
	int rs1 = 0;

	// 17 ~ 19
	int funct3 = 0;

	// string variables
	char* instruction;
	char rs1_str[20] = "(x";
	char rs2_str[20] = "x";
	char imm_str[20] = "";
	char temp_buf1[20];

	// calculate immediate
	int imm_temp[12] = {0,};

	for(int i = 24; i >= 20; i--){
		if(bits[i] == '1') imm_temp[i - 13] = 1;
		else imm_temp[i - 13] = 0;
	}

	for(int i = 6; i >= 0; i--){
		if(bits[i] == '1') imm_temp[i] = 1;
		else imm_temp[i] = 0;
	}

	for(int i = 11, j = 0; i > 0, j < 11; i--, j++){
		// positive
		if(bits[0] == '0'){
			if(imm_temp[i] == 1)
				immediate += pow1(2, j);
		}
		// negative
		else{
			if(imm_temp[i] == 0)
				immediate += pow1(2, j);
		}
	}

	if(bits[0] == '1')
		immediate = (immediate + 1) * -1;

	sprintf(temp_buf1, "%d", immediate);
	strcat(imm_str, temp_buf1);

	// calculate rs1
	for(int i = 16, j = 0; i >= 12, j < 5; i--, j++){
		if(bits[i] == '1')
			rs1 += pow1(2, j);
	}

	sprintf(temp_buf1, "%d)", rs1);
	strcat(rs1_str, temp_buf1);

	// calculate rs2
	for(int i = 11, j = 0; i >= 7, j < 5; i--, j++){
		if(bits[i] == '1')
			rs2 += pow1(2, j);
	}

	sprintf(temp_buf1, "%d, ", rs2);
	strcat(rs2_str, temp_buf1);

	// calculate funct3
	for(int i = 19, j = 0; i >= 17, j < 3; i--, j++){
		if(bits[i] == '1')
			funct3 += pow1(2, j);
	}

	// find instruction
	if(funct3 == 0) instruction = "sb ";
	else if(funct3 == 1) instruction = "sh ";
	else if(funct3 == 2) instruction = "sw ";

	// make result
	strcat(ans, instruction);
	strcat(ans, rs2_str);
	strcat(ans, imm_str);
	strcat(ans, rs1_str);

	return ans;
}

char* r_type(char* bits){
	
	// return string
	char* ans = (char*)malloc(sizeof(char) * 100);

	// 25 ~ 31 -> 0 ~ 6
	int funct7 = 0;

	// 12 ~ 14 -> 17 ~ 19 
	int funct3 = 0;

	// 20 ~ 24 -> 7 ~ 11
	int rs2 = 0;

	// 15 ~ 19 -> 12 ~ 16
	int rs1 = 0;

	// 7 ~ 11 -> 20 ~ 24
	int rd = 0;
	
	// string variables
	char* instruction;
	char rd_str[20] = "x";
	char rs1_str[20] = "x";
	char rs2_str[20] = "x";
	char temp_buf1[20];
	
	// calculate funct7
	for(int i = 6, j = 0; i >= 0, j < 7; i--, j++){
		if(bits[i] == '1')
			funct7 += pow1(2,j);
	}	
	
	// calculate funct3
	for(int i = 19, j = 0; i >= 17, j < 3; i--, j++){
		if(bits[i] == '1')
			funct3 += pow1(2,j);
	}

	// calculate rs2
	for(int i = 11, j = 0; i >= 7, j < 5; i--, j++){
		if(bits[i] == '1')
			rs2 += pow1(2,j);
	}

	sprintf(temp_buf1,"%d", rs2);
	strcat(rs2_str, temp_buf1);
	

	// calculate rs1
	for(int i = 16, j = 0; i >= 12, j < 5; i--, j++){
		if(bits[i] == '1')
			rs1 += pow1(2, j);
	}

	sprintf(temp_buf1, "%d, ", rs1);
	strcat(rs1_str, temp_buf1);

	// calculate rd
	for(int i = 24, j = 0; i >= 20, j < 5; i--, j++){
		if(bits[i] == '1')
			rd += pow1(2, j);
	}

	sprintf(temp_buf1, "%d, ", rd);
	strcat(rd_str, temp_buf1);

	//find instruction
	if(funct7 == 0 && funct3 == 0) instruction = "add ";
	else if(funct7 == 32 && funct3 == 0) instruction = "sub ";
	else if(funct7 == 0 && funct3 == 1) instruction = "sll ";
	else if(funct7 == 0 && funct3 == 2) instruction = "slt ";
	else if(funct7 == 0 && funct3 == 3) instruction = "sltu ";
	else if(funct7 == 0 && funct3 == 4) instruction = "xor ";
	else if(funct7 == 0 && funct3 == 5) instruction = "srl ";
	else if(funct7 == 32 && funct3 == 5) instruction = "sra ";
	else if(funct7 == 0 && funct3 == 6) instruction = "or ";
	else if(funct7 == 0 && funct3 == 7) instruction = "and ";
	
	// make result
	strcat(ans, instruction);
	strcat(ans, rd_str);
	strcat(ans, rs1_str);
	strcat(ans, rs2_str);
	
	return ans;
	
}

char* print_hex(char* bits){
	char* ans = (char*)malloc(sizeof(char) * 9);

	for(int i = 0; i < 8; i++){
		int num = 0;
		for(int j = i * 4 + 3, k = 0; j >= i * 4, k < 4; j--, k++){
			if(bits[j] == '1'){
				num += pow1(2, k);
			}
		}
		if(num < 10)
			ans[i] = num + '0';
		else if(num == 10)
			ans[i] = 'a';
		else if(num == 11)
			ans[i] = 'b';
		else if(num == 12)
			ans[i] = 'c';
		else if(num == 13)
			ans[i] = 'd';
		else if(num == 14)
			ans[i] = 'e';
		else
			ans[i] = 'f';

	}

	return ans;
}

int main(int argc, char* argv[]){

	// if there are not appropriate input, then exit
	if(argc != 2){
		exit(1);
	}

	FILE* input = fopen(argv[1], "rb");
	int ch;

	// count the number of input by 1 byte (4 is 32bits)
	int idx = 0;

	// store the binary file into int type 
	int* file_contents = (int*)malloc(sizeof(int)*10000);
	
	// read binary file
	while((ch = fgetc(input)) != EOF){
		file_contents[idx] = ch;
		idx++;	
	}
	
	// change into 32bits string
	char ** bits = (char**)malloc(sizeof(char*)*idx);
	for(int i = 0; i < idx; i++){
		bits[i] = (char*)malloc(sizeof(char)*33);
	}
	
	int ten_to_binary[8];	// store binary number
	int cnt_32 = 32;		// count 32 bits

	for(int i = 0; i < idx; i++){
		int cnt = 0;	// count the number of binary bit
		int ten = file_contents[i];	// decimal number	
		
		// Initialization
		for(int j = 0; j < 8; j++){
			ten_to_binary[j] = 0;
		}

		while(ten > 0){
			ten_to_binary[cnt] = ten % 2;
			ten /= 2;
			cnt++;
		}

		// fill the binary bits
		for(int j = 0; j <= cnt - 1; j++){
			bits[i / 4][cnt_32 - 1] = ten_to_binary[j] + '0';
			cnt_32--;
		}

		for(int j = 0; j < 8 - cnt; j++){
			bits[i / 4][cnt_32 - 1] = 0 + '0';
			cnt_32--;
		}

		// if cnt_32 == 32, one instruction is done. initialize the cnt_32 to 0
		if(cnt_32 == 0)
			cnt_32 = 32;
		
	}

	// print result
	for(int i = 0; i < idx / 4; i++){
		printf("inst %d: ",i);
		printf("%s ",print_hex(bits[i]));

		// print result of each type
		char* opcode = (char*)malloc(sizeof(char) * 8);
		for(int j = 25; j < 32; j++){
			opcode[j - 25] = bits[i][j];
		}

		int opcode_num = 0;
		for(int j = 6, k = 0; j >= 0, k <= 6; j--, k++){
			if(opcode[j] == '1')
				opcode_num += pow1(2,k);	
		}
		
		if(opcode_num == 55 || opcode_num == 23) printf("%s",u_type(bits[i]));
		else if(opcode_num == 111) printf("%s",j_type(bits[i]));
		else if(opcode_num == 99) printf("%s",b_type(bits[i]));
		else if(opcode_num == 103 || opcode_num == 3 || opcode_num == 19) printf("%s",i_type(bits[i]));
		else if(opcode_num == 35) printf("%s", s_type(bits[i]));
		else if(opcode_num == 51) printf("%s", r_type(bits[i]));
		else printf("unknown instruction");

		printf("\n");
	}
	
	fclose(input);
	for(int i = 0; i < idx; i++){
		free(bits[i]);
	}
	free(bits);
	return 0;
}
