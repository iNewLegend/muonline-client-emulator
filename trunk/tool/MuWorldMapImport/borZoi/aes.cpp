/* 
   borZoi - An Elliptic Curve Cryptography Library

   Copyright (C) 2001 Anthony Mulcahy

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   \You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  

*/


#include "borzoi.h"
extern "C" {
#include "rijndael-alg-fst.h"
}




OCTETSTR Enc (OCTETSTR U, OCTETSTR KB);
OCTETSTR Dec (OCTETSTR U, OCTETSTR KB);


// AES_CBC_IV0

OCTETSTR Enc (OCTETSTR U, OCTETSTR KB, int Nr, int keysize) {
	int j;

	OCTET Key[32];
	for (j=0; j<32; j++) {
		Key[j] = KB[j];
	}
	unsigned int KeySchedule[4*(14+1)];
	OCTET u[16];
	//rijndaelKeySetupEnc (KeySchedule, Key, 128);
	rijndaelKeySetupEnc (KeySchedule, Key, keysize);
	for (j=0; j<16; j++) {
		u[j] = U[j];
	}

	OCTET c[16];
	rijndaelEncrypt (KeySchedule, Nr, u, c);

	OCTETSTR C (16);
	for (j=0; j<16; j++) {
		C[j]=c[j];
	}
	return C;
}


OCTETSTR Dec (OCTETSTR KB, OCTETSTR U, int Nr, int keysize) {
	int j;

	OCTET Key[32];
	for (j=0; j<32; j++) {
		Key[j] = KB[j];
	}
	unsigned int KeySchedule[4*(14+1)];
	OCTET u[16];
	//rijndaelKeySetupDec (KeySchedule, Key, 128);
	rijndaelKeySetupDec (KeySchedule, Key, keysize);
	for (j=0; j<16; j++) {
		u[j] = U[j];
	}

	OCTET p[16];
	rijndaelDecrypt (KeySchedule, Nr, u, p);

	OCTETSTR P (16);
	for (j=0; j<16; j++) {
		P[j]=p[j];
	}
	return P;
}


OCTETSTR AES_CBC_IV0_Encrypt (OCTETSTR KB, OCTETSTR M, int keysize) {
	int Nr;

	switch (keysize) {
		case 256:
			Nr = 14;
			break;
		case 192:
			Nr = 12;
			break;
		default:
			Nr = 10;
	}

	OCTETSTR::size_type mLen = M.size();
	OCTET padLen = 16 - (mLen%16);
	long k = ((mLen+1)/16);
	if ((mLen+1)%16)
		k++;

	OCTETSTR P1(1);
	P1[0] = padLen;
	OCTETSTR P2(padLen);
	for (OCTET l=0; l<padLen; l++) {
		P2[l] = P1[0];
	}
	OCTETSTR T = M || P2;
	OCTETSTR C (16);
	OCTETSTR U (16);
	for (long i=1; i<=k; i++) {
		for (int j=0; j<16; j++) {
			if (i==1)
				U[j] = T[(i-1)*16+j];
			else U[j] = T[(i-1)*16+j] ^ C[(i-2)*16+j];
		}
		if (i==1)
			C = Enc (U, KB, Nr, keysize);
		else C = C || Enc (U, KB, Nr, keysize);
	}
	return C;
}


// Returns the decrypted plain text or else throws borzoiException
OCTETSTR AES_CBC_IV0_Decrypt (OCTETSTR KB, OCTETSTR C, int keysize) {
	int Nr;

	switch (keysize) {
		case 256:
			Nr = 14;
			break;
		case 192:
			Nr = 12;
			break;
		default:
			Nr = 10;
	}

	OCTETSTR M;
	OCTETSTR::size_type cLen = C.size();
	if (cLen%16) {
		throw borzoiException ("AES_CBC_IV0_Decrypt: cLen not a multiple of 16");
		return M;
	}
	if (cLen<16) {
		throw borzoiException ("AES_CBC_IV0_Decrypt: cLen < 16");
		return M;
	}
	long k =  ((cLen+1)/16);

	OCTETSTR T(cLen);
	OCTETSTR U (16);
	OCTETSTR CI (16);
	for (long i=1; i<=k; i++) {
		for (long n=0; n<16; n++) {
			CI[n] = C[(i-1)*16+n];
		}
		U = Dec (KB, CI, Nr, keysize);
		for (int j=0; j<16; j++) {
			if (i>1)
				T[(i-1)*16+j] = U[j] ^ C[(i-2)*16+j];
			else T[j] = U[j];
		}
	}

	OCTET padLen = T[(k*16)-1];
	if (padLen<1) {
		throw borzoiException ("AES_CBC_IV0_Decrypt: padLen < 1");
		return M;
	}
	if (padLen>16) {
		throw borzoiException ("AES_CBC_IV0_Decrypt: padLen > 16");
		return M;
	}
	for (OCTET l=1; l<padLen; l++) {
		if (T[(k*16)-1-l] != padLen) {
			throw borzoiException ("AES_CBC_IV0_Decrypt: OCTET != padLen");
			return M;
		}
	}
	M = OCTETSTR (T.begin(), T.end()-padLen);

	return M;
}

