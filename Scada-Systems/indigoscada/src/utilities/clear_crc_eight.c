/**************************************************************************************************
Credit to JohnPaul Adamovsky ( logarithm69@hotmail.com ) 
**************************************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "clear_crc_eight.h"

#define BYTE_WIDTH 8
#define REMAINDER_WIDTH 8

// Define a boolean type as an enumeration.
typedef enum { FALSE = 0, TRUE = 1 } Bool;

static const unsigned char PowersOfTwo[REMAINDER_WIDTH] = {1, 2, 4, 8, 16, 32, 64, 128};

// This truncated "Divisor", when displayed in binary, looks like this (11010101).
static const unsigned char Poly = 213;

// This function performs the clearest type of modulo-2 bitwise long-division on "NumberOfBytes" Bytes, starting at "DataMessage".
// The divisor used is defined in the constant "Poly".
// The value returned by the function is the "FinalRemainder", or CRC-Digest.
unsigned char clearCrc(const unsigned char *DataMessage, unsigned int NumberOfBytes)
{
//	unsigned int X;
	unsigned char CurrentRemainder = 0;
	unsigned int NextMessageBit;
	unsigned int NextMessageByte;
	
	// Load the first Byte of "DataMessage" into the "CurrentRemainder".
	memcpy(&CurrentRemainder, DataMessage, sizeof(unsigned char));
	NextMessageByte = sizeof(unsigned char);
	NextMessageBit = (BYTE_WIDTH - 1);
	
	// This loop eliminates "1"s in the "MSB" position of "CurrentRemainder" using bit-shifting and the "XOR" operation with "Poly".
	while ( TRUE ) 
	{
		// This loop left-shifts "0"s out of the "CurrentRemainder".
		// The process continues until a "1" is found, or the final message bit is pulled in from the right-hand-side.
		while ( !(CurrentRemainder & PowersOfTwo[REMAINDER_WIDTH - 1]) ) 
		{
			CurrentRemainder <<= 1;
			CurrentRemainder += (DataMessage[NextMessageByte] & PowersOfTwo[NextMessageBit])? 1: 0;
			// Increment the "NextMessageByte" when we have just pulled bit zero into the "CurrentRemainder".
			// Exit when there are no more bytes left to process.
			if ( !NextMessageBit ) 
			{
				NextMessageByte +=1;
				// End Event #1.
				if ( NextMessageByte == NumberOfBytes ) 
				{
				//	printf("End Event #1\n");
					return CurrentRemainder;
				}

				NextMessageBit = (BYTE_WIDTH - 1);
			}
			else NextMessageBit -= 1;
		}

		CurrentRemainder <<= 1;

		CurrentRemainder += (DataMessage[NextMessageByte] & PowersOfTwo[NextMessageBit])? 1: 0;

		// The pivotal "XOR" operation.
		CurrentRemainder ^= Poly;

		if ( !NextMessageBit ) 
		{
			NextMessageByte +=1;
			// End Event #2
			if ( NextMessageByte == NumberOfBytes ) 
			{
				//printf("End Event #2\n");
				return CurrentRemainder;
			}

			NextMessageBit = (BYTE_WIDTH - 1);
		}
		else NextMessageBit -= 1;
	}
}

// Simply print out "ThisByte" using "1"s and "0"s.
void printByteInBinary(unsigned char ThisByte)
{
	unsigned int X;
	char HoldOut[BYTE_WIDTH + 1];
	for ( X = 0; X < BYTE_WIDTH; X++ ) HoldOut[X] = (ThisByte & PowersOfTwo[BYTE_WIDTH - 1 - X])? '1': '0';
	HoldOut[BYTE_WIDTH] = '\0';
	printf("%s", HoldOut);
}

#ifdef EXECUTE_CRC_TEST

#define MESSAGE_LENGTH 3

int main(){
	unsigned int X;
	// Include an extra Byte in the message to test the trailing-zeros CRC property.
	unsigned char DemonstrationMessage[MESSAGE_LENGTH + 1] = {202, 45, 166, 0};
	unsigned char TheDigest;
	
	printf("\n---------------------------------------------------------------------------------\n");
	
	printf("\nBehold the |%d| Byte DataMessage - |", MESSAGE_LENGTH);
	for ( X = 0; X < MESSAGE_LENGTH; X++ ) {
		printByteInBinary(DemonstrationMessage[X]);
		printf("|");
	}
	printf("\n\n");
	
	TheDigest = clearCrc(DemonstrationMessage, MESSAGE_LENGTH*sizeof(unsigned char));
	printf("Behold TheDigest-|%d|-|", TheDigest);
	printByteInBinary(TheDigest);
	printf("|\n");
	
	printf("\n---------------------------------------------------------------------------------\n");
	
	// Calculate "TheDigest" with appended zeros, and then replace the trailing zeros with this number.
	// Recalculate "TheDigest", and this time the digest should compute to zero.
	
	printf("\nAppend \"W\" zeros to the DataMessage - |");
	for ( X = 0; X < (MESSAGE_LENGTH + 1); X++ ) {
		printByteInBinary(DemonstrationMessage[X]);
		printf("|");
	}
	printf("\n\n");
	
	TheDigest = clearCrc(DemonstrationMessage, MESSAGE_LENGTH*sizeof(unsigned char) + 1);
	printf("Behold TheDigest-|%d|-|", TheDigest);
	printByteInBinary(TheDigest);
	printf("|\n");
	
	printf("\n---------------------------------------------------------------------------------\n");
	
	DemonstrationMessage[MESSAGE_LENGTH] = TheDigest;
	printf("\n\"TheDigest\" of this DataMessage should be zero - |");
	for ( X = 0; X < (MESSAGE_LENGTH + 1); X++ ) {
		printByteInBinary(DemonstrationMessage[X]);
		printf("|");
	}
	printf("\n\n");
	
	TheDigest = clearCrc(DemonstrationMessage, MESSAGE_LENGTH*sizeof(unsigned char) + 1);
	printf("Behold TheDigest-|%d|-|", TheDigest);
	printByteInBinary(TheDigest);
	printf("|\n");
	
	printf("\n---------------------------------------------------------------------------------\n");
	printf("\n");
	
	return 0;
}

#endif //CRC_TEST