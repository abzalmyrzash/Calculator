#include "utils.h"
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <assert.h>

#define MAX_NUMBER_LENGTH 17

static char str[MAX_NUMBER_LENGTH];

static const int numIntDigits = 13;
static const int maxPrecision = 10;

static double preciseIntLimit;
static double preciseFracLimit;

static const bool separateThousands = false;
static const char thousandsSeparator = ' ';
static const char fractionalSeparator = '.';
static const char fracThousandsSeparator = ' ';

static uint64 pow5_19;
static const double log2_base10 = log10(2);
static const double exp10neg1022 = 1022 * log10(2);

static bool initialized = false;
static void initialize() {
	preciseIntLimit = pow(10, numIntDigits);
	preciseFracLimit = pow(10, -maxPrecision) / 2;
	pow5_19 = intpow(5, 19);
	
	initialized = true;
}

void print_bytes(void* data, int nBytes) {
	for (int i = 0; i < nBytes; i++) {
		printf("%hhx ", *((unsigned char*)data + i));
	}
	printf("\n");
}

uint64 get_bits(double number, int nBits, int nBitsRight) {
	uint64 mask = 0;
	const uint64 one = 1;
	for (int i = 0; i < nBits; i++) {
		mask = mask | (one << i);
	}
	uint64 res = (*(uint64*)&number >> nBitsRight) & mask;
	return res; }

int intlog2(uint64 n) {
	int l = 0;
	while(n >>= 1) l++;
	return l;
}

uint64 intpow(int n, unsigned int exp) {
	int nSteps = intlog2(exp) + 1;
	uint64 steps[nSteps];
	steps[0] = n;
	for (int i = 0; i < nSteps - 1; i++) {
		steps[i + 1] = steps[i] * steps[i];
	}
	uint64 res = 1;
	for (int i = 0; i < nSteps; i++) {
		if (exp & 1) res *= steps[i];
		exp >>= 1;
	}
	return res;
}

uint64 get_decimal_frac(uint64 fracBits) {
	fracBits >>= 33; // 52 - 19
	return fracBits * pow5_19;
}

void print_parts(double number) {
	// get the rightmost bit
	char sign = get_bits(number, 1, 63);

	// get 11 bits after the sign bit
	
	short exp = get_bits(number, 11, 52) - 1023;

	// get the leftmost 52 bits
	uint64 fracBits = get_bits(number, 52, 0);
	uint64 decimal = get_decimal_frac(fracBits);
	// double frac = (double)fracBits / 0x10'0000'0000'0000;
	double exp10 = exp * log2_base10;

	if (sign == 1) printf("-");
	if (exp > 0) {
		printf("2^%d * 1.", exp);
	} else if (exp == 0) {
		printf("1.");
	} else if (exp > -1023) {
		printf("2^(%d) * 1.", exp);
	} else if (exp == -1023) {
		printf("2^(-1022) * 0.");
	} else {
		printf("Impossible\n");
	}
	printf("%019llu\n", decimal);
}

void print_number(double number) {
	if(!initialized) initialize();
	//print_parts(number);

	char intDigits[numIntDigits];
	char fracDigits[maxPrecision];

	if (number < 0) {
		printf("-");
		number = -number;
	}

	if (number >= preciseIntLimit ||
		number < preciseFracLimit ||
		number != number)
	{
		printf("%g", number);
		return;
	}
	//printf("%.100g\n", number);

	uint64 integer = floor(number);

	int exp = 0;
	uint64 integer_ = integer;
	while (integer_ != 0) {
		integer_ /= 10;
		exp++;
	}

	double fraction = number - integer;

	int precision = maxPrecision - exp;
	double minFraction = pow(10, -precision)/2;

	if (fraction > 0 && 1 - fraction < minFraction) {
		integer++;
		fraction = 0;
	}

	int digitCnt = 0;
	// integer digits will be reversed, will have to flip later
	while(integer != 0) {
		intDigits[digitCnt++] = '0' + (integer % 10);
		integer /= 10;
	}

	int fracDigit;
	int fracDigitCnt = 0;

	if (fraction == 0) goto compose_string;

	for (int i = 0; i < precision; i++) {
		if (fabs(fraction) < minFraction) break;
		minFraction *= 10;
		fraction *= 10;
		if (ceil(fraction) - fraction <= minFraction) {
			fracDigit = ceil(fraction);
		} else fracDigit = floor(fraction);
		fracDigits[fracDigitCnt++] = '0' + fracDigit;
		fraction -= fracDigit;
	}

	compose_string:

	int strI = 0; // string iterator

	// integer part is 0
	if (digitCnt == 0) {
		str[strI++] = '0';
		goto fraction;
	}
	// add integer digits in reverse order
	for (int i = digitCnt - 1; i >= 0; i--) {
		str[strI++] = intDigits[i];
		if (separateThousands) {
			if (i > 2 && i % 3 == 0) {
				str[strI++] = thousandsSeparator;
			}
		}
	}

	fraction:

	if (fracDigitCnt == 0) goto print;
	int fracSep = fracDigitCnt % 3;
	str[strI++] = fractionalSeparator;
	// fractional digits are not reversed, add them normally
	for (int i = 0; i < fracDigitCnt; i++) {
		if (separateThousands) {
			if (i > 0 && i % 3 == fracSep) {
				str[strI++] = fracThousandsSeparator;
			}
		}
		str[strI++] = fracDigits[i];
	}

	print:
	// add null terminator
	str[strI] = '\0';

	printf(str);
}
