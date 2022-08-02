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

//		PRINTF("get_random_bits: DEBUG: readbytes = %d\n", readbytes);

		if (readbytes > 8) {
			PRINTF("get_random_bits: ERROR, readbytes too big, limiting to 8\n");
			readbytes = 8;
		}
//		extrabits = (readbytes * 8) - bits);
		ssize_t bytesread = getrandom(buffer, readbytes, 0);

//		DEBUGP("get_random_bits: DEBUG: bytesread = %d\n", bytesread);

		if (bytesread != readbytes) {
			PRINTF("get_random_bits: ERROR, getrandom returned wrong number of bytes\n");
		}

		for (int i = 0; i < readbytes; i++) {
			out |= ((uint64_t)(buffer[i])) << (i*8);
		}

//		DEBUGP("get_random_bits: DEBUG: out     = %016lx\n", out);
		if (bits == 64) {
			out &= (uint64_t)0 - 1;
//			DEBUGP("get_random_bits: DEBUG: bitmask = %016lx\n", (uint64_t)0 - 1);
		} else {
			out &= ((uint64_t)1 << bits) - 1;
//			DEBUGP("get_random_bits: DEBUG: bitmask = %016lx\n", ((uint64_t)1 << bits) - 1);
		}
//		DEBUGP("get_random_bits: DEBUG: out     = %016lx\n", out);
	}
/*	if (outbits < bits) {
		DEBUGP("get_random_bits: ERROR, not enough bits satisfied\n");
	}*/
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


//	uint8_t filebyte;
//	fseek(file, position, SEEK_SET);
//	fread(&filebyte, 1, 1, file);
//	DEBUGP("changing %x to %x\n", filebyte, randombyte);


	fseek(file, position, SEEK_SET);
	fwrite(&randombyte, 1, 1, file);


//	fseek(file, position, SEEK_SET);
//	fread(&filebyte, 1, 1, file);
//	DEBUGP("%x should now be %x\n", filebyte, randombyte);


	return 0;
}

int main(int argc, char* argv[]) {

	// testing code:

	PRINTF("get_random_bits(1):  %016lx\n", get_random_bits(1));
	PRINTF("get_random_bits(2):  %016lx\n", get_random_bits(2));
	PRINTF("get_random_bits(3):  %016lx\n", get_random_bits(3));
	PRINTF("get_random_bits(7):  %016lx\n", get_random_bits(7));
	PRINTF("get_random_bits(8):  %016lx\n", get_random_bits(8));
	PRINTF("get_random_bits(9):  %016lx\n", get_random_bits(9));
	PRINTF("get_random_bits(15): %016lx\n", get_random_bits(15));
	PRINTF("get_random_bits(16): %016lx\n", get_random_bits(16));
	PRINTF("get_random_bits(17): %016lx\n", get_random_bits(17));
	PRINTF("get_random_bits(33): %016lx\n", get_random_bits(33));
	PRINTF("get_random_bits(63): %016lx\n", get_random_bits(63));
	PRINTF("get_random_bits(64): %016lx\n", get_random_bits(64));

	PRINTF("bitsof_int(0):                  %d\n", bitsof_int(0));
	PRINTF("bitsof_int(1):                  %d\n", bitsof_int(1));
	PRINTF("bitsof_int(2):                  %d\n", bitsof_int(2));
	PRINTF("bitsof_int(3):                  %d\n", bitsof_int(3));
	PRINTF("bitsof_int(4):                  %d\n", bitsof_int(4));
	PRINTF("bitsof_int(5):                  %d\n", bitsof_int(5));
	PRINTF("bitsof_int(7):                  %d\n", bitsof_int(7));
	PRINTF("bitsof_int(8):                  %d\n", bitsof_int(8));
	PRINTF("bitsof_int(9):                  %d\n", bitsof_int(9));
	PRINTF("bitsof_int(64453):              %d\n", bitsof_int(64453));
	PRINTF("bitsof_int(0x8000000000000000): %d\n", bitsof_int(0x8000000000000000));
	PRINTF("bitsof_int(0xffffffffffffffff): %d\n", bitsof_int(0xffffffffffffffff));


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
//	sprintf(command1, "ffplay -autoexit %s", argv[2]);
	sprintf(command1, "[[ -z \"$(ffmpeg -v error -i %s -f null /dev/null 2>&1)\" ]]", argv[2]);
	sprintf(command2, "cp /tmp/vido %s", argv[2]);
	sprintf(command3, "cp %s /tmp/vido", argv[2]);

	int passes = 100;
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
