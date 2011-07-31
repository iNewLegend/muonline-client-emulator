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


BigInt OS2IP (OCTETSTR o) {
	BigInt i, result;
	for (long j=o.size()-1; j>=0; j--) {
		i<<=8;
		i |= BigInt (o[j]);
	}

	return i;
}

OCTETSTR I2OSP (BigInt i) {
	BigInt mask (0xff);
	OCTETSTR o(i.numBits()/8+1);
	for (OCTETSTR::size_type j=0; j<o.size(); j++) {
		o[j] = (i&mask).toOctet();
		i>>=8;
	}

	return o;
}

OCTETSTR BS2OSP (F2M b) {
	long size = (getModulus().numBits()-1)/8;
	if ((getModulus().numBits()-1)%8 > 0)
		size++;
	OCTETSTR o(size);
	for (long i=0; i<b.numBits(); i++) {
		o[size-1-i/8] |= (b.getCoeff(i)<<(i%8));
	}
	return o;
}


F2M I2FEP (BigInt z) {
	F2X x;
	long size=z.numBits();
	for (long i=0; i<size; i++) {
		if (z.getBit (i))
			x.setCoeff (i, 1);

	}
	return F2M (x);
}


OCTETSTR operator|| (const OCTETSTR& a, const OCTETSTR& b) { // concatenate
	OCTETSTR c (a.size()+b.size());
	for (int i=0; i<a.size();i++) {
		c[i] = a[i];
	}
	for (int j=0; j<b.size();j++) {
		c[j+a.size()] = b[j];
	}
	return c;
}

OCTETSTR operator^ (OCTETSTR a, OCTETSTR b) {
	int i;
	if (b.size()>a.size()) {
		OCTETSTR tempa(b.size());
		for (i=0; i<a.size();i++) {
			tempa[i] = a[i];
		}
		a = tempa;
	} else {
		OCTETSTR tempb(a.size());
		for (i=0; i<b.size();i++) {
			tempb[i] = b[i];
		}
		b = tempb;
	}
	OCTETSTR c (a.size());
	for (i=0; i<a.size();i++) {
		c[i] = a[i]^b[i];
	}
	return c;
}


void str_to_F2M (const std::string& s, F2M& f) {
	F2X x;
	std::string::const_iterator p = s.end ();
	long val;
	unsigned short i = 0;
	while (p > s.begin ()) {
		p--;
		if (!isspace (*p)) {
			//val = strtoul ((std::string (p, 1)).c_str (), NULL, 0x10);
			val = strtoul ((std::string (p, p+1)).c_str (), NULL, 0x10);
			if (val&1)
				x.setCoeff (i, 1);
			i++;
			if (val&2)
				x.setCoeff (i, 1);
			i++;
			if (val&4)
				x.setCoeff (i, 1);
			i++;
			if (val&8)
				x.setCoeff (i, 1);
			i++;
		}

	}
	f = F2M (x);
}


void str_to_BigInt (const std::string& s, BigInt& z, int base) {
	z = BigInt(0);
	std::string::const_iterator p = s.begin();
	long i = 0;
	while (p < s.end ()) {
		if (!isspace (*p)) {
			z *= BigInt (base);
			//z += BigInt (strtol ((std::string (p, 1)).c_str (), NULL, base));
			z += BigInt (strtol ((std::string (p, p+1)).c_str (), NULL, base));
		}
		p++;
	}
}


