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


#include "borzoi.h"    // Include this to use the elliptic curve and
                       // symmetric cipher routines
#include "nist_curves.h" // Include this to use the curves recommended by NIST

#include <fstream>


// Elliptic Curve Key Agreement Scheme Diffie-Hellman
void eckas_dh1_ex () {
	// Degree 163 Binary Field from fips186-2
	use_NIST_B_163 ();
	EC_Domain_Parameters dp = NIST_B_163;

	OCTETSTR P(3); // Key Derivation Parameters
	for (OCTET i=0; i<3; i++) {
		P[i] = i;
	}

	// User A generates a key from a password
	// (warning: not very secure in reality)
	ECPrivKey skA (dp, OS2IP(SHA1("A's password")));
	ECPubKey pkA (skA);

	// User B generates a key from a password
	// (warning: not very secure in reality)
	ECPrivKey skB (dp, OS2IP(SHA1("B's password")));
	ECPubKey pkB (skB);

	// DER encoding, is a common standard method of representing public 
	// keys

	DER derA (pkA); // User A
	HexEncoder hexA (derA); // User A

	DER derB (pkB); // User B
	HexEncoder hexB (derB); // User B

	// You might save the DER ecoded keys to files or send them over 
	// the network here.
	std::cout << "DER Encoding of pkA: " << hexA << std::endl; // send to User B

	std::cout << "DER Encoding of pkB: " << hexB << std::endl; // send to User A

	try { // try to catch any DER parsing errors

		// User A generates KA from his private key and user B's 
		// public key
		ECPubKey pkB2 = derB.toECPubKey();
		BigInt KA = OS2IP (ECKAS_DH1 (dp, skA.s, pkB2.W, P));

		// User B generates KB from his private key and user A's 
		// public key
		ECPubKey pkA2 = derA.toECPubKey();
		BigInt KB = OS2IP (ECKAS_DH1 (dp, skB.s, pkA2.W, P));


		// Verify that the keys match 
		// (typically, users A and B would use KA and KB to encrypt
		//  data with a symmetric cipher for transimission to each 
		//  other here)
		std::cout << "KA:   " << KA << std::endl;
		std::cout << "KB:   " << KB << std::endl;
		if (KA == KB)
			std::cout << "Keys match\n";
		else std::cout << "Error keys don't match\n";
	} catch (borzoiException e) { // print the error message and exit
		e.debug_print ();
	}
}


// Elliptic Curve Digital Signature Algorithm Example
void ecdsa_ex () {
	// Degree 163 Binary Field from fips186-2
	use_NIST_B_163 ();
	EC_Domain_Parameters dp = NIST_B_163;
	ECPrivKey sk (dp);// generate a key pair from the EC domain parameters

	std::string M ("correct message");

	ECDSA sig1 (sk, OS2IP(SHA1(M))); // generate the signature

	// DER encoding, is a common standard method of representing digital 
	// signatures
	DER der_str (sig1);
	HexEncoder hex_str (der_str);

	// You might save the DER ecoded signature to a file or send it over 
	// the network here.
	std::cout << "DER Encoding: " << hex_str << std::endl;
	
	ECDSA sig2;
	try { // try to catch any DER parsing errors
		sig2 = der_str.toECDSA (); // decode the DER string
	} catch (borzoiException e) { // print the error message and exit
		e.debug_print ();
		return;
	}

	ECPubKey pk (sk);
	std::cout << "Checking signature against M: " << M.c_str () << "\n->";

	std::cout << "SHA1(M): " << OS2IP(SHA1(M)) << std::endl;
	if (sig2.verify(pk, OS2IP(SHA1(M)))) // try to verify the signature
		std::cout << "valid signature\n";
	else std::cout << "invalid signature\n";

	M = "in" + M; // tamper with the message
	std::cout << "Checking signature against M: " << M.c_str () << "\n->";

	if (sig2.verify(pk, OS2IP(SHA1(M)))) // try to verify the signature
		std::cout << "valid signature\n";
	else std::cout << "invalid signature\n";
}


// Elliptic Curve Integrated Encryption Scheme
void ecies_ex () {
	// Degree 163 Binary Field from fips186-2
	use_NIST_B_233 ();
	EC_Domain_Parameters dp = NIST_B_233;

	ECPrivKey sk (dp);// generate a private key from the domain parameters
	ECPubKey pk (sk);// calculate the public key the private key

	// plaintext p1 = a, b, c, d
	OCTETSTR p1(4); p1[0]='a'; p1[1]='b'; p1[2]='c'; p1[3]='d';
	int i;
	for (i=0; i<4; i++) {
		std::cout << p1[i];
	}
	std::cout << std::endl;

	ECIES ct1 (p1, pk); // encrypt using the public key

	std::cout << "ct1: " << std::endl << ct1 << std::endl;
	OCTETSTR p2;
	try { // try to catch any exceptions if the tag is invalid
		p2 = ct1.decrypt(sk); // decrypt using the private key
	} catch (borzoiException e) { // print the error message and exit
		e.debug_print ();
		return;
	}
	for (i=0; i<4; i++) {
		std::cout << p2[i];
	}
	std::cout << std::endl;


	// DER encoding
	DER der_str (ct1);
	HexEncoder hex_str (der_str);

	// You might save the DER ecoded ciphertext to a file or send it over 
	// the network here.
	std::cout << "DER Encoding: " << hex_str << std::endl;
	
	ECIES ct2;
	try { // try to catch any DER parsing errors
		ct2 = der_str.toECIES (); // decode the DER string
	} catch (borzoiException e) { // print the error message and exit
		e.debug_print ();
		return;
	}

	std::cout << "ct2: " << std::endl << ct2 << std::endl;

	OCTETSTR p3;
	try { // try to catch any exceptions if the tag is invalid
		p3 = ct2.decrypt(sk); // decrypt using the private key
	} catch (borzoiException e) { // print the error message and exit
		e.debug_print ();
		return;
	}
	for (i=0; i<4; i++) {
		std::cout << p3[i];
	}
	std::cout << std::endl;

}


// Symmetric Cipher Example
void symcipher_ex () {
	//int keysize = 128; // 128 bit symmetric encryption key
	int keysize = 192; // 192 bit symmetric encryption key
	//int keysize = 256; // 256 bit symmetric encryption key

	int i;
	OCTETSTR P1; // This is 0
	OCTETSTR Z(keysize/8);
	for (i=0; i<(keysize/8); i++) {
		Z[i] = (OCTET)gen_random ();
	}
	OCTETSTR key = KDF2 (Z, keysize/8, P1);

	// plaintext p1 = a, b, c, d
	OCTETSTR p1(4); p1[0]='a'; p1[1]='b'; p1[2]='c'; p1[3]='d';
	for (i=0; i<4; i++) {
		std::cout << p1[i];
	}
	std::cout << std::endl;

	// encrypt p1 to produce ciphertext c
	OCTETSTR c = AES_CBC_IV0_Encrypt (key, p1, keysize);
	for (i=0; i<4; i++) {
		std::cout << c[i];
	}
	std::cout << std::endl;

	// decrypt to produce plaintext p2
	OCTETSTR p2;
	try { // try to catch any exceptions
		p2 = AES_CBC_IV0_Decrypt (key, c, keysize);
	} catch (borzoiException e) { // print the error message and exit
		e.debug_print ();
		return;
	}
	for (i=0; i<4; i++) {
		std::cout << p2[i];
	}
	std::cout << std::endl;
}


int notValid (int exp) {
	if (exp) {
		std::cout << "----> Test Successful" << std::endl;
	} else {
		std::cout << "----> Test Failure" << std::endl;
	}
	return !exp;
}


// Validation Tests
void validation_tests () {
	std::cout << std::endl << "borZoi Validation Tests" << std::endl;

	// ***********************************************************
	// SHA-1 Validation Tests
	// ***********************************************************
	std::cout << std::endl << "SHA-1 Validation Tests" << std::endl;

	OCTETSTR ABC(3); // Message to be hashed
	ABC[0] = 'a'; ABC[1] = 'b'; ABC[2] = 'c';

	BigInt Hash = OS2IP (SHA1 (ABC));

	BigInt Hash1 = hexto_BigInt("A9993E364706816ABA3E25717850C26C9CD0D89D");
	std::cout << "SHA-1 Hash of {\'a\', \'b\', \'c\'}:" << Hash<< std::endl;
	if (notValid (Hash == Hash1)) 
		return;

	Hash = OS2IP (SHA1 ("abc"));
	std::cout << "SHA-1 Hash of \"abc\":" << Hash << std::endl;
	if (notValid (Hash == Hash1))
		return;


	BigInt Hash2 = hexto_BigInt("84983E441C3BD26EBAAE4AA1F95129E5E54670F1");
	Hash = OS2IP (SHA1 ("abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"));
	std::cout << "SHA-1 Hash of \"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq\":" << Hash << std::endl;
	if (notValid (Hash == Hash2))
		return;


	OCTETSTR million_as(1000000);
	for (unsigned long i=0; i<1000000; i++) {
		million_as[i] = 'a';
	}
	BigInt Hash3 = hexto_BigInt("34AA973CD4C4DAA4F61EEB2BDBAD27316534016F");
	Hash = OS2IP (SHA1 (million_as));
	std::cout << "SHA-1 Hash of one million \'a\'s:" << Hash << std::endl;
	if (notValid (Hash == Hash3))
		return;

}


// File Operation Example
void file_ex () {
	// Degree 163 Binary Field from fips186-2
	use_NIST_B_163 ();
	EC_Domain_Parameters dp = NIST_B_163;

	ECPrivKey privKeyA (dp, OS2IP(SHA1("A's password")));
	ECPubKey pubKeyA (privKeyA);

	DER derA1 (pubKeyA);

	std::ofstream key_outfile ("pubKeyA.key", std::ios::binary);
	if (!key_outfile) {
		std::cout << "Error can't open file!\n";
		return;
	}
	key_outfile << derA1;
	key_outfile.close();

	std::ifstream key_infile ("pubKeyA.key", std::ios::binary);
	if (!key_infile) {
		std::cout << "Error can't open file!\n";
		return;
	}

	OCTETSTR derA2_v;
	char c; OCTET o;
	while (key_infile.get (c)) {
		o = (unsigned char)c;
		derA2_v.push_back (o);
	}
	std::cout << std::endl;

	key_infile.close();

	DER derA2 (derA2_v);
	HexEncoder hex1 (derA1);
	HexEncoder hex2 (derA2);
	std::cout << "derA1: " << hex1 << std::endl;
	std::cout << "derA2: " << hex2 << std::endl;
	if (!(derA1.v == derA2.v)) {
		std::cout << "Error\n";
		return;
	} else {
		std::cout << "OK\n";
	}

	return;
}


int main (int argc, char* argv[]) {

	// Examples of using the ECKAS, ECDSA ECIES and symCipher classes

	//eckas_dh1_ex (); // Elliptic Curve Key Agreement Scheme Diffie-Hellman
	//ecdsa_ex (); // Elliptic Curve Digital Signature Algorithm
	//ecies_ex (); // Elliptic Curve Integrated Encryption Scheme

	//symcipher_ex (); // Symmetric Cipher

	//validation_tests (); // Validation Tests

        //file_ex (); // File Operation Example
BigInt Hash;
BigInt sn;
ECPubKey m_pk;
Curve m_C;
//Point m_R;
OCTETSTR data(3); 

data[0] = 'a'; data[1] = 'b'; data[2] = 'c';
	{
		use_NIST_B_163 ();

		EC_Domain_Parameters dp = NIST_B_163;

		ECPrivKey sk (dp, OS2IP(SHA1(" .;sm_*)5&8c`w.~d")));

		ECPubKey pk (sk);

		DER derA (pk);
		HexEncoder hexA (derA);

		std::cout << "DER Encoding of pk: " << hexA << std::endl; // send to User B


		m_pk = pk;
		//////////////////////////////////////////////////////////////////////////
		BigInt r = GenRandom (pk.dp.m);

		r %= pk.dp.r;

		Curve C(pk.dp.a, pk.dp.b);
		m_C = C;

		Point R=C.mul (r, pk.dp.G);
		//m_R = R;

		//////////////////////////////////////////////////////////////////////////
		//然后我们定义一个字符串作为用户名



		//将用户名和R点的座标计算SHA：

		OCTETSTR x_buf = FE2OSP (R.x);

		OCTETSTR y_buf = FE2OSP (R.y);

		/*BigInt*/ Hash = OS2IP (SHA1 (data || x_buf || y_buf));
		//然后计算sn≡r - Hash * k (mod n)

		/*BigInt*/ sn = sk.s * Hash;

		sn %= pk.dp.r;  // pk.dp.r就是G点的阶

		sn = r - sn;

		if(sn < BigInt(0))

			sn += pk.dp.r;

		std::cout << "sn:   " << sn << std::endl;
		std::cout << "Hash:   " << Hash << std::endl;



	}
	{
		Point R2 = m_C.mul(sn,m_pk.dp.G);

		R2 = m_C.add(R2, m_C.mul(Hash,m_pk.W));

		OCTETSTR x2_buf = FE2OSP (R2.x);

		OCTETSTR y2_buf = FE2OSP (R2.y);

		//然后，H=SHA(username,x,y)；

		BigInt Hash2 = OS2IP (SHA1 (data || x2_buf || y2_buf));
		std::cout << "Hash2:   " << Hash2 << std::endl;

	}

	return 0;
}

