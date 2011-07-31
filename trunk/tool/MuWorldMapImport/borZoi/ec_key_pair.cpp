/* 
   borZoi - An Elliptic Curve Cryptography Library

   Copyright (C) 2001, 2002 Anthony Mulcahy

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


// generate an EC private key
ECPrivKey::ECPrivKey (const EC_Domain_Parameters& ecdp) {
	dp = ecdp;
	s = GenRandom (dp.m);
	s %= dp.r;
}


ECPrivKey::ECPrivKey (const EC_Domain_Parameters& ecdp, const BigInt& ecs) {
	dp = ecdp;
	s = ecs;
}


ECPrivKey& ECPrivKey::operator= (const ECPrivKey& sk) {
	if (this != &sk) { // avoid self assignment
		dp = sk.dp;
		s = sk.s;
	}

	return *this;
}


ECPubKey::ECPubKey (const ECPrivKey& sk) {
	dp = sk.dp;
	Curve E (dp.a, dp.b);
	W = E.mul (sk.s, dp.G);
}


ECPubKey::ECPubKey (const EC_Domain_Parameters& ecdp, const Point& ecW) {
	dp = ecdp;
	W = ecW;
}


bool ECPubKey::valid () {
	if (W.isZero ())
		return false;
	F2M x = W.x;
	F2M y = W.y;
	F2M y2_xy = y*y + x*y;
	F2M x3_ax_b = x*x*x + dp.a*x*x + dp.b;
	if (y2_xy != x3_ax_b)
		return false;
	Curve E (dp.a, dp.b);
	Point O = E.mul (dp.r, W);
	if (!O.isZero ())
		return false;

	return true;
}


ECPubKey& ECPubKey::operator= (const ECPubKey& pk) {
	if (this != &pk) { // avoid self assignment
		dp = pk.dp;
		W = pk.W;
	}

	return *this;
}


