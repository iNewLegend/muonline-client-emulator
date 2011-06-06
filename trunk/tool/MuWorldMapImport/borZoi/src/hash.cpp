/* 
   borZoi - An ELLiptic Curve Cryptography Library

   Copyright (C) 2001 Anthony Mulcahy

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it wiLL be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   \You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  

*/


#include <iomanip>
#include "borzoi.h"


// return a padded vector
std::vector<unsigned int> pad (OCTETSTR& in) {
	unsigned long l = in.size()*8;
	unsigned long k = (448-(l+1))%512;

	std::vector<unsigned int> out((l+1+k+64)/32);
	unsigned long i = 0;
	unsigned long j = 0;
	unsigned int shift = 0x1000000;
	out[i] = 0L;
	while (j<in.size ()) {
		out[i] |= in[j]*shift;
		shift /=0x100;
		if (!shift) {
			i++;
			shift = 0x1000000;
			out[i] = 0L;
		}
		j++;
	}
	out[i] |= 0x80L*shift;
	while (i<out.size ()-1) {
		i++;
		out[i] = 0;
	}
	out[i] |= in.size()*8;
	return out;
}

unsigned int f (unsigned int B, unsigned int C, unsigned int D, int t) {
	unsigned int f;

	if (t < 20) {
		f = (B & C) | (~B & D); 
	} else if (t<40) {
		f = B ^ C ^D;
	}else if (t<60) {
		f = (B & C) | (B & D) | (C & D);
	} else {
		f = B ^ C ^D;
	}

	return f;
}


unsigned int K (int t) {
	unsigned int k;

	if (t < 20) {
		k = 0x5a827999;
	} else if (t<40) {
		k = 0x6ed9eba1;
	}else if (t<60) {
		k = 0x8f1bbcdc;
	} else {
		k = 0xca62c1d6;
	}

	return k;
}


unsigned int S (unsigned int X, int n) {
	return (((X<<n) | (X>>(32-n))&0xffffffff));
}


// return the SHA1 hash of octet string x in OCTETSTR form
// Based on FIPS PUB 180-1
OCTETSTR SHA1 (const OCTETSTR& x) {
	OCTETSTR str = x;
	OCTETSTR hashstr(20);

	std::vector<unsigned int> M = pad (str);
	std::vector<unsigned int> W (80);

	unsigned int H0=0x67452301;
	unsigned int H1=0xefcdab89;
	unsigned int H2=0x98badcfe;
	unsigned int H3=0x10325476;
	unsigned int H4=0xc3d2e1f0;
	int t;
	unsigned int A;
	unsigned int B;
	unsigned int C;
	unsigned int D;
	unsigned int E;
	unsigned int TEMP;
	for (int n=0; n<M.size (); n+=16) {
		for (t=0; t<16; t++) {
			W[t] = M[t+n];
		}
		for (t=16; t<80; t++) {
			W[t] = S (W[t-3] ^ W[t-8] ^ W[t-14] ^ W[t-16], 1);
		}
		A=H0; B=H1; C=H2; D=H3; E=H4;
		for (t=0; t<80; t++) {
			TEMP = S(A,5) + f (B, C, D, t) + E + W[t] +K(t);
			E=D; D=C; C=S(B,30); B=A; A=TEMP;
		}
		H0+=A; H1+=B; H2+=C; H3+=D; H4+=E;
	}

	hashstr[0] = H4&0xff; hashstr[1] = (H4>>8)&0xff; hashstr[2] = (H4>>16)&0xff; hashstr[3] = (H4>>24)&0xff;
	hashstr[4] = H3&0xff; hashstr[5] = (H3>>8)&0xff; hashstr[6] = (H3>>16)&0xff; hashstr[7] = (H3>>24)&0xff;
	hashstr[8] = H2&0xff; hashstr[9] = (H2>>8)&0xff; hashstr[10] = (H2>>16)&0xff; hashstr[11] = (H2>>24)&0xff;
	hashstr[12] = H1&0xff; hashstr[13] = (H1>>8)&0xff; hashstr[14] = (H1>>16)&0xff; hashstr[15] = (H1>>24)&0xff;
	hashstr[16] = H0&0xff; hashstr[17] = (H0>>8)&0xff; hashstr[18] = (H0>>16)&0xff; hashstr[19] = (H0>>24)&0xff;
	//BigInt hash = OS2IP (hashstr); 

	return hashstr;
}


// return the SHA1 hash of string x in OCTETSTR form
// Based on FIPS PUB 180-1
OCTETSTR SHA1 (const std::string& x) {
	OCTETSTR os(x.size ());
	for (int i=0; i<x.size (); i++) {
		os[i] = x[i];
	}

	return SHA1 (os);
}


// return the SHA1 hash of BigInt x in OCTETSTR form
// Based on FIPS PUB 180-1
OCTETSTR SHA1 (BigInt x) {
	return SHA1 (I2OSP(x));
}


