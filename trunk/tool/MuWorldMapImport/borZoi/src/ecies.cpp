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


ECIES::ECIES (OCTETSTR& M, ECPubKey& pk) {
	OCTETSTR P1, P2; // These are 0

	ECPrivKey u (pk.dp);
	V = ECPubKey(u);

	F2M z = ECSVDP_DH (pk.dp, u.s, pk.W);
	OCTETSTR Z = FE2OSP (z);
	OCTETSTR K = KDF2 (Z, 32, P1); // 256 bits
	OCTETSTR K1 (16); // 128 bit symmetric encryption key
	OCTETSTR K2 (16); // 128 bit MAC key
	for (int j=0; j<K1.size();j++) {
		K1[j] = K[j];
	}
	for (int k=0; k<K2.size();k++) {
		K2[k] = K[k+K1.size()];
	}
	C = AES_CBC_IV0_Encrypt (K1, M);

	T = MAC1 (K2, C||P2);
}


// Throws ECIES_Err if the tag is invalid
OCTETSTR ECIES::decrypt (ECPrivKey& sk) {
	OCTETSTR P1, P2; // These are 0

	F2M z = ECSVDP_DH (sk.dp, sk.s, V.W);
	OCTETSTR Z = FE2OSP (z);
	OCTETSTR K = KDF2 (Z, 32, P1); // 256 bits

	OCTETSTR K1 (16); // 128 bit symmetric encryption key
	OCTETSTR K2 (16); // 128 bit MAC key
	for (int j=0; j<K1.size();j++) {
		K1[j] = K[j];
	}
	for (int k=0; k<K2.size();k++) {
		K2[k] = K[k+K1.size()];
	}
	OCTETSTR M = AES_CBC_IV0_Decrypt (K1, C);

	if (T != MAC1 (K2, C||P2)) {
		throw borzoiException ("ECIES: tag invalid");
	}

	return M;
}


std::ostream& ECIES::put (std::ostream&s) const {
	s << "V: " << V.W << std::endl << "T: ";
	OCTETSTR::size_type i;
	for (i=0; i<T.size (); i++) {
		s << std::hex << (int)T[i];
	}
	s << std::endl << "C: ";
	for (i=0; i<C.size (); i++) {
		s << std::hex << (int)C[i];
	}
	return s;
}


