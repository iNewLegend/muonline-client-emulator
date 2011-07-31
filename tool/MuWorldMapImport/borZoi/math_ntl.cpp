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


#include "borzoi_math.h"

// Big integer wrapper class
BigInt::BigInt () {
	clear (zz);
}


BigInt::BigInt (OCTET o) {
	zz=to_ZZ(o);
}


BigInt::~BigInt () {
}


BigInt::BigInt (const BigInt& a) {
	zz = a.zz;
}


BigInt& BigInt::operator= (const BigInt& a) { 
	if (this != &a)
		zz = a.zz; 
	return *this;
}


BigInt& BigInt::operator+= (const BigInt& a) {
	zz+=a.zz;
	return *this;
} 


BigInt& BigInt::operator-= (const BigInt& a) {
	zz-=a.zz;
	return *this;
} 


BigInt& BigInt::operator*= (const BigInt& a) {
	zz*=a.zz;
	return *this;
} 


BigInt& BigInt::operator/= (const BigInt& a) {
	zz/=a.zz;
	return *this;
} 


BigInt& BigInt::operator%= (const BigInt& a) {
	zz%=a.zz;
	return *this;
} 


BigInt& BigInt::operator&= (const BigInt& a) {
	zz&=a.zz;
	return *this;
} 


BigInt& BigInt::operator|= (const BigInt& a) {
	zz|=a.zz;
	return *this;
} 


BigInt& BigInt::operator^= (const BigInt& a) {
	zz^=a.zz;
	return *this;
} 


BigInt BigInt::operator>>= (unsigned short i) {
	zz>>=i;
	return *this;
} 


BigInt BigInt::operator<<= (unsigned short i) {
	zz<<=i;
	return *this;
} 


int BigInt::isZero () {
	return IsZero (zz);
} 


int BigInt::getBit (unsigned short k) {
	return bit (zz, k);
}


int BigInt::getDigit (unsigned short k) {
	return digit (zz, k);
}


long BigInt::numBits () {
	return NumBits (zz);
} 


OCTET BigInt::toOctet () {
	return (OCTET)to_int(zz&0xff);
} 


std::ostream& BigInt::put (std::ostream&s) {
	s << zz;
	return s;
}


int operator==(const BigInt& a, const BigInt& b) {
	return (a.zz==b.zz);
}


int operator<(const BigInt& a, const BigInt& b) {
	return (a.zz<b.zz);
}


int operator>(const BigInt& a, const BigInt& b) {
	return (a.zz>b.zz);
}


BigInt InvMod (BigInt a, BigInt n) {
	BigInt x;
	InvMod (x.zz, a.zz, n.zz);
	return x;
}


BigInt MulMod (BigInt a, BigInt b, BigInt n) {
	BigInt x;
	MulMod (x.zz, a.zz, b.zz, n.zz);
	return x;
}

BigInt GenRandom (unsigned long n) {
	BigInt x;
	while (NumBits (x.zz) < n) {
		x.zz <<= 32; x.zz |= gen_random ();
	}
	return x;
}


// Big integer wrapper class
F2X::F2X () {
	clear (x);
}


F2X::F2X (OCTET l) {
	int ix=0;
	clear (x);
	while (l) {
		SetCoeff (x, ix, to_GF2(l));
		l>>=1;
		ix++;
	}
}


F2X::~F2X () {
}


F2X::F2X (const F2X& a) { 
	x = a.x;
}


F2X& F2X::operator= (const F2X& a) { 
	if (this != &a) {
		x = a.x;
	}
	return *this;
}


F2X& F2X::operator+= (const F2X& a) {
	x+=a.x;
	return *this;
} 


F2X& F2X::operator-= (const F2X& a) {
	x-=a.x;
	return *this;
} 


F2X& F2X::operator*= (const F2X& a) {
	x*=a.x;
	return *this;
} 


F2X& F2X::operator/= (const F2X& a) {
	x/=a.x;
	return *this;
} 


F2X& F2X::operator%= (const F2X& a) {
	x%=a.x;
	return *this;
} 


F2X F2X::operator>>= (unsigned short i) {
	x>>=i;
	return *this;
} 


F2X F2X::operator<<= (unsigned short i) {
	x<<=i;
	return *this;
} 


int F2X::isZero () {
	return IsZero (x);
} 


int F2X::getCoeff (unsigned short k) {
	return rep(coeff (x, k));
}


void F2X::setCoeff (unsigned short k, unsigned short i) {
	SetCoeff (x, k, to_GF2(i));
}


long F2X::numBits () {
	return NumBits (x);
} 


std::ostream& F2X::put (std::ostream&s) {
	GF2X::HexOutput = 1;
	s << x;
	return s;
}


int operator==(const F2X& a, const F2X& b) {
	return (a.x==b.x);
}


F2X InvMod (F2X a, F2X n) {
	F2X x;
	x.x = InvMod (a.x, n.x);
	return x;
}


F2X MulMod (F2X a, F2X b, F2X n) {
	F2X x;
	x.x = MulMod (a.x, b.x, n.x);
	return x;
}

F2X Trinomial (unsigned short k3, unsigned short k2, unsigned short k1) {
	F2X x;
	x.setCoeff (k3, 1); x.setCoeff (k2, 1); x.setCoeff (k1, 1);
	return x;
}

F2X Pentanomial (unsigned short k5, unsigned short k4, unsigned short k3, unsigned short k2, unsigned short k1) {
	F2X x;
	x.setCoeff (k5, 1); x.setCoeff (k4, 1);
	x.setCoeff (k3, 1); x.setCoeff (k2, 1); x.setCoeff (k1, 1);
	return x;
}


// 2^m Galois Field Wrapper Class
F2M::F2M () {
	clear (m);
}


F2M::F2M (OCTET o) {
	int ix=0;
	GF2X x;
	clear (x);
	while (o) {
		SetCoeff (x, ix, to_GF2(o));
		o>>=1;
		ix++;
	}
	m=to_GF2E(x);
}


F2M::F2M (F2X x) {
	m=to_GF2E(x.x);
}


F2M::~F2M () {
}


F2M::F2M (const F2M& a) { 
	m = a.m;
}


F2M& F2M::operator= (const F2M& a) { 
	if (this != &a)
		m = a.m;
	return *this;
}


F2M& F2M::operator+= (const F2M& a) {
	m+=a.m;
	return *this;
} 


F2M& F2M::operator-= (const F2M& a) {
	m-=a.m;
	return *this;
} 


F2M& F2M::operator*= (const F2M& a) {
	m*=a.m;
	return *this;
} 


F2M F2M::sqr () {
	return (*this)*(*this);
} 


F2M& F2M::operator>>= (unsigned short i) {
	GF2X temp = rep(m);
	temp >>= i;
	m = to_GF2E(temp);
	return *this;
} 


F2M& F2M::operator<<= (unsigned short i) {
	GF2X temp = rep(m);
	temp <<= i;
	m = to_GF2E(temp);
	return *this;
} 


F2M F2M::inverse () {
	F2M x;
	x.m = inv (m);
	return x;
}


int F2M::isZero () {
	return IsZero (m);
} 


int F2M::getCoeff (unsigned short k) {
	return rep(coeff (rep(m), k));
}


void F2M::setCoeff (unsigned short k, unsigned short i) {
	GF2X rep = m.LoopHole ();
	SetCoeff (rep, i);
}


long F2M::numBits () {
	return NumBits (rep(m));
} 


std::ostream& F2M::put (std::ostream&s) {
	GF2X::HexOutput = 1;
	s << rep(m);
	return s;
}


int operator==(const F2M& a, const F2M& b) {
	return (a.m==b.m);
}


F2M GenRandomF2M (unsigned long n) {
	F2M x;
	GF2X temp;
	while (NumBits (temp) < n) {
		temp <<= 32; temp += gen_random ();
	}
	x.m = to_GF2E (temp);
	return x;
}


F2X getModulus () {
	F2X modulus;
	modulus.x = GF2E::modulus();
	return modulus;
} 


void setModulus (const F2X& m) {
	GF2E::init (const_cast<F2X&>(m).x); 
} 


