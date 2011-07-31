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

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software Foundation,
   Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  

*/


#ifndef BORZOI_MATH_H
#define BORZOI_MATH_H

#include <iostream>
#include <string>
#include <vector>

#ifdef USE_NTL
#include <NTL/GF2E.h>
#include <NTL/ZZ.h>
#else
extern "C" {
#include "mpi.h"
}
#endif

extern "C" {
#include "rng.h"
}

typedef unsigned char OCTET;
typedef std::vector<OCTET> OCTETSTR;

// Big integer wrapper class
class BigInt {
private:
#ifdef USE_NTL
	ZZ zz;
#else
	mp_int mp;
#endif
public:
	BigInt ();
	BigInt (OCTET o);
	~BigInt ();

	BigInt (const BigInt& a);
	BigInt& operator= (const BigInt& a);
	BigInt& operator+= (const BigInt& a);
	BigInt& operator-= (const BigInt& a);
	BigInt& operator*= (const BigInt& a);
	BigInt& operator/= (const BigInt& a);
	BigInt& operator%= (const BigInt& a);
	BigInt& operator&= (const BigInt& a);
	BigInt& operator|= (const BigInt& a);
	BigInt& operator^= (const BigInt& a);
	BigInt operator>>= (unsigned short i);
	BigInt operator<<= (unsigned short i);

	int isZero ();
	int getBit (unsigned short k);
	int getDigit (unsigned short k);
	long numBits ();
	OCTET toOctet ();
	std::ostream& put (std::ostream&s);

	friend int operator==(const BigInt& a, const BigInt& b);
	friend BigInt InvMod (BigInt a, BigInt n);
	friend BigInt MulMod (BigInt a, BigInt b, BigInt n);
	friend BigInt GenRandom (unsigned long n);
	friend int operator<(const BigInt& a, const BigInt& b);
	friend int operator>(const BigInt& a, const BigInt& b);
};


int operator==(const BigInt& a, const BigInt& b);

BigInt InvMod (BigInt a, BigInt n);

BigInt MulMod (BigInt a, BigInt b, BigInt n);

BigInt GenRandom (unsigned long n);

int operator<(const BigInt& a, const BigInt& b);

int operator>(const BigInt& a, const BigInt& b);

inline int operator!=(const BigInt& a, const BigInt& b) { return !(a==b); }

inline int operator<=(const BigInt& a, const BigInt& b) { return (a<b)||(a==b); }

inline int operator>=(const BigInt& a, const BigInt& b) { return (a>b)||(a==b); }

inline BigInt operator+ (const BigInt& a, const BigInt& b) { BigInt c; c=a; c+=b; return c; } 

inline BigInt operator- (const BigInt& a, const BigInt& b) { BigInt c=a; c-=b; return c; } 

inline BigInt operator* (const BigInt& a, const BigInt& b) { BigInt c=a; c*=b; return c; } 

inline BigInt operator/ (const BigInt& a, const BigInt& b) { BigInt c=a; c/=b; return c; } 

inline BigInt operator% (const BigInt& a, const BigInt& b) { BigInt c=a; c%=b; return c; } 

inline BigInt operator& (const BigInt& a, const BigInt& b) { BigInt c=a; c&=b; return c; } 

inline BigInt operator| (const BigInt& a, const BigInt& b) { BigInt c=a; c|=b; return c; } 

inline BigInt operator^ (const BigInt& a, const BigInt& b) { BigInt c=a; c^=b; return c; } 

inline BigInt operator>>(const BigInt& a, unsigned short n) {BigInt b=a; b>>=n; return b;}

inline BigInt operator<<(const BigInt& a, unsigned short n) {BigInt b=a; b<<=n; return b;}

inline BigInt concat (BigInt a, BigInt b) { BigInt c=a; c<<=b.numBits(); c|=b; return c; }

inline std::ostream& operator<< (std::ostream&s, BigInt z) { return z.put (s); }

class F2M;

// Polynomial over F2X wrapper class
class F2X {
private:
#ifdef USE_NTL
	GF2X x;
#else
	mp_int mp;
	F2X& operator|= (const F2X& a);
	OCTET toOctet ();
#endif
public:
	F2X ();
	F2X (OCTET l);
	~F2X ();

	F2X (const F2X& a);
	F2X& operator= (const F2X& a);
	F2X& operator+= (const F2X& a);
	F2X& operator-= (const F2X& a);
	F2X& operator*= (const F2X& a);
	F2X& operator/= (const F2X& a);
	F2X& operator%= (const F2X& a);
	F2X operator>>= (unsigned short i);
	F2X operator<<= (unsigned short i);

	int isZero ();
	int getCoeff (unsigned short k);
	void setCoeff (unsigned short k, unsigned short i);
	long numBits ();
	std::ostream& put (std::ostream&s);

	friend int operator==(const F2X& a, const F2X& b);
	friend F2X InvMod (F2X a, F2X n);
	friend F2X MulMod (F2X a, F2X b, F2X n);
	friend F2X getModulus (); 
	friend void setModulus (const F2X& m);
	friend class F2M; 
};


int operator==(const F2X& a, const F2X& b);

F2X InvMod (F2X a, F2X n);

F2X MulMod (F2X a, F2X b, F2X n);

F2X Trinomial (unsigned short k3, unsigned short k2, unsigned short k1);

F2X Pentanomial (unsigned short k5, unsigned short k4, unsigned short k3, unsigned short k2, unsigned short k1);

inline int operator!=(const F2X& a, const F2X& b) { return !(a==b); }


inline F2X operator+ (const F2X& a, const F2X& b) { F2X c; c=a; c+=b; return c; } 

inline F2X operator- (const F2X& a, const F2X& b) { F2X c=a; c-=b; return c; } 

inline F2X operator* (const F2X& a, const F2X& b) { F2X c=a; c*=b; return c; } 

inline F2X operator/ (const F2X& a, const F2X& b) { F2X c=a; c/=b; return c; } 

inline F2X operator% (const F2X& a, const F2X& b) { F2X c=a; c%=b; return c; } 

inline F2X operator>>(const F2X& a, unsigned short n) {F2X b=a; b>>=n; return b;}

inline F2X operator<<(const F2X& a, unsigned short n) {F2X b=a; b<<=n; return b;}

inline std::ostream& operator<< (std::ostream&s, F2X z) { return z.put (s); }


// Finite field wrapper class
class F2M {
private:
#ifdef USE_NTL
	GF2E m;
#else
	mp_int mp;
	F2M& operator|= (const F2M& a);
	OCTET toOctet ();
#endif
public:
	F2M ();
	F2M (OCTET o);
	F2M (F2X x);
	~F2M ();

	F2M (const F2M& a);
	F2M& operator= (const F2M& a);
	F2M& operator+= (const F2M& a);
	F2M& operator-= (const F2M& a);
	F2M& operator*= (const F2M& a);
	F2M sqr ();
	F2M& operator>>= (unsigned short i);
	F2M& operator<<= (unsigned short i);

	F2M inverse ();
	int isZero ();
	void setCoeff (unsigned short k, unsigned short i);
	int getCoeff (unsigned short k);
	long numBits ();
	std::ostream& put (std::ostream&s);

	friend F2X getModulus ();
	friend void setModulus (const F2X& m);
	friend int operator==(const F2M& a, const F2M& b);
	friend F2M GenRandomF2M (unsigned long n);
};


F2X getModulus ();

void setModulus (const F2X& m);

int operator==(const F2M& a, const F2M& b);

F2M GenRandomf2m (unsigned long n);

inline int operator!=(const F2M& a, const F2M& b) { return !(a==b); }

inline F2M operator+ (const F2M& a, const F2M& b) { F2M c; c=a; c+=b; return c; } 

inline F2M operator- (const F2M& a, const F2M& b) { F2M c=a; c-=b; return c; } 

inline F2M operator* (const F2M& a, const F2M& b) { F2M c=a; c*=b; return c; } 

inline F2M operator>>(const F2M& a, unsigned short n) {F2M b=a; b>>=n; return b;}

inline F2M operator<<(const F2M& a, unsigned short n) {F2M b=a; b<<=n; return b;}

inline std::ostream& operator<< (std::ostream&s, F2M z) { return z.put (s); }


void str_to_F2M (const std::string& s, F2M& f);
void str_to_BigInt (const std::string& s, BigInt& z, int base);
inline F2M to_F2M (const std::string& s) {F2M f;str_to_F2M(s,f);return f;}
inline BigInt decto_BigInt (const std::string& s) {BigInt z;str_to_BigInt(s,z, 10);return z;}
inline BigInt hexto_BigInt (const std::string& s) {BigInt z;str_to_BigInt(s,z, 0x10);return z;}

#endif	// BORZOI_MATH_H
