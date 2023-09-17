#include "skinny_aff.h"
#include "../Drivers/Inc/trng.h"
#include "mask.h"

unsigned char r1; // multiplicative mask
unsigned char r0; // aditive mask

// Skinny-128-384+ parameters: 128-bit block, 384-bit tweakey and 40 rounds
extern int BLOCK_SIZE;
extern int TWEAKEY_SIZE;
extern int N_RNDS;

// Tweakey permutation
extern unsigned char TWEAKEY_P[16];

// round constants
extern const unsigned char RC[40];

const unsigned char Sbox_8 [256] = {
                              0x65 ,0x4c ,0x6a ,0x42 ,0x4b ,0x63 ,0x43 ,0x6b ,0x55 ,0x75 ,0x5a ,0x7a ,0x53 ,0x73 ,0x5b ,0x7b,
                              0x35 ,0x8c ,0x3a ,0x81 ,0x89 ,0x33 ,0x80 ,0x3b ,0x95 ,0x25 ,0x98 ,0x2a ,0x90 ,0x23 ,0x99 ,0x2b,
                              0xe5 ,0xcc ,0xe8 ,0xc1 ,0xc9 ,0xe0 ,0xc0 ,0xe9 ,0xd5 ,0xf5 ,0xd8 ,0xf8 ,0xd0 ,0xf0 ,0xd9 ,0xf9,
                              0xa5 ,0x1c ,0xa8 ,0x12 ,0x1b ,0xa0 ,0x13 ,0xa9 ,0x05 ,0xb5 ,0x0a ,0xb8 ,0x03 ,0xb0 ,0x0b ,0xb9,
                              0x32 ,0x88 ,0x3c ,0x85 ,0x8d ,0x34 ,0x84 ,0x3d ,0x91 ,0x22 ,0x9c ,0x2c ,0x94 ,0x24 ,0x9d ,0x2d,
                              0x62 ,0x4a ,0x6c ,0x45 ,0x4d ,0x64 ,0x44 ,0x6d ,0x52 ,0x72 ,0x5c ,0x7c ,0x54 ,0x74 ,0x5d ,0x7d,
                              0xa1 ,0x1a ,0xac ,0x15 ,0x1d ,0xa4 ,0x14 ,0xad ,0x02 ,0xb1 ,0x0c ,0xbc ,0x04 ,0xb4 ,0x0d ,0xbd,
                              0xe1 ,0xc8 ,0xec ,0xc5 ,0xcd ,0xe4 ,0xc4 ,0xed ,0xd1 ,0xf1 ,0xdc ,0xfc ,0xd4 ,0xf4 ,0xdd ,0xfd,
                              0x36 ,0x8e ,0x38 ,0x82 ,0x8b ,0x30 ,0x83 ,0x39 ,0x96 ,0x26 ,0x9a ,0x28 ,0x93 ,0x20 ,0x9b ,0x29,
                              0x66 ,0x4e ,0x68 ,0x41 ,0x49 ,0x60 ,0x40 ,0x69 ,0x56 ,0x76 ,0x58 ,0x78 ,0x50 ,0x70 ,0x59 ,0x79,
                              0xa6 ,0x1e ,0xaa ,0x11 ,0x19 ,0xa3 ,0x10 ,0xab ,0x06 ,0xb6 ,0x08 ,0xba ,0x00 ,0xb3 ,0x09 ,0xbb,
                              0xe6 ,0xce ,0xea ,0xc2 ,0xcb ,0xe3 ,0xc3 ,0xeb ,0xd6 ,0xf6 ,0xda ,0xfa ,0xd3 ,0xf3 ,0xdb ,0xfb,
                              0x31 ,0x8a ,0x3e ,0x86 ,0x8f ,0x37 ,0x87 ,0x3f ,0x92 ,0x21 ,0x9e ,0x2e ,0x97 ,0x27 ,0x9f ,0x2f,
                              0x61 ,0x48 ,0x6e ,0x46 ,0x4f ,0x67 ,0x47 ,0x6f ,0x51 ,0x71 ,0x5e ,0x7e ,0x57 ,0x77 ,0x5f ,0x7f,
                              0xa2 ,0x18 ,0xae ,0x16 ,0x1f ,0xa7 ,0x17 ,0xaf ,0x01 ,0xb2 ,0x0e ,0xbe ,0x07 ,0xb7 ,0x0f ,0xbf,
                              0xe2 ,0xca ,0xee ,0xc6 ,0xcf ,0xe7 ,0xc7 ,0xef ,0xd2 ,0xf2 ,0xde ,0xfe ,0xd7 ,0xf7 ,0xdf ,0xff
                             };

unsigned char AffSbox_8 [256];

// Declare global tabs for LFSRs calculation
// Tab to rotate 1 bit leftwards and then add bit 5 to position 0
unsigned char lfsr1[256] = {0};
// Tab to rotate 1 bit rightwards and then add bit 6 to position 7
unsigned char lfsr2[256] = {0};



// apply the 8-bit Sbox
void AffSubCell8(unsigned char IS[4][4]) {
    int i, j;
    for(i = 0; i < 4; i++){
		for(j = 0; j <  4; j++){
			IS[i][j] = AffSbox_8[IS[i][j]];
		}
	}
}


// Function to add to operands with affine mask
unsigned char AffAdd(unsigned char ga, unsigned char b) {
    unsigned char m = get_random() % 40;
    return (((ga ^ m) ^ g(b, r1, r0)) ^ r0) ^ m;
}

// Apply the constants: using a LFSR counter on 6 bits, we XOR the 6 bits to the first 6 bits of the internal state
// Masquer avec G les constantes qu'on ajoute et state en argument doit être masqué aussi
// NE PAS MASQUER LA CST CAR ELLE EST CONNUE
void AffAddConstants(unsigned char state[4][4], const unsigned char r)
{
    //_Static_assert(r < 40);
    // Ici la constante est masquée dans AffAdd, state est dejà masqué
    state[0][0] = AffAdd(state[0][0], (RC[r] & 0xf));
    state[1][0] = AffAdd(state[1][0], ((RC[r]>>4) & 0x3));
    state[2][0] = AffAdd(state[2][0], 0x2);
}

// Function to add 3 masked operands 
unsigned char AffAdd3Op_m(unsigned char gstate, unsigned char gk1, unsigned char gk2, unsigned char gk3) {
    unsigned char m = get_random();
    return (((gstate ^ m) ^ gk1 ^ gk2 ^ gk3) ^ r0) ^ m;
}

// Extract and apply the subtweakey to the internal state (must be the two top rows XORed together), then update the tweakey state
// !!ATTENTION State AND keyCells have to be masked at the entry of the function
void AffAddKey(unsigned char state[4][4], unsigned char keyCells[3][4][4])
{
	int i, j, k;
	unsigned char pos;
	unsigned char keyCells_tmp[3][4][4];

    // apply the subtweakey to the internal state
    // ISi,j = ISi,j ^ T K1i,j ^ T K2i,j ^ T K3i,j
    for(i = 0; i <= 1; i++)
    {
        for(j = 0; j < 4; j++)
        {
            state[i][j] = AffAdd3Op_m(state[i][j], keyCells[0][i][j], keyCells[1][i][j], keyCells[2][i][j]);
        }
    }

    // update the subtweakey states with the permutation
    // Here TWEAKEY_SIZE/BLOCK_SIZE == 3
    // TWEAKEY_P[16] = {9,15,8,13,10,14,12,11,0,1,2,3,4,5,6,7};
    for(k = 0; k <(int)(TWEAKEY_SIZE/BLOCK_SIZE); k++){
        for(i = 0; i < 4; i++){
            for(j = 0; j < 4; j++){
                //application of the TWEAKEY permutation
                pos=TWEAKEY_P[j+4*i];
                keyCells_tmp[k][i][j]=keyCells[k][pos>>2][pos&0x3];
            }
        }
    }

    // update the subtweakey states with the LFSRs
    // Here TWEAKEY_SIZE/BLOCK_SIZE == 3
    for(k = 0; k <(int)(TWEAKEY_SIZE/BLOCK_SIZE); k++){
        for(i = 0; i <= 1; i++){
            for(j = 0; j < 4; j++){
                //application of LFSRs for TK updates
                if (k==1)
                {
                    keyCells_tmp[k][i][j] = lfsr1[keyCells_tmp[k][i][j]];
                }
                else if (k==2)
                {
                    keyCells_tmp[k][i][j] = lfsr2[keyCells_tmp[k][i][j]];
                }
            }
        }
    }

    // Updating keyCells for next round
    for(k = 0; k <(int)(TWEAKEY_SIZE/BLOCK_SIZE); k++){
        for(i = 0; i < 4; i++){
            for(j = 0; j < 4; j++){
                keyCells[k][i][j]=keyCells_tmp[k][i][j];
            }
        }
    }
}

// Apply the protected ShiftRows function
// G(s') = ShiftRows(G(s))
// AffShiftRows adn ShiftRows are exactly the same functions
// but the input state has to masked in AffShiftRows 
void AffShiftRows(unsigned char state[4][4])
{
    unsigned char tmp;
    tmp = state[1][3];
    state[1][3] = state[1][2];
    state[1][2] = state[1][1];
    state[1][1] = state[1][0];
    state[1][0] = tmp;
 
    tmp = state[2][0];
    state[2][0] = state[2][2];
    state[2][2] = tmp;
    tmp = state[2][1];
    state[2][1] = state[2][3];
    state[2][3] = tmp;
 
    tmp = state[3][0];
    state[3][0] = state[3][1];
    state[3][1] = state[3][2];
    state[3][2] = state[3][3];
    state[3][3] = tmp;
}

// Every input is already masked
unsigned char MixColumnSecXor(unsigned char gs1, unsigned char gs2) {
    unsigned char m = get_random();
    return (((gs1 ^ m) ^ gs2) ^ r0) ^ m;
}

// Apply the linear diffusion matrix
// Each column of the cipher internal state array is multiplied by the following matrix:
//M =
//1 0 1 1
//1 0 0 0
//0 1 1 0
//1 0 1 0
// We just protect the XOR operation
void AffMixColumn(unsigned char state[4][4]) {

	int j;
    unsigned char temp;

	for(j = 0; j < 4; j++){
        state[1][j] = MixColumnSecXor(state[1][j], state[2][j]);
        state[2][j] = MixColumnSecXor(state[2][j], state[0][j]);
        state[3][j] = MixColumnSecXor(state[3][j], state[2][j]);

        temp=state[3][j];
        state[3][j]=state[2][j];
        state[2][j]=state[1][j];
        state[1][j]=state[0][j];
        state[0][j]=temp;
	}

}

// encryption function of Skinny-128-384+
void Affenc(unsigned char* input, const unsigned char* userkey)
{
    unsigned char G [256];

	unsigned char state[4][4];
	unsigned char keyCells[3][4][4];
	int i;

    // True Random Number generator
    r1 = get_random(); // multiplicative mask
    r0 = get_random(); // aditive mask

    // Create tabs for LFSRs calculation depending on r1 and r0 values
    for (int k = 0; k < 256; k++) {
        unsigned char masked_val = g(k, r1, r0);
        lfsr1[masked_val]  = g(((k << 1) & 0xfe) ^ ((k >> 7) & 0x01) ^ ((k >> 5) & 0x01), r1, r0);
        lfsr2[masked_val] = g(((k >> 1) & 0x7f) ^ ((k << 7) & 0x80) ^ ((k << 1) & 0x80), r1, r0);
    }
	
    // Calculate the masked SBox
    for (int i=0; i < 256; i++) {
        G[i] = g(i, r1, r0);
    }
    for (int i=0; i < 256; i++) {
        AffSbox_8[G[i]] = G[Sbox_8[i]];
    }

    // Transform input in an array [4][4]
    // Transform userkey in an array [3][4][4]
	for(i = 0; i < 16; i++) {
        state[i>>2][i&0x3] = input[i]&0xFF;
        keyCells[0][i>>2][i&0x3] = userkey[i]&0xFF;
	    keyCells[1][i>>2][i&0x3] = userkey[i+16]&0xFF;
	    keyCells[2][i>>2][i&0x3] = userkey[i+32]&0xFF;
	}

    // mask keyCells
    for(int k = 0; k <(int)(TWEAKEY_SIZE/BLOCK_SIZE); k++) {
        for(int i = 0; i < 4; i++)
            for(int j = 0; j < 4; j++)
                keyCells[k][i][j] = g(keyCells[k][i][j], r1, r0);
    }

    // mask matrix before the begining of the encryption
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++)
            state[i][j] = g(state[i][j], r1, r0);
    }

	for(i = 0; i < N_RNDS; i++){
        AffSubCell8(state);
	    AffAddConstants(state, i);
	    AffAddKey(state, keyCells);
	    AffShiftRows(state);
	    AffMixColumn(state);
	}  //The last subtweakey should not be added


    // Unmask matrix at the end of the encryption
    for(int i = 0; i < 4; i++) {
        for(int j = 0; j < 4; j++)
            state[i][j] = mulGF256(state[i][j] ^ r0, invmulGF256(r1));
    }
	
    for(i = 0; i < 16; i++)
		input[i] = state[i>>2][i&0x3] & 0xFF;
        
}

void Aff_skinny_128_384_plus_enc (unsigned char* input, const unsigned char* userkey) {
 	Affenc(input,userkey); 
}
