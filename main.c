#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

int32_t powi(int base, int top) {
	assert(base > 0);
	assert(top >= 2);	
	long int count = base;
	for (int i = 1; i < top; i++) {
		count *= base;
	}
	return count;
}

int32_t signed_max(int bits) {
	int32_t max = powi(2,(bits-1)) - 1;
	return max;
}

int32_t signed_min(int bits) {
	int32_t min = -powi(2, bits-1); 
	return min;
}

int32_t enforce_bit_rep(int32_t input, int n) {
	int32_t output = input & (powi(2,n) -1); 
	return output;
}

int32_t reduce_to_n(int32_t input, int n) {
	int32_t output = 0;
	int32_t max = signed_max(n);	
	int32_t min = signed_min(n);	

	assert(input <= max);
	assert(input >= min);

	output = input & (powi(2,n) -1); 

	return output;
}

int32_t extend_from_n(int32_t input, int n) {
	int32_t output = input;

	if (((output >> (n - 1)) & 1) == 1)  { //make this dependent on n
		//negative case
		output = output | ((-1) & (~(powi(2,n)-1)));
	}

	return output;
}

//binary print from geeksforgeeks
void print_binary(int32_t n)
{
    int32_t i;
    printf("0");
    for (i = 1 << 31; i > 0; i = i / 2) {
        if ((n & i) != 0) {
            printf("1");
        }
        else {
            printf("0");
        }
    }
}
	
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
	new_instruction->im1= extend_from_n(shift_and_mask(new_instruction, 0b111111111111, 20), 12);
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
	new_instruction -> im1 = shift_and_mask(new_instruction, 0b11111111, 12);

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

void disasemble_r_type(instruction * source, char * buff) {
	if (source->opcode == 0b0110011) {
		switch(source->funct3) {
			case 0x0:
				if (source->funct7 == 0x00) { sprintf(buff,"add r%d, r%d, r%d\n", source->rd, source->rs1, source->rs2);
				} else if (source->funct7 == 0x20) { sprintf(buff,"sub r%d, r%d, r%d\n", source->rd, source->rs1, source->rs2);
				} else {printf("Unsuported funct 7: %x", source->funct7); exit(0);}
				break;
			case 0x4:
				if (source->funct7 == 0x00) { sprintf(buff,"xor r%d, r%d, r%d\n", source->rd, source->rs1, source->rs2);
				} else {printf("Unsuported funct 7: %x", source->funct7); exit(0);}
				break;
			case 0x6:
				if (source->funct7 == 0x00) { sprintf(buff,"or r%d, r%d, r%d\n", source->rd, source->rs1, source->rs2);
				} else {printf("Unsuported funct 7: %x", source->funct7); exit(0);}
				break;
			case 0x7:
				if (source->funct7 == 0x00) { sprintf(buff,"and r%d, r%d, r%d\n", source->rd, source->rs1, source->rs2);
				} else {printf("Unsuported funct 7: %x", source->funct7); exit(0);}
				break;
			case 0x1:
				if (source->funct7 == 0x00) { sprintf(buff,"sll r%d, r%d, r%d\n", source->rd, source->rs1, source->rs2);
				} else {printf("Unsuported funct 7: %x", source->funct7); exit(0);}
				break;
			case 0x5:
				if (source->funct7 == 0x00) { sprintf(buff,"srl r%d, r%d, r%d\n", source->rd, source->rs1, source->rs2);
				} else if (source->funct7 == 0x20) { sprintf(buff,"sra r%d, r%d, r%d\n", source->rd, source->rs1, source->rs2);
				} else {printf("Unsuported funct 7: %x", source->funct7); exit(0);}
				break;
			case 0x2:
				if (source->funct7 == 0x00) { sprintf(buff,"slt r%d, r%d, r%d\n", source->rd, source->rs1, source->rs2);
				} else {printf("Unsuported funct 7: %x", source->funct7); exit(0);}
				break;
			case 0x3:
				if (source->funct7 == 0x00) { sprintf(buff,"sltu r%d, r%d, r%d\n", source->rd, source->rs1, source->rs2);
				} else {printf("Unsuported funct 7: %x", source->funct7); exit(0);}
				break;
			default: printf("Unsuported func 3 value in r type disasembler: %x\n", source->funct3); exit(0);
		}
	}
}

void disasemble_i_type(instruction * source, char * buff) {
	if (source->opcode == 0b0010011) {
		switch(source->funct3) {
			case 0x0: sprintf(buff, "addi r%d, r%d, %d\n", source->rd, source->rs1, source->im1); break;
			case 0x4: sprintf(buff, "xori r%d, r%d, %d\n", source->rd, source->rs1, source->im1); break;
			case 0x6: sprintf(buff, "ori r%d, r%d, %d\n", source->rd, source->rs1, source->im1); break;
			case 0x7: sprintf(buff, "andi r%d, r%d, %d\n", source->rd, source->rs1, source->im1); break;
			case 0x1:
				if (((source->im1 >> 5) & 0b1111111) == 0x00) {
					sprintf(buff,"slli r%d, r%d, %d\n", source->rd, source->rs1, ((source->im1 >> 0) & 0b11111)); break;
				} else {printf("Unsuported imediate in i type.\n"); exit(0);}
			case 0x5:
				if (((source->im1 >> 5) & 0b1111111) == 0x00) {
					sprintf(buff,"srli r%d, r%d, %d\n", source->rd, source->rs1, ((source->im1 >> 0) & 0b11111)); break;
				} else if (((source->im1 >> 5) & 0b1111111) == 0x20) {
					sprintf(buff,"srai r%d, r%d, %d\n", source->rd, source->rs1, ((source->im1 >> 0) & 0b11111)); break;
				} else {printf("Unsuported imediate in special i type.\n"); exit(0);}
			case 0x2: sprintf(buff,"slti r%d, r%d, %d\n", source->rd, source->rs1, source->im1); break;
			case 0x3: sprintf(buff,"sltiu r%d, r%d, %d\n", source->rd, source->rs1, source->im1); break;
			default: printf("Unsuported func 3 value in i type disasembler: %x\n", source->funct3); exit(0);
		}
	} else if (source->opcode == 0b0000011) {
		switch(source->funct3) {
			case 0x0: sprintf(buff, "lb r%d, %d(r%d)\n", source->rd, source->im1, source->rs1); break;
			case 0x1: sprintf(buff, "lh r%d, %d(r%d)\n", source->rd, source->im1, source->rs1); break;
			case 0x2: sprintf(buff, "lw r%d, %d(r%d)\n", source->rd, source->im1, source->rs1); break;
			case 0x4: sprintf(buff, "lbu r%d, %d(r%d)\n", source->rd, source->im1, source->rs1); break;
			case 0x5: sprintf(buff, "lhu r%d, %d(r%d)\n", source->rd, source->im1, source->rs1); break;
			default: printf("Unsuported func 3 value in i type disasembler: %x\n", source->funct3); exit(0);
		}
	} else if (source->opcode == 0b1110011) {
		if (source->funct3 == 0x0) {
			if (source->im1 == 0x0) {
				sprintf(buff,"ecall\n");
			} else if (source->im1 == 0x1) {
				sprintf(buff,"ebreak\n");
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
			sprintf(buff, "jalr r%d, r%d, %d\n", source->rd, source->rs1, source->im1);
		} else {
			printf("Unsuported funct3 in i type disasembler: %x\n", source->funct3);
		}
	} else {
		printf("Unsuported opcode in i type disasembler: %x\n", source->funct3);
	}
}

void disasemble_s_type(instruction * source, char * buff) {
	int32_t im = extend_from_n((source->im1 + (source->im2 << 5)),12);
	switch(source->funct3) {
		case 0x0: sprintf(buff, "sb r%d, %d(r%d)\n", source->rs2, im, source ->rs1); break;
		case 0x1: sprintf(buff, "sh r%d, %d(r%d)\n", source->rs2, im, source ->rs1); break;
		case 0x2: sprintf(buff, "sw r%d, %d(r%d)\n", source->rs2, im, source ->rs1); break;
		default: printf("Invalid S type instruction\n"); exit(0);
	}
}

void disasemble_b_type(instruction * source, char * buff) {
	int32_t raw_im = (source->im1 << 11) + (source->im2 << 1) + (source->im3 << 5) + (source->im4 << 12);
	int32_t imediate = extend_from_n(raw_im, 13);
	switch(source->funct3) {
		case 0x0: sprintf(buff, "beq r%d, r%d, %d\n", source -> rs1, source -> rs2, imediate); break;
		case 0x1: sprintf(buff, "bne r%d, r%d, %d\n", source -> rs1, source -> rs2, imediate); break;
		case 0x4: sprintf(buff, "blt r%d, r%d, %d\n", source -> rs1, source -> rs2, imediate); break;
		case 0x5: sprintf(buff, "bge r%d, r%d, %d\n", source -> rs1, source -> rs2, imediate); break;
		case 0x6: sprintf(buff, "bltu r%d, r%d, %d\n", source -> rs1, source -> rs2, imediate); break;
		case 0x7: sprintf(buff, "bgeu r%d, r%d, %d\n", source -> rs1, source -> rs2, imediate); break;
		default: printf("Invalid B type instruction\n"); exit(0);
	}
}

void disasemble_u_type(instruction * source, char * buff) {
	if (source->opcode == 0b0110111) {
		sprintf(buff,"lui r%d, %d\n", source -> rd, source->im1);
	} else if (source->opcode == 0b0010111) {
		sprintf(buff,"auipc r%d, %d\n", source -> rd, source->im1);
	} else { 
		printf("Invalid U type instruction.\n");
		exit(0);
	}
}

void disasemble_j_type(instruction * source, char * buff) {
	if (source->opcode == 0b1101111) {
		int imediate = extend_from_n((source->im1 << 12) + (source->im2 << 11) + (source->im3 << 1) + (source->im4 << 20), 20); 
		sprintf(buff, "jal r%d, %d\n", source -> rd, imediate);
	} else { 
		printf("Invalid U type instruction.\n");
		exit(0);
	}
}

void disasemble_instruction(instruction * printable, char * buff) {
	switch (printable->type) {
		case R: disasemble_r_type(printable, buff); break;
		case I: disasemble_i_type(printable, buff); break;
		case S: disasemble_s_type(printable, buff); break;
		case B: disasemble_b_type(printable, buff); break; //b is for bad cause this one is the uggliest
		case U: disasemble_u_type(printable, buff); break;
		case J: disasemble_j_type(printable, buff); break;
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

typedef enum {
	ADD, SUB, XOR, OR, AND, SLL, SRL, SRA, SLT, SLTU, 
	ADDI, XORI, ORI, ANDI, SLLI, SRLI, SRAI, SLTI, SLTIU,
	LB, LH, LW, LBU, LHU, 
	SB, SH, SW, 
	BEQ, BNE, BLT, BGE, BLTU, BGEU,
	JAL, JALR, LUI, AUIPC, ECALL, EBREAK
} riscv_instruction;

void riscv_instruction_print(riscv_instruction i) {
	switch(i) {
		case ADD: printf("add "); break;
		case SUB: printf("sub "); break;
		case XOR: printf("xor "); break;
		case OR: printf("or "); break;
		case AND: printf("and "); break;
		case SLL: printf("sll "); break;
		case SRL: printf("srl "); break;
		case SRA: printf("sra "); break;
		case SLT: printf("slt "); break;
		case SLTU: printf("sltu "); break;
		//imediates
		case ADDI: printf("addi "); break;
		case XORI: printf("xori "); break;
		case ORI: printf("ori "); break;
		case ANDI: printf("andi "); break;
		case SLLI: printf("slli "); break;
		case SRLI: printf("srli "); break;
		case SRAI: printf("srai "); break;
		case SLTI: printf("slti "); break;
		case SLTIU: printf("sltiu "); break;
		//loads
		case LB: printf("lb "); break;
		case LH: printf("lh "); break;
		case LW: printf("lw "); break;
		case LBU: printf("lbu "); break;
		case LHU: printf("lhu "); break;
		//s types
		case SB: printf("sb "); break;
		case SH: printf("sh "); break;
		case SW: printf("sw "); break;
		//b types
		case BEQ: printf("beq "); break;
		case BNE: printf("bne "); break;
		case BLT: printf("blt "); break;
		case BGE: printf("bge "); break;
		case BLTU: printf("bltu "); break;
		case BGEU: printf("bgeu "); break;
		//others..
		case JAL: printf("jal "); break;
		case JALR: printf("jalr "); break;
		case LUI: printf("lui "); break;
		case AUIPC: printf("auipc "); break;
		case ECALL: printf("ecall "); break;
		case EBREAK: printf("ebreak "); break;
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
	} else if (s_strcmp("sub", target_string)) {
		return SUB;
	} else if (s_strcmp("xor", target_string)) {
		return XOR;
	} else if (s_strcmp("or", target_string)) {
		return OR;
	} else if (s_strcmp("and", target_string)) {
		return AND;
	} else if (s_strcmp("sll", target_string)) {
		return SLL;
	} else if (s_strcmp("srl", target_string)) {
		return SRL;
	} else if (s_strcmp("sra", target_string)) {
		return SRA;
	} else if (s_strcmp("slt", target_string)) {
		return SLT;
	} else if (s_strcmp("sltu", target_string)) {
		return SLTU;
	} 
	
	//imediate mode
	else if (s_strcmp("addi", target_string)) {
		return ADDI;
	} else if (s_strcmp("xori", target_string)) {
		return XORI;
	} else if (s_strcmp("ori", target_string)) {
		return ORI;
	} else if (s_strcmp("andi", target_string)) {
		return ANDI;
	} else if (s_strcmp("slli", target_string)) {
		return SLLI;
	} else if (s_strcmp("srli", target_string)) {
		return SRLI;
	} else if (s_strcmp("srai", target_string)) {
		return SRAI;
	} else if (s_strcmp("slti", target_string)) {
		return SLTI;
	} else if (s_strcmp("sltiu", target_string)) {
		return SLTIU;
	} 

	//load bytes
	else if (s_strcmp("lb", target_string)) {
		return LB;
	} else if (s_strcmp("lh", target_string)) {
		return LH;
	} else if (s_strcmp("lw", target_string)) {
		return LW;
	} else if (s_strcmp("lbu", target_string)) {
		return LBU;
	} else if (s_strcmp("lhu", target_string)) {
		return LHU;
	} 

	//stores
	else if (s_strcmp("sb", target_string)) {
		return SB;
	} else if (s_strcmp("sh", target_string)) {
		return SH;
	} else if (s_strcmp("sw", target_string)) {
		return SW;
	} 

	//branches
	else if (s_strcmp("beq", target_string)) {
		return BEQ;
	} else if (s_strcmp("bne", target_string)) {
		return BNE;
	} else if (s_strcmp("blt", target_string)) {
		return BLT;
	} else if (s_strcmp("bge", target_string)) {
		return BGE;
	} else if (s_strcmp("bltu", target_string)) {
		return BLTU;
	} else if (s_strcmp("bgeu", target_string)) {
		return BGEU;
	} 
	
	//other
	else if (s_strcmp("jal", target_string)) {
		return JAL;
	} else if (s_strcmp("jalr", target_string)) {
		return JALR;
	} else if (s_strcmp("lui", target_string)) {
		return LUI;
	} else if (s_strcmp("auipc", target_string)) {
		return AUIPC;
	} else if (s_strcmp("ecall", target_string)) {
		return ECALL ;
	} else if (s_strcmp("ebreak", target_string)) {
		return EBREAK;
	} 

	else {
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
		case XOR:
		case OR:
		case AND:
		case SLL:
		case SRL:
		case SRA:
		case SLT:
		case SLTU:
			return R;
		case ADDI:
		case XORI: 
		case ORI: 
		case ANDI: 
		case SLLI: 
		case SRLI: 
		case SRAI: 
		case SLTI: 
		case SLTIU:
		case LB:
		case LH:
		case LW:
		case LBU:
		case LHU:
			return I;
		case SB:
		case SH:
		case SW:
			return S;
		case BEQ:
		case BNE:
		case BLT:
		case BGE:
		case BLTU:
		case BGEU:
			return B;
		case JAL: 
			return J;
		case JALR: 
			return I;
		case LUI:
		case AUIPC: 
			return U;
		case ECALL:
		case EBREAK: 
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

int is_reg(char * input_line) {
	if (*input_line != 'r') {
		return 0;	
	}
	return 1;
}

void check_im(char * input_line) {
	if (*input_line > '9' && *input_line < '0') {
		printf("Input does not seem to be an imediate value: %s\n", input_line);
		exit(0);
	}
}

int is_im(char * input_line) {
	if (*input_line > '9' && *input_line < '0') {
		return 0;	
	}
	return 1;
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
	} else if ( new_instruction.major_type == B) {
		check_reg(input_line);
		new_instruction.reg_one= get_reg_from_char(input_line);
		input_line = eat_token(input_line);
		input_line = eat_space(input_line);
		check_valid(input_line);
		check_reg(input_line);
		new_instruction.reg_two= get_reg_from_char(input_line);
		input_line = eat_token(input_line);
		input_line = eat_space(input_line);
		check_valid(input_line);
		check_im(input_line);
		new_instruction.imediate= reduce_to_n(get_im_from_char(input_line), 13); 
	} else if (new_instruction.major_type == I) {
		if (is_reg(input_line)) {	
			check_reg(input_line);
			new_instruction.reg_dest = get_reg_from_char(input_line);
			input_line = eat_token(input_line);
			input_line = eat_space(input_line);
			check_valid(input_line);
			if (is_reg(input_line)) {
				new_instruction.reg_one = get_reg_from_char(input_line);
				input_line = eat_token(input_line);
				input_line = eat_space(input_line);
				check_valid(input_line);
				check_im(input_line);
				new_instruction.imediate= reduce_to_n(get_im_from_char(input_line),12);
			} else if (is_im(input_line)) {
				new_instruction.imediate= reduce_to_n(get_im_from_char(input_line),12);
				input_line = eat_token(input_line);
				input_line = eat_space(input_line);
				check_valid(input_line);
				check_reg(input_line);
				new_instruction.reg_one = get_reg_from_char(input_line);
			} else {
				printf("Horrible error\n");
				exit(0);
			}
		} else {
			new_instruction.reg_one = 0;
			if (new_instruction.instruction == ECALL) {
				new_instruction.imediate = 0x0;
			} else {
				new_instruction.imediate = 0x1;
			}
		}
	
	} else if (new_instruction.major_type == S) {
		if (is_reg(input_line)) {	
			check_reg(input_line);
			new_instruction.reg_two= get_reg_from_char(input_line);
			input_line = eat_token(input_line);
			input_line = eat_space(input_line);
			check_valid(input_line);
			check_im(input_line);	
			new_instruction.imediate= reduce_to_n(get_im_from_char(input_line),12);
			input_line = eat_token(input_line);
			input_line = eat_space(input_line);
			check_valid(input_line);
			check_reg(input_line);
			new_instruction.reg_one= get_reg_from_char(input_line);
		} else { 
			printf("Horrible S error\n");
			exit(0);
		}
	} else if (new_instruction.major_type == J) {
			check_reg(input_line);
			new_instruction.reg_dest = get_reg_from_char(input_line);
			input_line = eat_token(input_line);
			input_line = eat_space(input_line);
			check_valid(input_line);
			new_instruction.imediate= reduce_to_n(get_im_from_char(input_line), 20);
	} else if (new_instruction.major_type == U) {
			check_reg(input_line);
			new_instruction.reg_dest = get_reg_from_char(input_line);
			input_line = eat_token(input_line);
			input_line = eat_space(input_line);
			check_valid(input_line);
			new_instruction.imediate = get_im_from_char(input_line);
	}
	

	return new_instruction;
}

int i_type_one(riscv_instruction input) {
	riscv_instruction ok [9] = {ADDI, XORI, ORI, ANDI, SLLI, SRLI, SRAI, SLTI, SLTIU};
	for (int i = 0; i < 9; i++) {
		if (input == ok[i]) {
			return 1;
		}
	}
	return 0;
}

int i_type_two(riscv_instruction input) {
	riscv_instruction ok [6] = {LB, LH, LW, LBU, LHU, JALR};
	for (int i = 0; i < 6; i++) {
		if (input == ok[i]) {
			return 1;
		}
	}
	return 0;
}

int i_type_three(riscv_instruction input) {
	riscv_instruction ok [2] = {ECALL, EBREAK};
	for (int i = 0; i < 2; i++) {
		if (input == ok[i]) {
			return 1;
		}
	}
	return 0;
}

int32_t add_opcode(int32_t instruction, int32_t oppcode) {
	instruction += oppcode;
	return instruction;
}

int32_t add_rd (int32_t instruction, int32_t rd) {
	instruction += rd << 7;
	return instruction;
}

int32_t add_funct3(int32_t instruction, int32_t funct) {
	instruction += funct << 12;
	return instruction;
}

int32_t add_funct7(int32_t instruction, int32_t funct) {
	instruction += funct << 25;
	return instruction;
}

int32_t add_rs1(int32_t instruction, int32_t rs1) {
	instruction += rs1 << 15;
	return instruction;
}

int32_t add_rs2(int32_t instruction, int32_t rs2) {
	instruction += rs2 << 20;
	return instruction;
}

int is_negative(int32_t number) {
	if ((number >> 31) == 1) {
		return 1;
	}
	return 0;
}

int32_t add_i_imediate(int32_t instruction, int32_t im) {
	instruction += im << 20;
	return instruction;
}

int32_t add_i_imediate_weird(int32_t instruction, int32_t im) {
	instruction += (im << 25);
	return instruction;
}
int32_t get_i_type_funct_3(riscv_instruction type) {
	switch (type) {
		case ADDI: return 0x0;
		case XORI: return 0x4;
		case ORI:return 0x6;
		case ANDI:return 0x7;
		case SLLI:return 0x1;
		case SRLI:return 0x5;
		case SRAI:return 0x5;
		case SLTI:return 0x2;
		case SLTIU:return 0x3;
		case LB: return 0x0;
		case LH: return 0x1;
		case LW:return 0x2;
		case LBU:return 0x4;
		case LHU:return 0x5;
		case JALR:return 0x0;
		case ECALL:return 0x0;
		case EBREAK:return 0x0;
		default: printf("Unknown I type instruction\n"); exit(0);

	}
}

int32_t get_s_type_funct_3(riscv_instruction in) {
	switch(in) {
		case SB: return 0x0;
		case SH: return 0x1;
		case SW: return 0x2;
		default: printf("Unkown s type instruction\n"); exit(0);
	}
}

int32_t get_b_type_funct_3(riscv_instruction in) {
	switch(in) {
		case BEQ: return 0x0;
		case BNE: return 0x1;
		case BLT: return 0x4;
		case BGE: return 0x5;
		case BLTU: return 0x6;
		case BGEU: return 0x7;
		default: printf("Unkown b type instruction\n"); exit(0);
	}
}

uint32_t shift_and_mask_simple(int32_t instruction, int32_t mask, int32_t shift) {
	return ((instruction >> shift) & mask);
}

int32_t add_s_imediate(int32_t result, int32_t imediate) {
	result += (shift_and_mask_simple(imediate, 0b11111, 0)) << 7;
	result += (shift_and_mask_simple(imediate, 0b1111111, 5)) << 25;
	return result;
}

int32_t add_b_imediate(int32_t result, int32_t imediate) {
	result += (shift_and_mask_simple(imediate, 0b1, 11)) << 7;
	result += (shift_and_mask_simple(imediate, 0b1111, 1)) << 8;
	result += (shift_and_mask_simple(imediate, 0b111111, 5)) << 25;
	result += (shift_and_mask_simple(imediate, 0b1, 12)) << 31;
	return result;
}

int32_t add_u_imediate(int32_t result, int32_t imediate) {
	result += (shift_and_mask_simple(imediate, 0b11111111111111111111, 0)) << 12;
	return result;
}

int32_t add_j_imediate(int32_t result, int32_t imediate) {
	result += (shift_and_mask_simple(imediate, 0b11111111, 12)) << 12;
	result += (shift_and_mask_simple(imediate, 0b1, 11)) << 20;
	result += (shift_and_mask_simple(imediate, 0b1111111111, 1)) << 21;
	result += (shift_and_mask_simple(imediate, 0b1, 20)) << 31;
	return result;
}

int32_t encode_instruction(tokenized_instruction instruction) {
	int32_t result = 0b00000000000000000000000000000000;
	switch (instruction.major_type) {
		case R:
			result = add_opcode(result, 0b0110011);
			result = add_rd(result, instruction.reg_dest);
			result = add_rs1(result, instruction.reg_one);
			result = add_rs2(result, instruction.reg_two);
			switch (instruction.instruction) {
				case ADD:
					result = add_funct3(result, 0x0);
					result = add_funct7(result, 0x00);
					break;
				case SUB:
					result = add_funct3(result, 0x0);
					result = add_funct7(result, 0x20);
					break;
				case XOR:
					result = add_funct3(result, 0x4);
					result = add_funct7(result, 0x00);
					break;
				case OR:
					result = add_funct3(result, 0x6);
					result = add_funct7(result, 0x00);
					break;
				case AND:
					result = add_funct3(result, 0x7);
					result = add_funct7(result, 0x00);
					break;
				case SLL:
					result = add_funct3(result, 0x1);
					result = add_funct7(result, 0x00);
					break;
				case SRL:
					result = add_funct3(result, 0x5);
					result = add_funct7(result, 0x00);
					break;
				case SRA:
					result = add_funct3(result, 0x5);
					result = add_funct7(result, 0x20);
					break;
				case SLT:
					result = add_funct3(result, 0x2);
					result = add_funct7(result, 0x00);
					break;
				case SLTU:
					result = add_funct3(result, 0x3);
					result = add_funct7(result, 0x00);
					break;
				default: printf("R type instruction not recognized.\n"); exit(0);
			}
			break;	
		case I:	
			if (i_type_one(instruction.instruction)) {
				result = add_opcode(result, 0b0010011);
				result = add_rd(result, instruction.reg_dest);
				result = add_rs1(result, instruction.reg_one);
				result = add_i_imediate(result, instruction.imediate);
				result = add_funct3(result, get_i_type_funct_3(instruction.instruction));
				if (instruction.instruction == SRAI) {
					result = add_i_imediate_weird(result, 32);
				}
			} else if (i_type_two(instruction.instruction)) {
				if (instruction.instruction == JALR) {
					result = add_opcode(result, 0b1100111);
				} else {
					result = add_opcode(result, 0b0000011);
				}
				result = add_rd(result, instruction.reg_dest);
				result = add_rs1(result, instruction.reg_one);
				result = add_i_imediate(result, instruction.imediate);
				result = add_funct3(result, get_i_type_funct_3(instruction.instruction));
			} else if (i_type_three(instruction.instruction)) {
				result = add_opcode(result, 0b1110011);
				//result = add_rd(result, instruction.reg_dest);
				//result = add_rs1(result, instruction.reg_one);
				result = add_funct3(result, get_i_type_funct_3(instruction.instruction));
				result = add_i_imediate(result, instruction.imediate);
			} else {
				printf("Horrible error turning instruction into int32.\n"); exit(0);
			}
			break;
		case S:	
			result = add_opcode(result, 0b0100011);
			result = add_rs1(result, instruction.reg_one);
			result = add_rs2(result, instruction.reg_two);
			result = add_s_imediate(result, instruction.imediate);
			result = add_funct3(result, get_s_type_funct_3(instruction.instruction));
			break;
		case B:	
			result = add_opcode(result, 0b1100011);
			result = add_rs1(result, instruction.reg_one);
			result = add_rs2(result, instruction.reg_two);
			result = add_b_imediate(result, instruction.imediate);
			result = add_funct3(result, get_b_type_funct_3(instruction.instruction));
			break;
		case U:	
			if (instruction.instruction == LUI) {
				result = add_opcode(result, 0b0110111);
			} else if (instruction.instruction == AUIPC) {
				result = add_opcode(result, 0b0010111);
			} else {
				printf("Bad u type oppcode\n");
				exit(0);
			}
			result = add_rd(result, instruction.reg_dest);
			result = add_u_imediate(result, instruction.imediate); 
			break;
		case J:	
			result = add_opcode(result, 0b1101111);
			result = add_rd(result, instruction.reg_dest);
			result = add_j_imediate(result, instruction.imediate);
			break;
		default: printf("Horrible error turning instruction into int32.\n"); exit(0);
	}	
	return result;
}

void test_asm(char * source) {
	char destination[100];
	tokenized_instruction instr = get_tokenized_instruction(source);
	printf("%d\n", instr.imediate);
	printf("%d\n", extend_from_n(instr.imediate,13));
	int32_t encoded = encode_instruction(instr);
	instruction * decoded = decode_instruction(encoded);
	disasemble_instruction(decoded, destination);
	if (strcmp(source, destination) == 0) {
		printf("Passed test %s",source);
	} else {
		printf("Failed test %s\n",source);
		printf("Input: \n%s.\n",source);
		printf("Output: \n%s.\n",destination);
		printf("%d\n", encoded);
		print_instruction(decoded);
		free(decoded);
		exit(0);
	}
}

void test_r_types() {
	char buff[100];
	char *ops[] = {"add", "sub", "xor", "or", "and", "sll", "srl", "sra", "slt", "sltu", NULL};
	for (int j= 0; j < 1000; j++) {
		int i = 0; 
		while (ops[i] != NULL) {
			sprintf(buff, "%s r%d, r%d, r%d\n", ops[i], rand()%32, rand()%32, rand()%32);
			test_asm(buff);	
			i++;
		}
	}
}

void test_i_types() {
	char buff[100];
	char *ops[] = {"jalr", "addi", "xori", "ori", "andi", "slti", "sltiu", NULL};
	for (int j= 0; j < 1000; j++) {
		int i = 0; 
		while (ops[i] != NULL) {
			sprintf(buff, "%s r%d, r%d, %d\n", ops[i], rand()%32, rand()%32, (rand()%2048 - 1024));
			test_asm(buff);	
			i++;
		}
	}

	char *ops2[] = {"lb", "lh", "lw", "lbu", "lhu", NULL};
	for (int j= 0; j < 1000; j++) {
		int i = 0; 
		while (ops2[i] != NULL) {
			sprintf(buff, "%s r%d, %d(r%d)\n", ops2[i], rand()%32, (rand()%2048 - 1024), rand()%32);
			test_asm(buff);	
			i++;
		}
	}
	sprintf(buff, "ecall\n");
	test_asm(buff);	
	
	sprintf(buff, "ebreak\n");
	test_asm(buff);	

	//seperate because negative shifts are not suported!
	char *ops3[] = {"slli", "srai", "srli",NULL};
	for (int j= 0; j < 1000; j++) {
		int i = 0; 
		while (ops3[i] != NULL) {
			sprintf(buff, "%s r%d, r%d, %d\n", ops3[i], rand()%32, rand()%32, rand()%32);
			test_asm(buff);	
			i++;
		}
	}
}

void test_s_types() {
	char buff[100];
	char *ops[] = {"sb", "sh", "sw", NULL};
	for (int j= 0; j < 1000; j++) {
		int i = 0; 
		while (ops[i] != NULL) {
			sprintf(buff, "%s r%d, %d(r%d)\n", ops[i], rand()%32, (rand()%2024)-1012, rand()%32); //rand()%1024
			test_asm(buff);	
			i++;
		}
	}
}

void test_b_types() {
	char buff[100];
	char *ops[] = {"beq", "bne", "blt", "bge", "bltu", "bgeu", NULL};
	for (int j= 0; j < 1000; j++) {
		int i = 0; 
		while (ops[i] != NULL) {
			sprintf(buff, "%s r%d, r%d, %d\n", ops[i], rand()%32, rand()%32, (rand()%1024)*2 - 1024);
			test_asm(buff);	
			i++;
		}
	}

}

void test_u_and_j_types() {
	//u types: 
	char buff[100];
	char *ops[] = {"lui", "auipc", NULL};
	for (int j= 0; j < 1000; j++) {
		int i = 0; 
		while (ops[i] != NULL) {
			sprintf(buff, "%s r%d, %d\n", ops[i], rand()%32, (rand()%5000) * 2);
			test_asm(buff);	
			i++;
		}
	}

	sprintf(buff, "jal r%d, %d\n", rand()%32, -10);
	test_asm(buff);	
	
}

typedef struct cpu_state {
	uint32_t registers[32];
	uint32_t *memory;
	uint32_t memsize;
	uint32_t pc;
	//instruction pointer and so on to come	
} cpu_state;

cpu_state cpu_create(uint32_t memsize) {
	cpu_state new_state;
	new_state.memory = malloc(sizeof(char) * memsize);
	new_state.memsize = memsize * sizeof(char);
	new_state.pc = 0;
	for (int i = 0; i < 32; i++) {
		new_state.registers[i] = 0;
	}	
	return new_state;
}

void cpu_delete(cpu_state cpu) {
	free(cpu.memory);
}
		
cpu_state execute_r_type(cpu_state state, instruction inst) {
	//problems:
	//1. don't know wha tmsb extends means
	//2. don't konw what zero extends means in the context of the last instruction
	uint32_t op1 = state.registers[inst.rs1];
	uint32_t op2 = state.registers[inst.rs2];
	uint32_t op3;
	if (inst.opcode == 0b0110011) {
		switch(inst.funct3) {
			case 0x0:
				if (inst.funct7 == 0x00) { op3 = op1+op2; } 
				else if (inst.funct7 == 0x20) { op3 = op1 - op2;} 
				else {printf("Unsuported funct 7: %x", inst.funct7); exit(0);}
				break;
			case 0x4:
				if (inst.funct7 == 0x00) { op3 = op1 ^ op2;} 
				else {printf("Unsuported funct 7: %x", inst.funct7); exit(0);}
				break;
			case 0x6:
				if (inst.funct7 == 0x00) { op3 = op1 | op2;} 
				else {printf("Unsuported funct 7: %x", inst.funct7); exit(0);}
				break;
			case 0x7:
				if (inst.funct7 == 0x00) { op3 = op1 & op2;} 
				else {printf("Unsuported funct 7: %x", inst.funct7); exit(0);}
				break;
			case 0x1:
				if (inst.funct7 == 0x00) { op3 = op1 << op2;} 
				else {printf("Unsuported funct 7: %x", inst.funct7); exit(0);}
				break;
			case 0x5:
				if (inst.funct7 == 0x00) { op3 = op1 >> op2;} 
				else if (inst.funct7 == 0x20) { op3 = op1 >> op2;} //msb extends? 
				else {printf("Unsuported funct 7: %x", inst.funct7); exit(0);}
				break;
			case 0x2:
				if (inst.funct7 == 0x00) { op3 = (op1 < op2? 1 : 0);} 
				else {printf("Unsuported funct 7: %x", inst.funct7); exit(0);}
				break;
			case 0x3:
				if (inst.funct7 == 0x00) { op3 = (op1 < op2? 1: 0);} 
				else {printf("Unsuported funct 7: %x", inst.funct7); exit(0);}
				break;
			default: printf("Unsuported func 3 value in r type execution: %x\n", inst.funct3); exit(0);
		}
	} else {
		printf("Unsuported type\n");
		exit(0);
	}
	state.registers[inst.rd] = op3;
	state.pc ++;
	return state;
}

cpu_state execute_i_type(cpu_state state, instruction inst) {
	//problems: unclear what msb extends means
	//unclear what zero extends means in this context
	//unclear what zero extends means for loading
	//ecall and ebreak do not really make much sense yet
	int32_t rs1 = state.registers[inst.rs1];
	int32_t output;
	if (inst.opcode == 0b0010011) {
		switch(inst.funct3) {
			case 0x0: output = state.registers[rs1] + inst.im1; break;
			case 0x4: output = state.registers[rs1] ^ inst.im1; break; 
			case 0x6: output = state.registers[rs1] | inst.im1; break;
			case 0x7: output = state.registers[rs1] & inst.im1; break;
			case 0x1:
				if (((inst.im1 >> 5) & 0b1111111) == 0x00) {
					output = state.registers[rs1] << inst.im1; break;
				} else {printf("Unsuported imediate in i type.\n"); exit(0);}
			case 0x5:
				if (((inst.im1 >> 5) & 0b1111111) == 0x00) {
					output = state.registers[rs1] >> inst.im1; break;
				} else if (((inst.im1 >> 5) & 0b1111111) == 0x20) {
					output = state.registers[rs1] >> inst.im1; break;
				} else {printf("Unsuported imediate in special i type.\n"); exit(0);}
			case 0x2: output = (rs1 < inst.im1) ? 1 : 0;
			case 0x3: output = (rs1 < inst.im1) ? 1 : 0;
			default: printf("Unsuported func 3 value in i type disasembler: %x\n", inst.funct3); exit(0);
		}
		state.pc ++;
		state.registers[inst.rd] = output;
		return state;
	} else if (inst.opcode == 0b0000011) {
		int32_t data;
		switch(inst.funct3) {
			case 0x0: data = (uint8_t)state.memory[state.registers[inst.rs1] + inst.im1]; break;
			case 0x1: data = (uint16_t)state.memory[state.registers[inst.rs1] + inst.im1]; break;
			case 0x2: data = (uint32_t)state.memory[state.registers[inst.rs1] + inst.im1]; break;
			case 0x4: data = (uint8_t)state.memory[state.registers[inst.rs1] + inst.im1]; break;
			case 0x5: data = (uint16_t)state.memory[state.registers[inst.rs1] + inst.im1]; break;
			default: printf("Unsuported func 3 value in i type disasembler: %x\n", inst.funct3); exit(0);
		}
		state.pc ++;
		state.registers[inst.rd] = data;
		return state;
	} else if (inst.opcode == 0b1110011) {
		if (inst.funct3 == 0x0) {
			if (inst.im1 == 0x0) {
				state.pc = 0;
				return state;
			} else if (inst.im1 == 0x1) {
				state.pc = 0;
				return state;
			} else {
			printf("Unsuported imediate value in i type disasembler: %x\n", inst.funct3);
			exit(0);
			}
		} else {
			printf("Unsuported func 3 value in i type disasembler: %x\n", inst.funct3);
			exit(0);
		}
	} else if (inst.opcode == 0b1100111) {
		if (inst.funct3 == 0x0) {
			//sprintf(buff, "jalr r%d, r%d, %d\n", inst.rd, inst.rs1, inst.im1);
			state.registers[inst.rd] = state.pc+4;		
			state.pc = state.registers[inst.rs1] + inst.im1;
			return state;
		} else {
			printf("Unsuported funct3 in i type disasembler: %x\n", inst.funct3);
			exit(0);
		}
	} else {
		printf("Unsuported opcode in i type disasembler: %x\n", inst.funct3);
		exit(0);
	}
}

cpu_state execute_s_type(cpu_state state, instruction inst){
	cpu_state result = state;
	int32_t im = extend_from_n((inst.im1 + (inst.im2 << 5)),12);
	int32_t op1 = state.registers[inst.rs1];
	int32_t op2 = state.registers[inst.rs2];
	uint32_t out;
	 	
	switch(inst.funct3) {
		case 0x0: out = (uint8_t)op2; uint8_t * outptr = (uint8_t*)&result.memory[op1 + im]; *outptr = (uint8_t)out; break;
		case 0x1: out = (uint16_t)op2; uint16_t* outptr16 = (uint16_t*)&result.memory[op1 + im]; *outptr16 = (uint16_t)out; break;
		case 0x2: out = (uint32_t)op2; uint32_t* outptr32 = (uint32_t*)&result.memory[op1 + im]; *outptr32 = (uint32_t)out; break;
		default: printf("Invalid S type instruction execution\n"); exit(0);
	}

	result.memory[op1 + im] = out;
	result.pc ++;
	return result;	
}

cpu_state execute_b_type(cpu_state state, instruction inst){
	//problems: last two instructions zero extend. unclear what that means in this context
	cpu_state result = state;	
	int32_t raw_im = (inst.im1 << 11) + (inst.im2 << 1) + (inst.im3 << 5) + (inst.im4 << 12);
	int32_t imediate = extend_from_n(raw_im, 13);
	int32_t op1 = state.registers[inst.rs1];
	int32_t op2 = state.registers[inst.rs2];

	switch(inst.funct3) {
		case 0x0: if (op1 == op2) {result.pc += imediate;} break;
		case 0x1: if (op1 != op2) {result.pc += imediate;} break;
		case 0x4: if (op1 < op2) {result.pc += imediate;} break;
		case 0x5: if (op1 >= op2) {result.pc += imediate;} break;
		case 0x6: if (op1 < op2) {result.pc += imediate;} break;  //unclear what zero extends means here
		case 0x7: if (op1 >= op2) {result.pc += imediate;} break; //unclear what zero extends means here
		default: printf("Invalid B type instruction\n"); exit(0);
	}

	return result;
}


cpu_state execute_u_type(cpu_state state, instruction inst){
	cpu_state result = state;	
	int32_t out; 

	if (inst.opcode == 0b0110111) {
		out = inst.im1 << 12;
	} else if (inst.opcode == 0b0010111) {
		out = state.pc + (inst.im1 << 12);
	} else { 
		printf("Invalid U type instruction.\n");
		exit(0);
	}

	result.registers[inst.rd] = out;
	result.pc ++;
	return result; 
}

cpu_state execute_j_type(cpu_state state, instruction inst){
	if (inst.opcode == 0b1101111) {
		int imediate = extend_from_n((inst.im1 << 12) + (inst.im2 << 11) + (inst.im3 << 1) + (inst.im4 << 20), 20); 
		state.registers[inst.rd] = state.pc + 4;
		state.pc += 4;
		return state;
	} else { 
		printf("Invalid U type instruction.\n");
		exit(0);
	}
} 

cpu_state execute_instruction(cpu_state initial_state, instruction input_instruction) {
	cpu_state result_state; 

	switch (input_instruction.type) {
		case R: result_state = execute_r_type(initial_state, input_instruction); break;
		case I: result_state = execute_i_type(initial_state, input_instruction); break;
		case S: result_state = execute_s_type(initial_state, input_instruction); break;
		case B: result_state = execute_b_type(initial_state, input_instruction); break; 
		case U: result_state = execute_u_type(initial_state, input_instruction); break;
		case J: result_state = execute_j_type(initial_state, input_instruction); break;
		default: printf("Execution type undefined.\n"); exit(0);
	}	
		
	return result_state;	
}

int main() {
	test_b_types();
	test_s_types();
	test_i_types();
	test_r_types();
	test_u_and_j_types();
	//test_asm("add r1, r2, r3\n");
	//print_instruction(new_instruction);
	//disasemble_instruction(new_instruction);
	//free(new_instruction);
}
