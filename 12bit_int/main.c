#include <stdint.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

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

int32_t reduce_to_12(int32_t input) {
	int32_t output = 0;
	int32_t max = signed_max(12);	
	int32_t min = signed_min(12);	

	assert(input <= max);
	assert(input >= min);

	output = input & 0b111111111111; //not clear that anything actually has to be done

	return output;
}

int32_t extend_from_12(int32_t input) {
	int32_t output = input;

	if (((output >> 11) & 1) == 1)  {
		//negative case
		output = output | 0b11111111111111111111000000000000;
	}

	return output;
}

int32_t enforce_12_bit_rep(int32_t input) {
	int32_t output = input & 0b111111111111;
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

int32_t enforce_bit_rep(int32_t input, int n) {
	int32_t output = input & (powi(2,n) -1); 
	return output;
}

int main() {
	int32_t min = signed_min(12);
	int32_t max = signed_max(12);

	for (int32_t input = min; input < max; input ++) {
		int32_t reduced = reduce_to_12(input);
		int32_t reduced2 = reduce_to_n(input,12);	
		if (reduced != reduced2) {
			printf("Reduced 1 and 2 not equal: %d, %d\n", reduced, reduced2);
			exit(0);
		}

		int32_t enforced = enforce_12_bit_rep(input);
		int32_t enforced2 = enforce_bit_rep(input, 12);
		if (enforced != enforced2) {
			printf("Enforced 1 and 2 not equal: %d, %d\n", enforced , enforced2);
			exit(0);
		}

		int32_t extended = extend_from_12(input);
		int32_t extended2 = extend_from_n(input,12);
		if (extended != extended2) {
			printf("Extended 1 and 2 not equal: %d, %d\n", extended, extended2);
			exit(0);
		}

		printf("Input %d, reduced: %d, enforced: %d, extended: %d\n", input, reduced, enforced, extended);
		printf("Input %d, reduced: %d, enforced: %d, extended: %d\n", input, reduced2, enforced2, extended2);

		if (input != extended) {
			printf("Error\n");
			exit(0);
		}
	}

}
