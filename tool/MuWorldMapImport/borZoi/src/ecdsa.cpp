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


struct ECDSA_Signature {
	BigInt c;
	BigInt d;
};


ECDSA_Signature ECSP_DSA (const ECPrivKey& sk, const BigInt& f);
bool ECVP_DSA (const ECPubKey& pk, const BigInt& f, const ECDSA_Signature& sig);


ECDSA_Signature ECSP_DSA (const ECPrivKey& sk, const BigInt& f) {
	BigInt temp, i, uinv;
	ECDSA_Signature sig;

	ECPrivKey u(sk.dp);
	i = FE2IP (ECPubKey(u).W.x);
	sig.c = i % sk.dp.r;
	uinv = InvMod (u.s, sk.dp.r);
	temp = f + (sk.s * sig.c);
	sig.d = MulMod (uinv, temp, sk.dp.r);

	while ((sig.c==BigInt(0)) || (sig.d==BigInt(0))) {
		u = ECPrivKey (sk.dp);
		i = FE2IP (ECPubKey(u).W.x);
		sig.c = i % sk.dp.r;
		BigInt uinv = InvMod (u.s, sk.dp.r);
		temp = f + (sk.s * sig.c);
		sig.d = MulMod (uinv, temp, sk.dp.r);
	}

	return sig;
}


bool ECVP_DSA (const ECPubKey& pk, const BigInt& f, const ECDSA_Signature& sig) {
	BigInt ONE(1);
	if (!(((ONE<=sig.c)&(sig.c<pk.dp.r)) & ((ONE<=sig.d)&(sig.d<pk.dp.r))))
		return false;

	BigInt h = InvMod (sig.d, pk.dp.r);
	BigInt h1 = MulMod (f, h, pk.dp.r);
	BigInt h2 = MulMod (sig.c, h, pk.dp.r);

	Curve E (pk.dp.a, pk.dp.b);
	Point P = E.add (E.mul (h1, pk.dp.G), E.mul (h2, pk.W));

	if (P.isZero ())
		return false;

	BigInt i = FE2IP (P.x); BigInt ci = i % pk.dp.r;

	if (sig.c == ci)
		return true;
	else return false;
}


ECDSA::ECDSA (const BigInt& cval, const BigInt& dval) {
	c = cval;
	d = dval;
}


ECDSA::ECDSA (const ECPrivKey& sk, const BigInt& f) {
	ECDSA_Signature esig = ECSP_DSA (sk, f);
	c = esig.c;
	d = esig.d;
}


bool ECDSA::verify (const ECPubKey& pk, const BigInt& f) {
	ECDSA_Signature esig;
	esig.c = c;
	esig.d = d;
	return ECVP_DSA (pk, f, esig);
}


std::ostream& ECDSA::put (std::ostream&s) const {
	s << "c: " << c << std::endl;
	s << "d: " << d << std::endl;
	return s;
}


ECDSA& ECDSA::operator= (const ECDSA& e) {
	if (this != &e) {
		c = e.c;
		d = e.d;
	}
	return *this;
}


