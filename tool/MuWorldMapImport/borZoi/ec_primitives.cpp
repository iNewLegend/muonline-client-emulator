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


EC_Domain_Parameters& EC_Domain_Parameters::operator= (const EC_Domain_Parameters& dp) {
	if (this != &dp) {
		m=dp.m;
		basis = dp.basis;
		trinomial_k=dp.trinomial_k;
		pentanomial_k3=dp.pentanomial_k3;
		pentanomial_k2=dp.pentanomial_k2;
		pentanomial_k1=dp.pentanomial_k1;
		a = dp.a;
		b = dp.b;
		r = dp.r;
		G = dp.G;
		k = dp.k;
	}
	return *this;
}


bool MOV_Condition (unsigned long m, BigInt& r) {
	unsigned long B;

	if (m<=142)
		B=6;
	else if (m<=165)
		B=7;
	else if (m<=186)
		B=8;
	else if (m<=206)
		B=9;
	else if (m<=226)
		B=10;
	else if (m<=244)
		B=11;
	else if (m<=262)
		B=12;
	else if (m<=280)
		B=13;
	else if (m<=297)
		B=14;
	else if (m<=313)
		B=15;
	else if (m<=330)
		B=16;
	else if (m<=346)
		B=17;
	else if (m<=361)
		B=18;
	else if (m<=376)
		B=19;
	else if (m<=391)
		B=20;
	else if (m<=406)
		B=21;
	else if (m<=420)
		B=22;
	else if (m<=434)
		B=23;
	else if (m<=448)
		B=24;
	else if (m<=462)
		B=25;
	else if (m<=475)
		B=26;
	else if (m<=488)
		B=27;
	else if (m<=501)
		B=28;
	else B=29;
	
	BigInt t(1);
	BigInt q = BigInt(1)<<m;
	for (unsigned long i=1; i<=B; i++) {
		t = MulMod (t, q, r);
		if (t== BigInt (1))
			return false;
	}

	return true;
}

// A partial implementation (steps 6.4 to 7) of A.16.8 in P1363
//
// 6.4 Check that b != 0 in GF (2 m ).
// 6.5 Check that G != O. Let G = (x, y).
// 6.6 Check that x and y are elements of GF (2 m ).
// 6.7 Check that y^2 + xy = x^3 + ax^2 + b in GF (2 m ).
// 6.8 Check that rG = O.
// 6.9 Check that the curve is not an instance of the following excluded case:
// 6.9.1 If the output of the algorithm given in A.12.1 is gFalse,h then 
//       the curve is excluded because it is subject to the MOV reduction 
//       attack described in [MOV93].
// 7. Output "True" if the checks given in Steps 4 through 6 work, and 
//    "False" otherwise.
bool EC_Domain_Parameters::valid () {
	if (b.isZero())
		return false;
	if (G.x.isZero()&&G.y.isZero())
		return false;
	if ((G.y*G.y+G.x*G.y)!=(G.x*G.x*G.x+a*G.x*G.x+b))
		return false;
	Curve C(a, b);
	Point P=C.mul (r, G);
	if (!(P.x.isZero()&&P.y.isZero()))
		return false;
	if (!MOV_Condition(m, r))
		return false;
	return true;
}


std::ostream& EC_Domain_Parameters::put (std::ostream&s) const {
	s << "m: " << m << std::endl;
	switch (basis) {
		case 1:
			s << "Gaussian Basis (" << basis <<  ')' << std::endl;
			break;
		case 2:
			s << "Trinomial Basis (" << basis <<  ')' << std::endl;
			s << "->trinomial_k: " << trinomial_k << std::endl;
			break;
		case 3:
			s << "Pentanomial Basis (" << basis <<  ')' << std::endl;
			s << "->k3: " << pentanomial_k3 << std::endl;
			s << "->k2: " << pentanomial_k2 << std::endl;
			s << "->k1: " << pentanomial_k1 << std::endl;
			break;
		default:
			s << "Error Basis Undefined (" << basis << ')' << std::endl;
	}
	s << "a: " << a << std::endl << "b: " << b << std::endl;
	s << "r: " << r << std::endl  << "G: " << G << std::endl;
	s << "k: " << k << std::endl;

	return s;
}


// Throws borzoiException if the point P is zero
F2M ECSVDP_DH (const EC_Domain_Parameters& dp, const BigInt& s, const Point& Wi) {
	Curve E (dp.a, dp.b);
	Point P = E.mul (s, Wi);
	if (P.isZero ())
		throw borzoiException ("ECSVDP_DH: P is zero");
	return P.x;
}


// Key Derivation Function 2
// Uses the SHA-1 hash function
// Throws borzoiException for invalid output lengths
OCTETSTR KDF2 (OCTETSTR Z, OCTETSTR::size_type oLen, OCTETSTR P) {
        //if (oLen > 160*((2<<31)-1)) // 160 * (2^32-1)
        //	throw borzoiException ("KDF2: oLen > hbits * (2^32-1) bits");
	if (oLen > 20*((2<<31)-1)) // 160/8 * (2^32-1)
		throw borzoiException ("KDF2: oLen > hbits/8 * (2^32-1) bits");
	OCTETSTR K;
	OCTETSTR CB (1);
	OCTET cThreshold = (oLen+19)/20; // 20 = 160/8
	for (OCTET i=1; i<=cThreshold; i++) {
		CB[0] = (OCTET)i;
		K = K || SHA1 (Z || CB || P);
	}
	K.resize (oLen);
	return K;
}


// HMAC message authentication code
// Uses the SHA-1 hash function
// The tag length is equal to 160 bits
OCTETSTR MAC1 (OCTETSTR K, OCTETSTR M) {
	OCTETSTR KK;
	// SHA1 Blocksize B = 512
	if (K.size() > (8*512))
		KK = SHA1 (K); // kkLen = 20 octets, 160 bits
	else KK = K;
	OCTETSTR P (512-KK.size(), 0x0);
	OCTETSTR K0 = KK || P;
	OCTETSTR iPad (512, 0x36);
	OCTETSTR oPad (512, 0x54);
	OCTETSTR H = SHA1 ((K0^iPad) || M);
	OCTETSTR HH = SHA1 ((K0^oPad) || H);

	return HH;
}


