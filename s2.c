#include <stdio.h>
#include <stdlib.h>
#include "grain.h"

#define SAMPLE_SIZE 10000

int grain_keystream(grain* mygrain) {
	int i,NBit,LBit,outbit;
	/* Calculate feedback and output bits */
	outbit = N(79)^N(78)^N(76)^N(70)^N(49)^N(37)^N(24) ^ X1 ^ X4 ^ X0&X3 ^ X2&X3 ^ X3&X4 ^ X0&X1&X2 ^ X0&X2&X3 ^ X0&X2&X4 ^ X1&X2&X4 ^ X2&X3&X4;
 
	NBit=L(80)^N(18)^N(20)^N(28)^N(35)^N(43)^N(47)^N(52)^N(59)^N(66)^N(71)^N(80)^
			N(17)&N(20) ^ N(43)&N(47) ^ N(65)&N(71) ^ N(20)&N(28)&N(35) ^ N(47)&N(52)&N(59) ^ N(17)&N(35)&N(52)&N(71)^
			N(20)&N(28)&N(43)&N(47) ^ N(17)&N(20)&N(59)&N(65) ^ N(17)&N(20)&N(28)&N(35)&N(43) ^ N(47)&N(52)&N(59)&N(65)&N(71)^
			N(28)&N(35)&N(43)&N(47)&N(52)&N(59);
	LBit=L(18)^L(29)^L(42)^L(57)^L(67)^L(80);
	/* Update registers */
	for (i=1;i<(mygrain->keysize);++i) {
		mygrain->NFSR[i-1]=mygrain->NFSR[i];
		mygrain->LFSR[i-1]=mygrain->LFSR[i];
	}
	mygrain->NFSR[(mygrain->keysize)-1]=NBit;
	mygrain->LFSR[(mygrain->keysize)-1]=LBit;
	return outbit;	
}

void keysetup(
  grain* mygrain, 
  const int* key, 
  int keysize,                /* Key size in bits. */ 
  int ivsize)				  /* IV size in bits. */ 
{
	mygrain->p_key=key;
	mygrain->keysize=keysize;
	mygrain->ivsize=ivsize;
}

void ivsetup(
  grain* mygrain, 
  const int* iv)
{
	int i,j;
	int outbit;
	/* load registers */
	for (i=0;i<(mygrain->ivsize)/8;++i) {
		for (j=0;j<8;++j) {
			mygrain->NFSR[i*8+j]=((mygrain->p_key[i]>>j)&1);  
			mygrain->LFSR[i*8+j]=((iv[i]>>j)&1);
		}
	}
	for (i=(mygrain->ivsize)/8;i<(mygrain->keysize)/8;++i) {
		for (j=0;j<8;++j) {
			mygrain->NFSR[i*8+j]=((mygrain->p_key[i]>>j)&1);
			mygrain->LFSR[i*8+j]=1;
		}
	}
	/* do initial clockings */
	for (i=0;i<INITCLOCKS;++i) {
		outbit=grain_keystream(mygrain);
		mygrain->LFSR[79]^=outbit;
		mygrain->NFSR[79]^=outbit;             
	}

}



int ks[10];
void keystream_bytes(
  grain* mygrain, 
  int* keystream, 
  int msglen)
{
	int i,j;
	for (i = 0; i < msglen; ++i) {
		keystream[i]=0;
		for (j = 0; j < 8; ++j) {
			keystream[i]|=(grain_keystream(mygrain)<<j);
		}
	}
}


void printData(int *IV, int *ks) {
    int i;
    printf("IV:        ");
    for (i = 0; i < 8; ++i) {
        printf("%02x", IV[i]);
    }
    printf("\nKeystream:  ");
    for (i = 0; i < 10; ++i) {
        printf("%02x", ks[i]);
    }
    printf("\n");
}

void transferDataToCSV(int iv[][8], int keystream[][10], int numSamples) {
    FILE *csvFile = fopen("data.csv", "w");
    if (csvFile == NULL) {
        printf("Error opening file.\n");
        return;
    }

    // Write the column headers
    fprintf(csvFile, "IV,Keystream\n");

    // Write the data rows
    for (int i = 0; i < numSamples; ++i) {
        for (int j = 0; j < 8; ++j) {
            fprintf(csvFile, "%02x", iv[i][j]);
        }
        fprintf(csvFile, ",");
        for (int j = 0; j < 10; ++j) {
            fprintf(csvFile, "%02x", keystream[i][j]);
        }
        fprintf(csvFile, "\n");
    }

    fclose(csvFile);
    printf("Data transferred to data.csv successfully.\n");
}

int main() {
    /*grain mygrain;
    int IV[SAMPLE_SIZE][8];
    int keystream2[SAMPLE_SIZE][10];
    int i;

    int key1[10] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0x12, 0x34};
    int IV2[8] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef};

    for (i = 0; i < SAMPLE_SIZE; ++i) {
        keysetup(&mygrain, key1, 80, 64);  // Replace 'key1' with your desired key
        ivsetup(&mygrain, IV2);  // Replace 'IV2' with your desired IV
        keystream_bytes(&mygrain, keystream2[i], 10);

        // Copy IV to the array
        int j;
        for (j = 0; j < 8; ++j) {
            IV[i][j] = IV2[j];  // Replace 'IV2' with your desired IV
        }
    }

    // Print the first 10 samples
    for (i = 0; i < 10; ++i) {
        printData(IV[i], keystream2[i]);
    }

    // Call the function to transfer the data to the CSV file
    transferDataToCSV(IV, keystream2, SAMPLE_SIZE);

    return 0;
    */
    grain mygrain;
    int IV[SAMPLE_SIZE][8];
    int keystream2[SAMPLE_SIZE][10];
    int i;

    int key1[10] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0x12, 0x34};

    for (i = 0; i < SAMPLE_SIZE; ++i) {
        int IV2[8];  // Define IV2 inside the loop for each sample
        int keystream[10];  // Define keystream inside the loop for each sample

        // Generate random IV
        int j;
        for (j = 0; j < 8; ++j) {
            IV2[j] = rand() & 0xFF;  // Generate a random byte
        }

        keysetup(&mygrain, key1, 80, 64);
        ivsetup(&mygrain, IV2);
        keystream_bytes(&mygrain, keystream, 10);

        // Copy IV and keystream to the arrays
        for (j = 0; j < 8; ++j) {
            IV[i][j] = IV2[j];
        }
        for (j = 0; j < 10; ++j) {
            keystream2[i][j] = keystream[j];
        }
    }

    // Print the first 10 samples
    for (i = 0; i < 10; ++i) {
        printData(IV[i], keystream2[i]);
    }

    // Call the function to transfer the data to the CSV file
    transferDataToCSV(IV, keystream2, SAMPLE_SIZE);

}
