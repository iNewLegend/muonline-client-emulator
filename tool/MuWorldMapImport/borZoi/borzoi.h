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


#include "borzoi_util.h"


#ifndef BORZOI_H
#define BORZOI_H

typedef unsigned char OCTET;
typedef std::vector<OCTET> OCTETSTR;

// Abstract base class
class borZoi {
public:
	virtual std::ostream& put (std::ostream&s) const = 0;
};

inline std::ostream& operator<< (std::ostream&s, borZoi& b) { return b.put (s); }


// Elliptic Curve Domain Parameters
class EC_Domain_Parameters : public borZoi{
public:
	unsigned long m; // a positive integer, specifying the field GF(2^m)
	int basis; // Gaussia(1), Trinomial(2), Pentanomial(3)
	unsigned long trinomial_k; // x^m + x^k +1
	unsigned long pentanomial_k3; // x^m + x^k3 + x^k2 + x^k1 +1
	unsigned long pentanomial_k2; // x^m + x^k3 + x^k2 + x^k1 +1
	unsigned long pentanomial_k1; // x^m + x^k3 + x^k2 + x^k1 +1
	F2M a; // an element of GF(2^m), E: y^2 + xy = x^3 + ax^2 + b
	F2M b; // an element of GF(2^m), E: y^2 + xy = x^3 + ax^2 + b
	BigInt r; // a positive prime integer dividing the number of points on E
	Point G; // a point on E of order r;
	BigInt k; // a positive prime integer, k = #E/r

	inline EC_Domain_Parameters () {}
	inline EC_Domain_Parameters (unsigned long m_val,
			int basis_val, unsigned long tri_k_val,
			const Curve& c_val, const BigInt& r_val,
			const Point& G_val, const BigInt& k_val) { m=m_val; basis=basis_val; trinomial_k = tri_k_val; a=c_val.a2; b=c_val.a6; r=r_val; G=G_val; k=k_val; }
	inline EC_Domain_Parameters (unsigned long m_val,
			int basis_val, unsigned long pen_k3_val,
			unsigned long pen_k2_val, 
			unsigned long pen_k1_val, const Curve& c_val, 
			const BigInt& r_val, const Point& G_val, const BigInt& k_val) { m=m_val; basis=basis_val; pentanomial_k3 = pen_k3_val; pentanomial_k2 = pen_k2_val; pentanomial_k1 = pen_k1_val; a=c_val.a2; b=c_val.a6; r=r_val; G=G_val; k=k_val; }

	bool valid ();
	// A partial implementation (steps 6.4 to 7) of A.16.8 in P1363
	//
	// 6.4 Check that b != 0 in GF (2 m ).
	// 6.5 Check that G != O. Let G = (x, y).
	// 6.6 Check that x and y are elements of GF (2 m ).
	// 6.7 Check that y^2 + xy = x^3 + ax^2 + b in GF (2 m ).
	// 6.8 Check that rG = O.
	// 6.9 Check that the curve is not an instance of the following 
	//     excluded case:
	// 6.9.1 If the output of the algorithm given in A.12.1 is "False"
	//       then the curve is excluded because it is subject to the MOV
	//       reduction attack described in [MOV93].
	// 7. Output "True" if the checks given in Steps 4 through 6 work, 
	//    and "False" otherwise.

	EC_Domain_Parameters& operator= (const EC_Domain_Parameters& dp);
	std::ostream& put (std::ostream&s) const;
};


// Elliptic Curve Private Key
class ECPrivKey {
public:
	EC_Domain_Parameters dp;
	BigInt s; // the private key

	ECPrivKey (const EC_Domain_Parameters& ecdp); // generate a random private key
	ECPrivKey (const EC_Domain_Parameters& ecdp, const BigInt& ecs);

	ECPrivKey& operator= (const ECPrivKey& kp);
};

#define ECSecKey ECPrivKey	// Deprecated - only for compatibility with old code


// Elliptic Curve Public Key
class ECPubKey {
public:
	EC_Domain_Parameters dp; // the EC domain parameters for this key pair
	Point W; // the public key

	inline ECPubKey () {};
	ECPubKey (const ECPrivKey& sk);
	ECPubKey (const EC_Domain_Parameters& dp, const Point& W);

	bool valid (); // return true iff the public key is valid
	ECPubKey& operator= (const ECPubKey& kp);
};


// Hash Functions
OCTETSTR SHA1 (const OCTETSTR& x);
OCTETSTR SHA1 (const std::string& x);
OCTETSTR SHA1 (BigInt x);


// Key Derivation Function 2 from P1363a
// Uses the SHA-1 hash function
// Throws borzoiException for invalid output lengths
OCTETSTR KDF2 (OCTETSTR Z, OCTETSTR::size_type oLen, OCTETSTR P);

//  HMAC message authentication code from P1363a
// Uses the SHA-1 hash function
// The tag length is equal to 160 bits
OCTETSTR MAC1 (OCTETSTR KB, OCTETSTR M);


/********** Symmetric Encryption / Decryption **********/

// Advanced Encryption Symmetric (AES) encryption algorithm in Cipher Block 
// Chaining (CBC) mode with a null initialization vector
// Returns the encrypted cipher text
OCTETSTR AES_CBC_IV0_Encrypt (OCTETSTR KB, OCTETSTR M, int keysize=128);


// Advanced Encryption Symmetric (AES) decryption algorithm in Cipher Block 
// Chaining (CBC) mode with a null initialization vector
// Returns the decrypted plain text or else throws borzoiException
OCTETSTR AES_CBC_IV0_Decrypt (OCTETSTR KB, OCTETSTR C, int keysize=128);


/********** Elliptic Curve Algorithms **********/


// Elliptic Curve Secret Value Derivation Primitive, Diffie-Hellman version
// Throws borzoiException if the point P is zero
F2M ECSVDP_DH (const EC_Domain_Parameters& dp, const BigInt& s, const Point& Wi);


// Elliptic Curve Key Agreement Scheme, Diffie-Hellman
OCTETSTR ECKAS_DH1 (const EC_Domain_Parameters& dp, const BigInt& s, const Point& Wi);
OCTETSTR ECKAS_DH1 (const EC_Domain_Parameters& dp, const BigInt& s, const Point& Wi, const OCTETSTR& P);


// Elliptic Curve Digital Signature Algorithm
class ECDSA : public borZoi {
public:
	BigInt c;
	BigInt d;

	inline ECDSA () {}
	ECDSA (const BigInt& cval, const BigInt& dval);
	ECDSA (const ECPrivKey& sk, const BigInt& f);
	bool verify (const ECPubKey& pk, const BigInt& f);
	std::ostream& put (std::ostream&s) const;
	ECDSA& operator= (const ECDSA& esig);
};


// Elliptic Curve Integrated Encryption System
class ECIES : public borZoi {
public:
	ECPubKey V;
	OCTETSTR C; // Ciphertext
	OCTETSTR T; // Tag;

	inline ECIES () {}
	inline ECIES (ECPubKey V_val, OCTETSTR C_val, OCTETSTR T_val) { V=V_val; C=C_val; T=T_val;}
	ECIES (OCTETSTR& M, ECPubKey& pk);
	OCTETSTR decrypt (ECPrivKey& sk); // Throws borzoiException if the tag is invalid
	std::ostream& put (std::ostream&s) const;
};


/********** Data Formats **********/
class DER : public borZoi {
public:
	inline DER (OCTETSTR vin) { v=vin; } // set v = to vin
	DER (EC_Domain_Parameters dp);
	DER (ECPubKey pk); // encode a public key
	DER (ECPrivKey sk); // encode a private key
	DER (ECDSA sig); // encode a signature
	DER (ECIES ct); // encode an ECIES ciphertext (non-standard)

	EC_Domain_Parameters toECDP ();
	ECPubKey toECPubKey (); // convert a DER string to an EC public key
	ECPrivKey toECPrivKey (); // convert a DER string to an EC private key
	ECDSA toECDSA (); // convert a DER string to an ECDSA signature
	ECIES toECIES (); // convert a DER string to an ECIES ciphertext (non-standard)
	DER& operator= (const DER& d);
	std::ostream& put (std::ostream&s) const;
	OCTETSTR v;

private:
};


class HexEncoder : public borZoi {
public:
	inline HexEncoder (OCTETSTR vin) { v=vin; } // set v = to vin
	inline HexEncoder (DER der) { v=der.v; }

	HexEncoder& operator= (const HexEncoder& d);
	std::ostream& put (std::ostream&s) const;
	OCTETSTR v;

private:
};


#endif	// BORZOI_H
