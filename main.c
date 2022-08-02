#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/random.h>

#define PRINTF(...) fprintf(stderr, __VA_ARGS__)

//uint8_t extrabitslen = 0;
//uint8_t extrabits = 0;

uint64_t get_random_bits(uint8_t bits) {
	uint64_t out = 0;
	uint8_t outbits = 0;

/*	if (extrabitslen) {
		if (extrabitslen > bits) {
			out = extrabits & ((2^bits) - 1);
			extrabitslen -= bits;
			extrabits = extrabits >> bits;
			return outbits;
		} else {
			out = extrabits & ((2^extrabitslen) - 1);
			outbits = extrabits;
			extrabits = 0;
			extrabitslen = 0;
		}
	}
*/
	if (outbits < bits) {
		uint8_t buffer[8];

		// ceil(bits/8)
		uint8_t readbytes = (bits + 7) / 8;
		if (readbytes > 8) {
			PRINTF("get_random_bits: ERROR, readbytes too big, limiting to 8");
			readbytes = 8;
		}
//		extrabits = (readbytes * 8) - bits);
		if (getrandom(buffer, readbytes, 0) != readbytes) {
			PRINTF("get_random_bits: ERROR, getrandom returned wrong number of bytes");
		}

		for (int i = 0; i < readbytes; i++) {
			out |= ((uint64_t)(buffer[i])) << (i/8)
		}

		out &= ((2^bits) - 1);
	}
	if (outbits < bits) {
		PRINTF("get_random_bits: ERROR, not enough bits satisfied");
	}
	return out;
}

uint64_t sizeof_file(FILE* file) {
	return 0;
}

uint8_t bitsof_int(uint64_t integer) {
	// finish properly later
	// return 1gb for the moment
	return 30;
}

uint64_t randomint(uint64_t max, uint8_t bits) {
	uint64_t out;
	do {
		out = get_random_bits(bits);
	} while (out < max);
	return out;
}

int main(int argc, char* argv[]) {

	// testing code:

	printf("get_random_bits(1):  %lx\n", get_random_bits(1));
	printf("get_random_bits(2):  %lx\n", get_random_bits(2));
	printf("get_random_bits(3):  %lx\n", get_random_bits(3));
	printf("get_random_bits(7):  %lx\n", get_random_bits(7));
	printf("get_random_bits(8):  %lx\n", get_random_bits(8));
	printf("get_random_bits(9):  %lx\n", get_random_bits(9));
	printf("get_random_bits(15): %lx\n", get_random_bits(15));
	printf("get_random_bits(16): %lx\n", get_random_bits(16));
	printf("get_random_bits(17): %lx\n", get_random_bits(17));
	printf("get_random_bits(33): %lx\n", get_random_bits(33));
	printf("get_random_bits(63): %lx\n", get_random_bits(63));
	printf("get_random_bits(64): %lx\n", get_random_bits(64));


/*	FILE* corruptee;
	char command[256];
	uint64_t filesize;
	uint8_t filesizeb;

	if (argc < 3) {
		PRINTF("Please specify an input and output file\n");
		return 1;
	}

	sprintf(command, "cp %s %s", argv[1], argv[2]);
	if (system(command)) {
		PRINTF("Error copying %s to %s", argv[1], argv[2]);
	}

	corruptee = fopen(argv[2], "r+");
	filesize = sizeof_file(corruptee);
	filesizebits = bitsof_int(filesize);*/
}
