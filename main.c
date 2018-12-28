/*
2018 David DiPaola
licensed under CC0 (public domain, see https://creativecommons.org/publicdomain/zero/1.0/)
*/

#include <unistd.h>

#include <stdio.h>

static unsigned char
_byte_tohex(unsigned char byte) {
	if ((byte >= '0') && (byte <= '9')) {
		return (byte - '0') + 0x0;
	}
	else if ((byte >= 'A') && (byte <= 'F')) {
		return (byte - 'A') + 0xA;
	}
	else if ((byte >= 'a') && (byte <= 'f')) {
		return (byte - 'a') + 0xA;
	}
	else {
		return 0xFF;
	}
}

static void
_utf8_write(unsigned int codepoint) {
	int status;
	size_t bytes_size;
	if (codepoint <= 0x7F) {
		const char bytes[] = {
			codepoint
		};
		bytes_size = sizeof(bytes);
		status = write(STDOUT_FILENO, bytes, bytes_size);
	}
	else if (codepoint <= 0x7FF) {
		const char bytes[] = {
			((0b110 << 5) | ((codepoint >> (6 * 1)) &  0b11111)),
			((0b10  << 6) | ((codepoint >> (6 * 0)) & 0b111111)),
		};
		bytes_size = sizeof(bytes);
		status = write(STDOUT_FILENO, bytes, bytes_size);
	}
	else if (codepoint <= 0xFFFF) {
		const char bytes[] = {
			((0b1110 << 4) | ((codepoint >> (6 * 2)) &   0b1111)),
			((0b10   << 6) | ((codepoint >> (6 * 1)) & 0b111111)),
			((0b10   << 6) | ((codepoint >> (6 * 0)) & 0b111111)),
		};
		bytes_size = sizeof(bytes);
		status = write(STDOUT_FILENO, bytes, bytes_size);
	}
	else if (codepoint <= 0x10FFFF) {
		const char bytes[] = {
			((0b11110 << 3) | ((codepoint >> (6 * 3)) &    0b111)),
			((0b10    << 6) | ((codepoint >> (6 * 2)) & 0b111111)),
			((0b10    << 6) | ((codepoint >> (6 * 1)) & 0b111111)),
			((0b10    << 6) | ((codepoint >> (6 * 0)) & 0b111111)),
		};
		bytes_size = sizeof(bytes);
		status = write(STDOUT_FILENO, bytes, bytes_size);
	}
	if (status < bytes_size) {
		perror(NULL);
	}
}

int
main() {
	unsigned char byte;
	int           sawhex    = 0;
	unsigned int  codepoint = 0;
	while (read(STDIN_FILENO, &byte, sizeof(byte)) == sizeof(byte)) {
		unsigned char byte_hex = _byte_tohex(byte);
		if (byte_hex <= 0xF) {
			sawhex = 1;
			codepoint = (codepoint << 4) | byte_hex;
		}
		else if(sawhex) {
			_utf8_write(codepoint);
			codepoint = 0;
			sawhex    = 0;
		}
	}

	return 0;
}

