#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/random.h>

#define PRINTF(...) fprintf(stderr, __VA_ARGS__)
#define DEBUGP(...) fprintf(stderr, __VA_ARGS__)

//uint8_t extrabitslen = 0;
//uint8_t extrabits = 0;

uint8_t get_random_byte() {
	uint8_t buffer[1];
	ssize_t bytesread = getrandom(buffer, 1, 0);
	if (bytesread != 1) {
		PRINTF("get_random_byte: ERROR, getrandom returned wrong number of bytes\n");
	}

	return buffer[0];
}

uint64_t get_random_bits(uint8_t bits) {
	uint64_t out = 0;
	uint8_t outbits = 0;

	if (outbits < bits) {
		uint8_t buffer[8];

		// ceil(bits/8)
		uint8_t readbytes = (bits + 7) / 8;

		if (readbytes > 8) {
			PRINTF("get_random_bits: ERROR, readbytes too big, limiting to 8\n");
			readbytes = 8;
		}
		ssize_t bytesread = getrandom(buffer, readbytes, 0);

		if (bytesread != readbytes) {
			PRINTF("get_random_bits: ERROR, getrandom returned wrong number of bytes\n");
		}

		for (int i = 0; i < readbytes; i++) {
			out |= ((uint64_t)(buffer[i])) << (i*8);
		}

		if (bits == 64) {
			out &= (uint64_t)0 - 1;
		} else {
			out &= ((uint64_t)1 << bits) - 1;
		}
	}
	return out;
}

uint64_t sizeof_file(FILE* file) {
	long originalpos;
	long length;

	originalpos = ftell(file);
	fseek(file, 0, SEEK_END);
	length = ftell(file);
	fseek(file, 0, SEEK_SET);

	return length;
}

// Minimum bits needed to store a number, aka number of digits of a number if it was in binary.
uint8_t bitsof_int(uint64_t integer) {
	uint8_t bits = 0;

	if (integer > ((uint64_t)1 << 63) - 1)
		return 64;
	while (((uint64_t)1 << bits) - 1 < integer)
		bits++;
	return bits;
}

// uniform random. Saves a few calls to random by saving the number of bits.
uint64_t randomint(uint64_t max, uint8_t bits) {
	uint64_t out;
	do {
		out = get_random_bits(bits);
	} while (out >= max);
	return out;
}

int corruptbyte(FILE* file, uint64_t position) {
	uint8_t randombyte = get_random_byte();

	fseek(file, position, SEEK_SET);
	fwrite(&randombyte, 1, 1, file);

	return 0;
}

int main(int argc, char* argv[]) {
	FILE* corruptee;
	char command[256];
	uint64_t filesize;
	uint8_t filesizebits;

	if (argc < 3) {
		PRINTF("Please specify an input, output file, and number of passes\n");
		return 1;
	}

	sprintf(command, "cp %s %s", argv[1], argv[2]);
	if (system(command)) {
		PRINTF("Error copying %s to %s", argv[1], argv[2]);
		return 1;
	}
	sprintf(command, "cp %s /tmp/vido", argv[1]);
	if (system(command)) {
		PRINTF("Error copying %s to /tmp/vido", argv[1]);
		return 1;
	}

	corruptee = fopen(argv[2], "r+");
	filesize = sizeof_file(corruptee);
	filesizebits = bitsof_int(filesize);

	DEBUGP("filesize: %ld\n", filesize);
	DEBUGP("filesizebits: %d\n", filesizebits);

	for (int i = 0; i < 10; i++) {
		DEBUGP("randomint(filesize, filesizebits): %ld\n", randomint(filesize, filesizebits));
	}

	char command1[256];
	char command2[256];
	char command3[256];
	sprintf(command1, "[[ -z \"$(ffmpeg -v error -i %s -f null /dev/null 2>&1)\" ]]", argv[2]);
	sprintf(command2, "cp /tmp/vido %s", argv[2]);
	sprintf(command3, "cp %s /tmp/vido", argv[2]);

	int passes = atoi(argv[3]);
	int systemret;
	for (int i = 0; i < passes; i++) {
		PRINTF("\n%d/%d", i, passes);
		corruptbyte(corruptee, randomint(filesize, filesizebits));
		systemret = system(command1);
		if (systemret) {
			PRINTF("retry");
			system(command2);
			i--;
		} else {
			system(command3);
		}
	}
	PRINTF("\n");

	return 0;
}
