#include <stdint.h>
#include <stdio.h>

typedef struct int12 {
	uint8_t data [2];
} int12;

int12 blank_int12() {
	int12 output;
	output.data[0] = 0;
	output.data[0] = 0;
	return output;
}

uint8_t int_12_get_bit(int12 source, int index) {
	if (index >= 8) {
		index -= 8;
		return (source.data[1] >> index) & 0b1;
	} else {
		return (source.data[0] >> index) & 0b1;
	}
}

int12 int32_to_int12(int32_t input) {
	int12 output = blank_int12();
	if (input == 0) {
		return output;
	} else if (input > 0) {
		output.data[0] = input & 0b11111111;		
		output.data[1] = (input >> 8) & 0b1111;		
		return output;
	} else {
		input = input * -1;
		output.data[0] = input & 0b11111111;		
		output.data[1] = (input >> 8) & 0b1111;		
		output.data[1] = (input >> 8) | 0b1000;		

		if (output.data[0] == 255) {
			output.data[1] += 1;
		} else {
			output.data[0] += 1;
		}

		return output;
	}
}

uint32_t int12_to_int32(int12 input) {

}

//uint32_t int12_to_buffer() {
//
//}
//
//uint32_t buffer_to_int12() {
//
//}
int main () {
	uint32_t u = 0b0001000111;
	int32_t s = (int32_t)u;
	printf("%u %d\n", u, s);
}
