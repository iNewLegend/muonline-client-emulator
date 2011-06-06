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

mp_int f2m_modulus;

// Big integer wrapper class
BigInt::BigInt () {
        mp_init(&mp);
}


BigInt::BigInt (OCTET o) {
        mp_init(&mp);
        mp_set(&mp, (long)o);
}


BigInt::~BigInt () {
        mp_clear(&mp);
}


BigInt::BigInt (const BigInt& a) { 
        mp_init (&mp);
        mp_copy (&(const_cast<BigInt&>(a)).mp, &mp);
}


BigInt& BigInt::operator= (const BigInt& a) { 
	if (this != &a) {
        	mp_copy (&(const_cast<BigInt&>(a)).mp, &mp);
	}
	return *this;
}


BigInt& BigInt::operator+= (const BigInt& a) {
        mp_add (&mp, &(const_cast<BigInt&>(a)).mp, &mp);
	return *this;
} 


BigInt& BigInt::operator-= (const BigInt& a) {
        mp_sub (&mp, &(const_cast<BigInt&>(a)).mp, &mp);
	return *this;
} 


BigInt& BigInt::operator*= (const BigInt& a) {
        mp_mul (&mp, &(const_cast<BigInt&>(a)).mp, &mp);
	return *this;
} 


BigInt& BigInt::operator/= (const BigInt& a) {
	mp_int r; mp_init (&r);
        mp_div (&mp, &(const_cast<BigInt&>(a)).mp, &mp, &r);
	mp_clear(&r);
	return *this;
} 


BigInt& BigInt::operator%= (const BigInt& a) {
	mp_int q; mp_init (&q);
        mp_div (&mp, &(const_cast<BigInt&>(a)).mp, &q, &mp);
        mp_clear(&q);
	return *this;
} 


BigInt& BigInt::operator&= (const BigInt& a) {
        mp_and (&mp, &(const_cast<BigInt&>(a)).mp, &mp);
	return *this;
} 


BigInt& BigInt::operator|= (const BigInt& a) {
        mp_or (&mp, &(const_cast<BigInt&>(a)).mp, &mp);
	return *this;
} 


BigInt& BigInt::operator^= (const BigInt& a) {
        mp_xor (&mp, &(const_cast<BigInt&>(a)).mp, &mp);
	return *this;
} 


BigInt BigInt::operator>>= (unsigned short i) {
	mp_right_shift (&mp, i);
	return *this;
} 


BigInt BigInt::operator<<= (unsigned short i) {
	mp_left_shift (&mp, i);
	return *this;
} 


int BigInt::isZero () {
	return !mp_cmp_int (&mp, 0);
} 


int BigInt::getBit (unsigned short k) {
	return mp_getbit (&mp,k);
}


int BigInt::getDigit (unsigned short k) {
	return ((*this>>(k*4)).toOctet()&0xf);
}


long BigInt::numBits () {
	return mp_msb(&mp)+1;
} 


OCTET BigInt::toOctet () {
	return mp_tooctet(&mp);
} 


std::ostream& BigInt::put (std::ostream&s) {
	int size=mp_mag_size (&mp);
	char *str = (char *)calloc (size, sizeof(char));
	mp_tomag(&mp, str);
	if (str==NULL)
		return s;
	if (size==0)
		s<<'0'<<'0';
	for (int i=0; i<size; i++) {
		s.width(2);
		s.fill('0');
		s<<std::hex<<(int)(str[i]&0xff);
	}
	free (str);
	return s;
}


int operator==(const BigInt& a, const BigInt& b) {
	return !mp_cmp (&(const_cast<BigInt&>(a)).mp, &(const_cast<BigInt&>(b)).mp);
}


int operator<(const BigInt& a, const BigInt& b) {
	return (mp_cmp (&(const_cast<BigInt&>(a)).mp, &(const_cast<BigInt&>(b)).mp)<0);
}


int operator>(const BigInt& a, const BigInt& b) {
	return (mp_cmp (&(const_cast<BigInt&>(a)).mp, &(const_cast<BigInt&>(b)).mp)>0);
}


BigInt InvMod (BigInt a, BigInt n) {
	BigInt x;
	mp_int amp=a.mp;
	mp_int nmp=n.mp;
	mp_invmod (&amp, &nmp, &x.mp);
	return x;
}


BigInt MulMod (BigInt a, BigInt b, BigInt n) {
	BigInt x;
	mp_int amp=a.mp;
	mp_int bmp=b.mp;
	mp_int nmp=n.mp;
	mp_mulmod (&amp, &bmp, &nmp, &x.mp);
	return x;
}

BigInt GenRandom (unsigned long n) {
	BigInt x;
	mp_gen_random (&x.mp, n);
	return x;
}


// Big integer wrapper class
F2X::F2X () {
        mp_init(&mp);
}


F2X::F2X (OCTET l) {
        mp_init(&mp);
        mp_set(&mp, l);
}


F2X::~F2X () {
        mp_clear(&mp);
}


F2X::F2X (const F2X& a) { 
        mp_init (&mp);
        mp_copy (&(const_cast<F2X&>(a)).mp, &mp);
}


F2X& F2X::operator= (const F2X& a) { 
	if (this != &a) {
        	mp_copy (&(const_cast<F2X&>(a)).mp, &mp);
	}
	return *this;
}


OCTET F2X::toOctet () {
	return mp_tooctet(&mp);
} 


F2X& F2X::operator|= (const F2X& a) {
        mp_or (&mp, &(const_cast<F2X&>(a)).mp, &mp);
	return *this;
} 


F2X& F2X::operator+= (const F2X& a) {
        mp_xor (&mp, &(const_cast<F2X&>(a)).mp, &mp);
	return *this;
} 


F2X& F2X::operator-= (const F2X& a) {
        mp_xor (&mp, &(const_cast<F2X&>(a)).mp, &mp);
	return *this;
} 


F2X& F2X::operator*= (const F2X& a) {
        F2x_multiply (&mp, &(const_cast<F2X&>(a)).mp, &mp);
	return *this;
} 


F2X& F2X::operator/= (const F2X& a) {
        poly_F2x_div (&mp, &(const_cast<F2X&>(a)).mp, &mp, NULL);
	return *this;
} 


F2X& F2X::operator%= (const F2X& a) {
        poly_F2x_div (&mp, &(const_cast<F2X&>(a)).mp, NULL, &mp);
	return *this;
} 


F2X F2X::operator>>= (unsigned short i) {
	mp_right_shift (&mp, i);
	return *this;
} 


F2X F2X::operator<<= (unsigned short i) {
	mp_left_shift (&mp, i);
	return *this;
} 


int F2X::isZero () {
	return !mp_cmp_int (&mp, 0);
} 


int F2X::getCoeff (unsigned short k) {
	return mp_getbit (&mp,k);
}


void F2X::setCoeff (unsigned short k, unsigned short i) {
	*this |= (F2X(i&0x1))<<k;
}


long F2X::numBits () {
	return mp_msb(&mp)+1;
} 


std::ostream& F2X::put (std::ostream&s) {
	int size=mp_mag_size (&mp);
	char *str = (char *)calloc (size, sizeof(char));
	mp_tomag(&mp, str);
	if (str==NULL)
		return s;
	if (size==0)
		s<<'0'<<'0';
	for (int i=0; i<size; i++) {
		s.width(2);
		s.fill('0');
		s<<std::hex<<(int)(str[i]&0xff);
	}
	free (str);
	return s;
}


int operator==(const F2X& a, const F2X& b) {
	return !mp_cmp (&(const_cast<F2X&>(a)).mp, &(const_cast<F2X&>(b)).mp);
}


F2X InvMod (F2X a, F2X n) {
	F2X x;
	mp_int amp=a.mp;
	mp_int nmp=n.mp;
	F2m_inverse (&amp, &nmp, &x.mp);
	return x;
}


F2X MulMod (F2X a, F2X b, F2X n) {
	F2X x;
	mp_int amp=a.mp;
	mp_int bmp=b.mp;
	mp_int nmp=n.mp;
	poly_F2x_mulmod (&amp, &bmp, &nmp, &x.mp);
	return x;
}

F2X Trinomial (unsigned short k3, unsigned short k2, unsigned short k1) {
	F2X x;
	F2X one(1);
	x = (one<<k3)+(one<<k2)+(one<<k1);
	return x;
}

F2X Pentanomial (unsigned short k5, unsigned short k4, unsigned short k3, unsigned short k2, unsigned short k1) {
	F2X x;
	F2X one(1);
	x = (one<<k5)+(one<<k4)+(one<<k3)+(one<<k2)+(one<<k1);
	return x;
}


// Big integer wrapper class
F2M::F2M () {
        mp_init(&mp);
}


F2M::F2M (OCTET o) {
        mp_init(&mp);
        mp_set(&mp, o);
}


F2M::F2M (F2X x) {
        mp_init(&mp);
		x %= getModulus();
        mp_copy (&x.mp, &mp);
}


F2M::~F2M () {
        mp_clear(&mp);
}


F2M::F2M (const F2M& a) { 
        mp_init (&mp);
        mp_copy (&(const_cast<F2M&>(a)).mp, &mp);
}


F2M& F2M::operator= (const F2M& a) { 
	if (this != &a) {
		mp_copy (&(const_cast<F2M&>(a)).mp, &mp);
	}
	return *this;
}


F2M& F2M::operator+= (const F2M& a) {
        mp_xor (&mp, &(const_cast<F2M&>(a)).mp, &mp);
	return *this;
} 


F2M& F2M::operator-= (const F2M& a) {
        mp_xor (&mp, &(const_cast<F2M&>(a)).mp, &mp);
	return *this;
} 


F2M& F2M::operator*= (const F2M& a) {
        poly_F2x_mulmod (&mp, &(const_cast<F2M&>(a)).mp, &f2m_modulus, &mp);
	return *this;
} 


F2M F2M::sqr () {
	F2M x;
    F2m_square (&mp, &f2m_modulus, &x.mp);
	return x;
} 


F2M& F2M::operator>>= (unsigned short i) {
	mp_right_shift (&mp, i);
	return *this;
} 


F2M& F2M::operator<<= (unsigned short i) {
	mp_left_shift (&mp, i);
	return *this;
} 


F2M F2M::inverse () {
	F2M x;
	//poly_F2x_invmod (&mp, &f2m_modulus, &x.mp);
	F2m_inverse (&mp, &f2m_modulus, &x.mp);
	return x;
}


int F2M::isZero () {
	return !mp_cmp_int (&mp, 0);
} 


int F2M::getCoeff (unsigned short k) {
	return mp_getbit (&mp,k);
}


void F2M::setCoeff (unsigned short k, unsigned short i) {
	*this |= (F2M(i&0x1))<<k;
}


long F2M::numBits () {
	return mp_msb(&mp)+1;
} 


F2M& F2M::operator|= (const F2M& a) {
        mp_or (&mp, &(const_cast<F2M&>(a)).mp, &mp);
	return *this;
} 


OCTET F2M::toOctet () {
	return mp_tooctet(&mp);
} 


std::ostream& F2M::put (std::ostream&s) {
	int size=mp_mag_size (&mp);
	char *str = (char *)calloc (size, sizeof(char));
	mp_tomag(&mp, str);
	if (str==NULL)
		return s;
	if (size==0)
		s<<'0'<<'0';
	for (int i=0; i<size; i++) {
		s.width(2);
		s.fill('0');
		s<<std::hex<<(int)(str[i]&0xff);
	}
	free (str);
	return s;
}


int operator==(const F2M& a, const F2M& b) {
	return !mp_cmp (&(const_cast<F2M&>(a)).mp, &(const_cast<F2M&>(b)).mp);
}


F2M GenRandomF2M (unsigned long n) {
	F2M x;
	mp_gen_random (&x.mp, n);
	return x;
}


F2X getModulus () {
	F2X m;
        mp_copy (&f2m_modulus, &m.mp);
	return m;
} 


void setModulus (const F2X& m) {
        mp_copy (&(const_cast<F2X&>(m)).mp, &f2m_modulus);
} 


