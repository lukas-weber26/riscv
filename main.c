#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef int32_t raw_instruction;
typedef enum {R, I, S, B, U, J} instruction_type;

typedef struct instruction {
	raw_instruction raw;
	instruction_type type;
	int32_t opcode;
	int32_t rd;
	int32_t funct3;
	int32_t rs1;
	int32_t rs2;
	int32_t funct7;
	int32_t im1;
	int32_t im2;
	int32_t im3;
	int32_t im4;
} instruction;

uint32_t shift_and_mask(instruction * instruction, int32_t mask, int32_t shift) {
	return ((instruction->raw>> shift) & mask);
}

void resolve_r_type(instruction * new_instruction) {
	new_instruction->rd = shift_and_mask(new_instruction, 0b11111, 7);
	new_instruction->funct3 = shift_and_mask(new_instruction, 0b111, 12);
	new_instruction->rs1 = shift_and_mask(new_instruction, 0b11111, 15);
	new_instruction->rs2 = shift_and_mask(new_instruction, 0b11111, 20);
	new_instruction->funct7 = shift_and_mask(new_instruction, 0b1111111, 25);
}

void resolve_i_type(instruction * new_instruction) {
	new_instruction->rd = shift_and_mask(new_instruction, 0b11111, 7);
	new_instruction->funct3 = shift_and_mask(new_instruction, 0b111, 12);
	new_instruction->rs1 = shift_and_mask(new_instruction, 0b11111, 15);
	new_instruction->im1= shift_and_mask(new_instruction, 0b111111111111, 20);
}

void resolve_s_type(instruction * new_instruction) {
	new_instruction->funct3 = shift_and_mask(new_instruction, 0b111, 12);
	new_instruction->rs1 = shift_and_mask(new_instruction, 0b11111, 15);
	new_instruction->rs2 = shift_and_mask(new_instruction, 0b11111, 20);
	new_instruction -> im1 = shift_and_mask(new_instruction, 0b11111, 7);
	new_instruction->im2 = shift_and_mask(new_instruction, 0b1111111, 25);
}

void resolve_b_type(instruction * new_instruction) {
	new_instruction->funct3 = shift_and_mask(new_instruction, 0b111, 12);
	new_instruction->rs1 = shift_and_mask(new_instruction, 0b11111, 15);
	new_instruction->rs2 = shift_and_mask(new_instruction, 0b11111, 20);

	new_instruction->im1 = shift_and_mask(new_instruction, 0b1, 7);
	new_instruction->im2 = shift_and_mask(new_instruction, 0b1111, 8);
	new_instruction->im3 = shift_and_mask(new_instruction, 0b111111, 25);
	new_instruction->im4 = shift_and_mask(new_instruction, 0b1, 31);
}

void resolve_u_type(instruction * new_instruction) {
	new_instruction->rd = shift_and_mask(new_instruction, 0b11111, 7);
	new_instruction->im1 = shift_and_mask(new_instruction, 0b11111111111111111111, 12);
}

void resolve_j_type(instruction * new_instruction) {
	new_instruction->rd = shift_and_mask(new_instruction, 0b11111, 7);
	
	new_instruction -> im4 = shift_and_mask(new_instruction, 0b1, 31);
	new_instruction -> im3 = shift_and_mask(new_instruction, 0b1111111111, 21);
	new_instruction -> im2 = shift_and_mask(new_instruction, 0b1, 20);
	new_instruction -> im1 = shift_and_mask(new_instruction, 0b1111111, 12);

}

void print_r_type(instruction * source) {
	printf("FMT: R, Opcode: %x, Funct3: %x, Funct7: %x, RS1: %d, RS2: %d, RD: %d\n", source->opcode, source->funct3, source->funct7, source->rs1, source->rs2, source->rd);
}

void print_i_type(instruction * source) {
	printf("FMT: I, Opcode: %x, Funct3: %x, Imediate: %x, RS1: %d, RD: %d\n", source->opcode, source->funct3, source->im1, source -> rs1, source -> rd);
}

void print_s_type(instruction * source) {
	printf("FMT: S, Opcode: %x, Funct3: %x, Imediate 1: %x, Imediate 2: %x, RS1 %d, RS2 %d\n", source->opcode, source->funct3, source->im1, source->im2, source->rs1, source->rs2);
}

void print_b_type(instruction * source) {
	printf("FMT: B, Opcode: %x, Funct3: %x, Imediate 1: %x, Imediate 2: %x, RS1 %d, RS2 %d\n", source->opcode, source->funct3, source->im1, source->im2, source->rs1, source->rs2);
}

void print_u_type(instruction * source) {
	printf("FMT: U, Opcode: %x, Imediate 1: %x, RD %d\n", source->opcode, source->im1, source->rd);
}

void print_j_type(instruction * source) {
	printf("FMT: J, Opcode: %x, Imediate 1: %x, Imediate 2: %x, Imediate 3: %x, Imediate 4: %x, RD %d\n", source->opcode, source->im1, source->im2, source->im3, source->im4, source->rd);
}

void print_instruction(instruction * printable) {
	switch (printable->type) {
		case R: print_r_type(printable); break;
		case I: print_i_type(printable); break;
		case S: print_s_type(printable); break;
		case B: print_b_type(printable); break; //b is for bad cause this one is the uggliest
		case U: print_u_type(printable); break;
		case J: print_j_type(printable); break;
		default: printf("Type undefined.\n"); exit(0);
	}	
}

void disasemble_r_type(instruction * source) {
	if (source->opcode == 0b0110011) {
		switch(source->funct3) {
			case 0x0:
				if (source->funct7 == 0x00) { printf("add r%d, r%d, r%d\n", source->rd, source->rs1, source->rs2);
				} else if (source->funct7 == 0x20) { printf("sub r%d r%d r%d\n", source->rd, source->rs1, source->rs2);
				} else {printf("Unsuported funct 7: %x", source->funct7); exit(0);}
				break;
			case 0x4:
				if (source->funct7 == 0x00) { printf("xor r%d, r%d, r%d\n", source->rd, source->rs1, source->rs2);
				} else {printf("Unsuported funct 7: %x", source->funct7); exit(0);}
				break;
			case 0x6:
				if (source->funct7 == 0x00) { printf("or r%d, r%d, r%d\n", source->rd, source->rs1, source->rs2);
				} else {printf("Unsuported funct 7: %x", source->funct7); exit(0);}
				break;
			case 0x7:
				if (source->funct7 == 0x00) { printf("and r%d, r%d, r%d\n", source->rd, source->rs1, source->rs2);
				} else {printf("Unsuported funct 7: %x", source->funct7); exit(0);}
				break;
			case 0x1:
				if (source->funct7 == 0x00) { printf("sll r%d, r%d, r%d\n", source->rd, source->rs1, source->rs2);
				} else {printf("Unsuported funct 7: %x", source->funct7); exit(0);}
				break;
			case 0x5:
				if (source->funct7 == 0x00) { printf("srl r%d, r%d, r%d\n", source->rd, source->rs1, source->rs2);
				} else if (source->funct7 == 0x20) { printf("sra r%d, r%d, r%d\n", source->rd, source->rs1, source->rs2);
				} else {printf("Unsuported funct 7: %x", source->funct7); exit(0);}
				break;
			case 0x2:
				if (source->funct7 == 0x00) { printf("slt r%d, r%d, r%d\n", source->rd, source->rs1, source->rs2);
				} else {printf("Unsuported funct 7: %x", source->funct7); exit(0);}
				break;
			case 0x3:
				if (source->funct7 == 0x00) { printf("sltu r%d, r%d, r%d\n", source->rd, source->rs1, source->rs2);
				} else {printf("Unsuported funct 7: %x", source->funct7); exit(0);}
				break;
			default: printf("Unsuported func 3 value in r type disasembler: %x\n", source->funct3); exit(0);
		}
	}
}

void disasemble_i_type(instruction * source) {
	if (source->opcode == 0b0010011) {
		switch(source->funct3) {
			case 0x0: printf("addi r%d, r%d, %d\n", source->rd, source->rs1, source->im1); break;
			case 0x4: printf("xori r%d, r%d, %d\n", source->rd, source->rs1, source->im1); break;
			case 0x6: printf("orii r%d, r%d, %d\n", source->rd, source->rs1, source->im1); break;
			case 0x7: printf("andi r%d, r%d, %d\n", source->rd, source->rs1, source->im1); break;
			case 0x1:
				if (((source->im1 >> 5) & 0b1111111) == 0x00) {
					printf("slli r%d, r%d, %d\n", source->rd, source->rs1, ((source->im1 >> 0) & 0b11111)); break;
				} else {printf("Unsuported imediate in i type.\n"); exit(0);}
			case 0x5:
				if (((source->im1 >> 5) & 0b1111111) == 0x00) {
					printf("slli r%d, r%d, %d\n", source->rd, source->rs1, ((source->im1 >> 0) & 0b11111)); break;
				} else if (((source->im1 >> 5) & 0b1111111) == 0x20) {
					printf("sili r%d, r%d, %d\n", source->rd, source->rs1, ((source->im1 >> 0) & 0b11111)); break;
				} else {printf("Unsuported imediate in i type.\n"); exit(0);}
			case 0x2: printf("slti r%d, r%d, %d\n", source->rd, source->rs1, source->im1); break;
			case 0x3: printf("sltui r%d, r%d, %d\n", source->rd, source->rs1, source->im1); break;
			default: printf("Unsuported func 3 value in i type disasembler: %x\n", source->funct3); exit(0);
		}
	} else if (source->opcode == 0b0000011) {
		switch(source->funct3) {
			case 0x0: printf("lb r%d, %x(r%d)\n", source->rd, source->im1, source->rs1); break;
			case 0x1: printf("lh r%d, %x(r%d)\n", source->rd, source->im1, source->rs1); break;
			case 0x2: printf("lw r%d, %x(r%d)\n", source->rd, source->im1, source->rs1); break;
			case 0x4: printf("lbu r%d, %x(r%d)\n", source->rd, source->im1, source->rs1); break;
			case 0x5: printf("lhu r%d, %x(r%d)\n", source->rd, source->im1, source->rs1); break;
			default: printf("Unsuported func 3 value in i type disasembler: %x\n", source->funct3); exit(0);
		}
	} else if (source->opcode == 0b1110011) {
		if (source->funct3 == 0x0) {
			if (source->im1 == 0x0) {
				printf("ecall\n");
			} else if (source->im1 == 0x1) {
				printf("ebreak\n");
			} else {
			printf("Unsuported imediate value in i type disasembler: %x\n", source->funct3);
			exit(0);
			}
		} else {
			printf("Unsuported func 3 value in i type disasembler: %x\n", source->funct3);
			exit(0);
		}
	} else if (source->opcode == 0b1100111) {
		if (source->funct3 == 0x0) {
			printf("jalr r%d, %x(r%d)\n", source->rd, source->im1, source->rs1);
		} else {
			printf("Unsuported funct3 in i type disasembler: %x\n", source->funct3);
		}
	} else {
		printf("Unsuported opcode in i type disasembler: %x\n", source->funct3);
	}
}

void disasemble_s_type(instruction * source) {
	switch(source->funct3) {
		case 0x0: printf("sb r%d, %d(r%d)", source->rs2, (source->im1 + (source->im2 << 5)), source ->rs1); break;
		case 0x1: printf("sh r%d, %d(r%d)", source->rs2, (source->im1 + (source->im2 << 5)), source ->rs1); break;
		case 0x2: printf("sw r%d, %d(r%d)", source->rs2, (source->im1 + (source->im2 << 5)), source ->rs1); break;
		default: printf("Invalid S type instruction\n"); exit(0);
	}
}

void disasemble_b_type(instruction * source) {
	int imediate = (source->im1 << 11) + (source->im2 << 1) + (source->im3 << 5) + (source->im4 << 12);
	switch(source->funct3) {
		case 0x0: printf("beq r%d, r%d, %d", source -> rs1, source -> rs2, imediate); break;
		case 0x1: printf("bne r%d, r%d, %d", source -> rs1, source -> rs2, imediate); break;
		case 0x4: printf("blt r%d, r%d, %d", source -> rs1, source -> rs2, imediate); break;
		case 0x5: printf("bge r%d, r%d, %d", source -> rs1, source -> rs2, imediate); break;
		case 0x6: printf("bltu r%d, r%d, %d", source -> rs1, source -> rs2, imediate); break;
		case 0x7: printf("bgeu r%d, r%d, %d", source -> rs1, source -> rs2, imediate); break;
		default: printf("Invalid B type instruction\n"); exit(0);
	}
}

void disasemble_u_type(instruction * source) {
	if (source->opcode == 0b0110111) {
		printf("lui r%d, %d", source -> rd, source->im1);
	} else if (source->opcode == 0b0010111) {
		printf("auipc r%d, %d", source -> rd, source->im1);
	} else { 
		printf("Invalid U type instruction.\n");
		exit(0);
	}
}

void disasemble_j_type(instruction * source) {
	if (source->opcode == 0b1101111) {
		int imediate = (source->im1 << 12) + (source->im2 << 11) + (source->im3 << 1) + (source->im4 << 20); 
		printf("jal r%d, %d\n", source -> rd, imediate);
	} else { 
		printf("Invalid U type instruction.\n");
		exit(0);
	}
}

void disasemble_instruction(instruction * printable) {
	switch (printable->type) {
		case R: disasemble_r_type(printable); break;
		case I: disasemble_i_type(printable); break;
		case S: disasemble_s_type(printable); break;
		case B: disasemble_b_type(printable); break; //b is for bad cause this one is the uggliest
		case U: disasemble_u_type(printable); break;
		case J: disasemble_j_type(printable); break;
		default: printf("Type undefined.\n"); exit(0);
	}	
}

//so decoding was really ok.
//only concern is that imediates continue to be used and accessed in weird ass ways..
//would be nice to store them in a fancy way 
instruction * decode_instruction(int32_t input) {
	instruction * new_instruction = calloc(1, sizeof(instruction));
	new_instruction -> raw = input;
	new_instruction->opcode = shift_and_mask(new_instruction, 0b1111111, 0);

	switch(new_instruction->opcode) {
		case 0b0110011: new_instruction->type = R; break;
		case 0b0010011: new_instruction->type = I; break;
		case 0b0000011: new_instruction->type = I; break;
		case 0b0100011: new_instruction->type = S; break;
		case 0b1100011: new_instruction->type = B; break;
		case 0b1101111: new_instruction->type = J; break;
		case 0b1100111: new_instruction->type = I; break;
		case 0b0110111: new_instruction->type = U; break;
		case 0b0010111: new_instruction->type = U; break;
		case 0b1110011: new_instruction->type = I; break;
		default: printf("Instruction undefined.\n"); exit(0);
	}

	switch(new_instruction->type) {
		case R: resolve_r_type(new_instruction); break;
		case I: resolve_i_type(new_instruction); break;
		case S: resolve_s_type(new_instruction); break;
		case B: resolve_b_type(new_instruction); break; //b is for bad cause this one is the uggliest
		case U: resolve_u_type(new_instruction); break;
		case J: resolve_j_type(new_instruction); break;
		default: printf("Type undefined.\n"); exit(0);
	}

	return new_instruction;
}

typedef enum {ADD, SUB, ADDI, SUBI} riscv_instruction;

void riscv_instruction_print(riscv_instruction i) {
	switch(i) {
		case ADD:
			printf("add ");
			break;
		case SUB:
			printf("sub ");
			break;
		case ADDI:
			printf("addi ");
			break;
		case SUBI:
			printf("subi ");
			break;
		default:
			printf("Unrecognized instruction\n");
			exit(0);
	}
}

typedef struct tokenized_instruction {
	instruction_type major_type;
	riscv_instruction instruction;	
	int reg_one;	
	int reg_two;
	int imediate;
	int reg_dest;	
} tokenized_instruction;

int32_t encode_instruction(tokenized_instruction instruction) {
	return 1;
}

int is_valid(char input) {
	char invalids [7] = {' ', ',', '\n', '\t', '(', ')', '\0'};
	for (int i = 0; i < 7; i++) {
		if (invalids[i] == input) {
			return 0;
		}
	}
	return 1;
}

int get_token_length(char * token_start) {
	int i = 0;
	while(is_valid(token_start[i])) {
		i++;
	}
	return i;
}

char * eat_space(char * token_start) {
	while(!is_valid(token_start[0])) {
		token_start += 1;
	}
	return token_start;
}

char * eat_token(char * token_start) {
	while(is_valid(token_start[0])) {
		token_start += 1;
	}
	return token_start;
}

int check_end(char * string) {
	if (string[0] == '\0') {
		return 1;
	}
	return 0;
}

int s_strcmp(char * instruction_string, char * compare) {
	int instruction_len = strlen(instruction_string);
	int compare_len = strlen(compare);
	if (compare_len < instruction_len) {
		return 0;
	}
	for (int i = 0; i < instruction_len; i ++) {
		if (instruction_string[i] != compare[i]) {
			return 0;
		}
	}
	if (is_valid(compare[instruction_len])) {
		return 0;
	}
	return 1;
}

riscv_instruction get_riscv_from_char(char * target_string) {
	if (s_strcmp("add", target_string)) {
		return ADD;
	} else if (s_strcmp("addi", target_string)) {
		return ADDI;
	} else if (s_strcmp("sub", target_string)) {
		return SUB;
	} else if (s_strcmp("subi", target_string)) {
		return SUBI;
	} else {
		printf("Failed to identify instruction in: %s\n", target_string);
		exit(0);
	}
}
	
void check_valid(char * input_line) {
	if ((!is_valid(*input_line)) || check_end(input_line)) {
		printf("Invalid instruction at: %s.\n", input_line);
		exit(0);
	}
}

instruction_type instruction_type_from_riscv(riscv_instruction input) {
	switch(input) {
		case ADD:
		case SUB:
			return R;
		case ADDI:
		case SUBI:
			return I;
		default:
			printf("Instruction type not recognized.\n");
			exit(0);
	}
} 

void check_reg(char * input_line) {
	if (*input_line != 'r') {
		printf("Input does not seem to be a register: %s\n", input_line);
		exit(0);
	}
}

void check_im(char * input_line) {
	if (*input_line > '9' && *input_line < '0') {
		printf("", input_line);
		exit(0);
	}
}

int get_reg_from_char(char * input) {
	return atoi(input + 1);
}

int get_im_from_char(char * input) {
	return atoi(input);
}

tokenized_instruction get_tokenized_instruction(char * input_line) {
	tokenized_instruction new_instruction;
	check_valid(input_line);
	new_instruction.instruction = get_riscv_from_char(input_line);
	input_line = eat_token(input_line);
	input_line = eat_space(input_line);
	check_valid(input_line);
	
	new_instruction.major_type = instruction_type_from_riscv(new_instruction.instruction); 
	
	if (new_instruction.major_type == R) {
		check_reg(input_line);
		new_instruction.reg_dest = get_reg_from_char(input_line);
		input_line = eat_token(input_line);
		input_line = eat_space(input_line);
		check_valid(input_line);
		check_reg(input_line);
		new_instruction.reg_one = get_reg_from_char(input_line);
		input_line = eat_token(input_line);
		input_line = eat_space(input_line);
		check_valid(input_line);
		check_reg(input_line);
		new_instruction.reg_two = get_reg_from_char(input_line);
	} else if (new_instruction.major_type == I) {
		check_reg(input_line);
		new_instruction.reg_dest = get_reg_from_char(input_line);
		input_line = eat_token(input_line);
		input_line = eat_space(input_line);
		check_valid(input_line);
		check_reg(input_line);
		new_instruction.reg_one = get_reg_from_char(input_line);
		input_line = eat_token(input_line);
		input_line = eat_space(input_line);
		check_valid(input_line);
		check_im(input_line);
		new_instruction.imediate= get_im_from_char(input_line);
	}
		
	return new_instruction;
}

void print_tokenized_instruciton(tokenized_instruction input) {
	riscv_instruction_print(input.instruction);

	if (input.major_type == R) {
		printf("r%d, r%d, r%d\n", input.reg_dest, input.reg_one, input.reg_two);	
	} else if (input.major_type == I) {
		printf("r%d, r%d, %d\n", input.reg_dest, input.reg_one, input.imediate);	
	} else {
		printf("Invalid major type in print function.\n");
		exit(0);
	}
} 

int main() {
	tokenized_instruction instruction = get_tokenized_instruction("add r1, r2, r3");
	print_tokenized_instruciton(instruction);	
	instruction = get_tokenized_instruction("addi r1, r2, 10");
	print_tokenized_instruciton(instruction);	
	instruction = get_tokenized_instruction("sub r1, r2, r3");
	print_tokenized_instruciton(instruction);	
	instruction = get_tokenized_instruction("subi r1, r2, 10");
	print_tokenized_instruciton(instruction);	
	//instruction * new_instruction = decode_instruction(0b1000000000100001000000010110011);
	//print_instruction(new_instruction);
	//disasemble_instruction(new_instruction);
	//free(new_instruction);
}
