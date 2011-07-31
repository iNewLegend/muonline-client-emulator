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


OCTETSTR ECKAS_DH1 (const EC_Domain_Parameters& dp, const BigInt& s, const Point& Wi) {
	OCTETSTR P1; // This is 0 in this function

	F2M z = ECSVDP_DH (dp, s, Wi);
	OCTETSTR Z = FE2OSP (z);
	OCTETSTR K = KDF2 (Z, 16, P1); // 128 bits

	return K;
}

OCTETSTR ECKAS_DH1 (const EC_Domain_Parameters& dp, const BigInt& s, const Point& Wi, const OCTETSTR& P) {
	F2M z = ECSVDP_DH (dp, s, Wi);
	OCTETSTR Z = FE2OSP (z);
	OCTETSTR K = KDF2 (Z, 16, P); // 128 bits

	return K;
}


