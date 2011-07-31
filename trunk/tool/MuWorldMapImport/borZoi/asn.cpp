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


#pragma warning(disable:4786) // for Visual C++
#include "borzoi.h"


//ansi-X9-62 OBJECT IDENTIFIER ::= { iso(1) member-body(2) us(840) 10045 }

OCTETSTR DER_Encode (unsigned long z);
unsigned long DER2UL (OCTETSTR der_rep);

OCTETSTR DER_Encode (BigInt z);
BigInt DER2BigInt (OCTETSTR der_rep);

OCTETSTR DER_Encode (F2M f);
F2M DER2F2M (OCTETSTR der_rep);

OCTETSTR DER_Encode (Point P);
Point DER2Point (OCTETSTR der_rep);

OCTETSTR DER_Encode (Curve C);
Curve DER2Curve (OCTETSTR der_rep);

OCTETSTR DER_Encode (OCTETSTR in);
OCTETSTR DER2OCTETSTR (OCTETSTR der_rep);

OCTETSTR DER_Encode (EC_Domain_Parameters dp);
EC_Domain_Parameters DER2ECDP (OCTETSTR der_rep);


OCTETSTR DER_Seq_Encode (OCTETSTR der_rep);

std::vector<OCTETSTR> DER_Seq_Decode (OCTETSTR in);

OCTETSTR DER_Seq_Encode (std::vector<OCTETSTR> in);

unsigned long DER_Extract_Length (OCTETSTR& der);

void DER_Insert_Length (OCTETSTR& der);


DER::DER (EC_Domain_Parameters dp) {
	v = DER_Encode (dp);
}


DER::DER (ECPubKey pk) {
	OCTETSTR algorithmidentifier;
     	// 06 Length (40*01+02+840+10045+02+01=)
	std::vector<OCTETSTR> a;

	OCTETSTR algorithm(9);
	algorithm[0] = 0x06; algorithm[1] = 0x07; algorithm[2] = 0x2A;
	algorithm[3] = 0x86; algorithm[4] = 0x48; algorithm[5] = 0xCE;
	algorithm[6] = 0x3D; algorithm[7] = 0x02; algorithm[8] = 0x01;
	a.push_back (algorithm); // version

	a.push_back (DER_Encode (pk.dp));

	algorithmidentifier = DER_Seq_Encode (a);
	std::vector<OCTETSTR> buf;
	buf.push_back (algorithmidentifier);

	OCTETSTR ecpoint = DER_Encode (pk.W);

	ecpoint.insert (ecpoint.begin (), 0x00);	
	if (ecpoint.size() > 127) {
		std::vector<OCTETSTR>::size_type size = ecpoint.size();
		OCTET no_octets = 0;
		while (size>0) {
			ecpoint.insert (ecpoint.begin (), size%0x100);
			size /= 0x100;
			no_octets++;
		}
		no_octets |= 0x80;
		ecpoint.insert (ecpoint.begin (), no_octets);	
	} else {
		ecpoint.insert (ecpoint.begin (), ecpoint.size ());	

	}
	ecpoint.insert (ecpoint.begin (), 0x03);	
	buf.push_back (ecpoint);

	v = DER_Seq_Encode (buf);
}


DER::DER (ECPrivKey sk) {
	std::vector<OCTETSTR> buf;

	buf.push_back (DER_Encode (1)); // version
	buf.push_back (DER_Encode (sk.s)); // private key
	buf.push_back (DER_Encode (sk.dp)); // EC domain parameters

	v = DER_Seq_Encode (buf);
}


DER::DER (ECDSA sig) {

	OCTETSTR Algorithmidentifier;
	std::vector<OCTETSTR> Algorithmidentifier_buf;
     	// 06 Length (40*01+02+840+10045+04+01=)
	OCTETSTR algorithm(9);
	algorithm[0] = 0x06; algorithm[1] = 0x07; algorithm[2] = 0x2A;
	algorithm[3] = 0x86; algorithm[4] = 0x48; algorithm[5] = 0xCE;
	algorithm[6] = 0x3D; algorithm[7] = 0x04; algorithm[8] = 0x01;
	Algorithmidentifier_buf.push_back (algorithm);
	Algorithmidentifier = DER_Seq_Encode (Algorithmidentifier_buf);

	OCTETSTR ECDSA_Sig_Value;
	std::vector<OCTETSTR> ECDSA_Sig_Value_buf;
	ECDSA_Sig_Value_buf.push_back (DER_Encode (sig.c)); // r in X9.62
	ECDSA_Sig_Value_buf.push_back (DER_Encode (sig.d)); // s in X9.62
	ECDSA_Sig_Value = DER_Seq_Encode (ECDSA_Sig_Value_buf);

	std::vector<OCTETSTR> buf;
	buf.push_back (Algorithmidentifier);
	buf.push_back (ECDSA_Sig_Value);

	v = DER_Seq_Encode (buf);
}


DER::DER (ECIES ct) {
	std::vector<OCTETSTR> buf;
	buf.push_back (DER(ct.V).v);
	buf.push_back (DER_Encode(ct.C));
	buf.push_back (DER_Encode(ct.T));

	v = DER_Seq_Encode (buf);
}


DER& DER::operator= (const DER& d) {
	if (this != &d)
		v = d.v;
	return *this;
}


std::ostream& DER::put (std::ostream&s) const {
	OCTETSTR::size_type i;
	for (i = 0; i < v.size(); i++) {
		//s.width(2); // 1.0.1 Incorrect, Remove
		//s.fill('0'); // 1.0.1 Incorrect, Remove
		s << v[i];
	}
	return s;
}


EC_Domain_Parameters DER::toECDP () {
	EC_Domain_Parameters dp = DER2ECDP (v);
	return dp;
}


ECPubKey DER::toECPubKey () {
	if (v[0] != 0x30)
		throw (borzoiException("DER_Decode_Public_Key: Not a Sequence"));

	std::vector<OCTETSTR> buf = DER_Seq_Decode (v);
	std::vector<OCTETSTR> a = DER_Seq_Decode (buf[0]);

	// buf[0]

     	// 06 Length (40*01+02+840+10045+02+01=)
	// a[0]
	// algorithm[0] = 0x06; algorithm[1] = 0x07; algorithm[2] = 0x2A;
	// algorithm[3] = 0x86; algorithm[4] = 0x48; algorithm[5] = 0xCE;
	// algorithm[6] = 0x3D; algorithm[7] = 0x02; algorithm[8] = 0x01;
	if (a[0][8] != 0x01)
		throw (borzoiException("DER_Decode_Public_Key: Invalid Algorithm"));

	// a[1]
	EC_Domain_Parameters dp = DER2ECDP (a[1]);

	// buf[1]
	OCTETSTR::iterator j=buf[1].begin();
	if (*j != 0x03)
		throw (borzoiException("ECPOINT Invalid"));
	else DER_Extract_Length (buf[1]); // Use this to delete the length OCTETs
	j=buf[1].begin(); buf[1].erase(j); // delete 0x00 OCTET
	Point ecpoint = DER2Point (buf[1]);

	ECPubKey pk (dp, ecpoint);
	return pk;
}


ECPrivKey DER::toECPrivKey () {
	if (v[0] != 0x30)
		throw (borzoiException("DER_Decode_Public_Key: Not a Sequence"));

	std::vector<OCTETSTR> buf = DER_Seq_Decode (v);

	if (DER2UL (buf[0]) != 0x01)
		throw (borzoiException("Unsupported Version"));
	BigInt s = DER2BigInt (buf[1]);
	EC_Domain_Parameters dp = DER2ECDP (buf[2]);

	ECPrivKey sk (dp, s);
	return sk;
}


ECDSA DER::toECDSA () {
	if (v[0] != 0x30)
		throw (borzoiException("DER::toECDSA: Not a Sequence"));

	std::vector<OCTETSTR> buf = DER_Seq_Decode (v);

	// buf[0]
	std::vector<OCTETSTR> a = DER_Seq_Decode (buf[0]);

    // 06 Length (40*01+02+840+10045+04+01=)
	// a[0]
	// algorithm[0] = 0x06; algorithm[1] = 0x07; algorithm[2] = 0x2A;
	// algorithm[3] = 0x86; algorithm[4] = 0x48; algorithm[5] = 0xCE;
	// algorithm[6] = 0x3D; algorithm[7] = 0x04; algorithm[8] = 0x01;
	if (a[0][8] != 0x01)
		throw (borzoiException("DER::toECDSA: Invalid Algorithm"));

	std::vector<OCTETSTR> b = DER_Seq_Decode (buf[1]);

	// c is r in X9.62, d is s in X9.62
	return ECDSA (DER2BigInt (b[0]), DER2BigInt (b[1]));
}


ECIES DER::toECIES () {
	std::vector<OCTETSTR> buf = DER_Seq_Decode (v);

	if (buf.size() != 3)
		throw (borzoiException("DER::toECIES: Invalid Sequence"));
	return ECIES (DER(buf[0]).toECPubKey(), DER2OCTETSTR(buf[1]), DER2OCTETSTR(buf[2]));
}


unsigned long DER2UL (OCTETSTR der_rep) {
	unsigned long length;
	if (der_rep[0] != 0x02)
		throw (borzoiException("Not an Integer"));
	else length = DER_Extract_Length (der_rep);

	unsigned long z = 0;
	for (unsigned long i=0; i<length; i++) {
		z *= 0x100;
		z += der_rep[i];
	}
	return z;
}


F2M DER2F2M (OCTETSTR der_rep) {
	F2X x;
	unsigned long k=0;

	unsigned long length;
	if (der_rep[0] != 0x04)
		throw (borzoiException("Not an Octet String"));
	else length = DER_Extract_Length (der_rep);

	for (unsigned long i=0; i<length; i++) {
		if (der_rep[length-1-i]&1)
			x.setCoeff (k, 1);
		k++;
		if (der_rep[length-1-i]&2)
			x.setCoeff (k, 1);
		k++;
		if (der_rep[length-1-i]&4)
			x.setCoeff (k, 1);
		k++;
		if (der_rep[length-1-i]&8)
			x.setCoeff (k, 1);
		k++;
		if (der_rep[length-1-i]&16)
			x.setCoeff (k, 1);
		k++;
		if (der_rep[length-1-i]&32)
			x.setCoeff (k, 1);
		k++;
		if (der_rep[length-1-i]&64)
			x.setCoeff (k, 1);
		k++;
		if (der_rep[length-1-i]&128)
			x.setCoeff (k, 1);
		k++;
	}

	F2M f = F2M (x);
	return f;
}


BigInt DER2BigInt (OCTETSTR der_rep) {
	unsigned long length;
	if (der_rep[0] != 0x02)
		throw (borzoiException("Not an Integer"));
	else length = DER_Extract_Length (der_rep);

	BigInt z;
	for (unsigned long i=0; i<length; i++) {
		z *= BigInt (0x10);
		z *= BigInt (0x10);
		z += BigInt (der_rep[i]);
	}
	return z;
}


Point DER2Point (OCTETSTR der_rep) {
	unsigned long length;
	if (der_rep[0] != 0x04)
		throw (borzoiException("Not an Octet String"));
	else length = DER_Extract_Length (der_rep);

	F2X x, y;
	unsigned long k=0;
	for (unsigned long i=(length-1)/2; i>0; i--) {
		if (der_rep[i]&1)
			x.setCoeff (k, 1);
		if (der_rep[i+(length-1)/2]&1)
			y.setCoeff (k, 1);
		k++;
		if (der_rep[i]&2)
			x.setCoeff (k, 1);
		if (der_rep[i+(length-1)/2]&2)
			y.setCoeff (k, 1);
		k++;
		if (der_rep[i]&4)
			x.setCoeff (k, 1);
		if (der_rep[i+(length-1)/2]&4)
			y.setCoeff (k, 1);
		k++;
		if (der_rep[i]&8)
			x.setCoeff (k, 1);
		if (der_rep[i+(length-1)/2]&8)
			y.setCoeff (k, 1);
		k++;
		if (der_rep[i]&16)
			x.setCoeff (k, 1);
		if (der_rep[i+(length-1)/2]&16)
			y.setCoeff (k, 1);
		k++;
		if (der_rep[i]&32)
			x.setCoeff (k, 1);
		if (der_rep[i+(length-1)/2]&32)
			y.setCoeff (k, 1);
		k++;
		if (der_rep[i]&64)
			x.setCoeff (k, 1);
		if (der_rep[i+(length-1)/2]&64)
			y.setCoeff (k, 1);
		k++;
		if (der_rep[i]&128)
			x.setCoeff (k, 1);
		if (der_rep[i+(length-1)/2]&128)
			y.setCoeff (k, 1);
		k++;
	}

	Point p = Point (F2M (x), F2M (y));
	return p;
}


Curve DER2Curve (OCTETSTR der_rep) {
	if (der_rep[0] != 0x30)
		throw (borzoiException("Not a Sequence"));

	std::vector<OCTETSTR> v = DER_Seq_Decode (der_rep);
	return Curve (DER2F2M (v[0]), DER2F2M (v[1]));
}


OCTETSTR DER2OCTETSTR (OCTETSTR der_rep) {
	unsigned long length;
	if (der_rep[0] != 0x04)
		throw (borzoiException("Not an Octet String"));
	else length = DER_Extract_Length (der_rep);

	return der_rep;
}


OCTETSTR DER_Seq_Encode (std::vector<OCTETSTR> in) {
	OCTETSTR seq_rep;

	std::vector<OCTETSTR>::iterator i;

	for (i = in.begin(); i !=in.end(); i++) {
		seq_rep = seq_rep || (*i);
	}

	if (seq_rep.size() > 127) {
		std::vector<OCTETSTR>::size_type size = seq_rep.size();
		OCTET no_octets = 0;
		while (size>0) {
			seq_rep.insert (seq_rep.begin (), size%0x100);
			size /= 0x100;
			no_octets++;
		}
		no_octets |= 0x80;
		seq_rep.insert (seq_rep.begin (), no_octets);	
	} else {
		seq_rep.insert (seq_rep.begin (), seq_rep.size ());	
	}
	seq_rep.insert (seq_rep.begin (), 0x30);	
	return seq_rep;
}


unsigned long DER_Extract_Length (OCTETSTR& der) {
	unsigned long length=0;
	OCTETSTR::iterator i=der.begin();
	
	der.erase(i); // erase DER type OCTET

	i=der.begin();
	if (*i&0x80) { // Long
		for (unsigned long j= 1; j <=(*i&0x7f); j++) {
			length *= 0x100;
			length += der[j];
		}
		der.erase(i, i+(*i&0x7f)+1);
	} else { // Short
		length = *i;
		der.erase(i);
	}

	if (length != der.size())
		length = 0;

	return length;
}


void DER_Insert_Length (OCTETSTR& der) {
	if (der.size () > 127) { // long form
		unsigned long length = der.size();
		OCTET no_octets = 0x80;
		while (length) {
			der.insert (der.begin (), (OCTET)(length%256));
			length /= 256;
			no_octets++;
		}
		der.insert (der.begin (), no_octets);
	} else { // short form
		der.insert (der.begin (), (OCTET)der.size());
	}
}


std::vector<OCTETSTR> DER_Seq_Decode (OCTETSTR in) {
	std::vector<OCTETSTR> out;
	unsigned long j, k;
	unsigned long length=0;

	OCTETSTR::iterator i=in.begin();
	
	if (*i != 0x30)
		throw (borzoiException("Not a Sequence"));
	else length = DER_Extract_Length (in);

	i=in.begin();
	for (j=0; j<length; j++) {
		if (in[j+1]&0x80) { // Long
			OCTETSTR vec;
			unsigned long l=0;
			vec.push_back (in[j]);
			vec.push_back (in[j+1]);
			for (k= 0; k <(in[j+1]&0x7f); k++) {
				l *= 0x100;
				l += in[j+2+k];
				vec.push_back (in[j+2+k]);
			}
			for (k=0; k<l; k++) {
				vec.push_back (in[j+2+(in[j+1]&0x7f)+k]);
			}
			out.push_back (vec);
			j +=(1+in[j+1]&0x7f)+l;
		} else { // Short*/
			OCTETSTR vec;
			for (k=j; k<=j+1+in[j+1]; k++) {
				vec.push_back (in[k]);
			}
			out.push_back (vec);
			j +=1+in[j+1];
		}
	}
	return out;
}




OCTETSTR DER_Encode (unsigned long z) {
	OCTETSTR der_rep;
	while (z) {
		der_rep.insert (der_rep.begin(), z & 0xff);	
		z /= 0x100;
	}
	if (der_rep[0]&0x80)
		der_rep.insert (der_rep.begin (), 0x00);
	DER_Insert_Length (der_rep);
	der_rep.insert (der_rep.begin (), 0x02);	
	return der_rep;
}


OCTETSTR DER_Encode (F2M f) {
	OCTETSTR der_rep = FE2OSP (f);
	DER_Insert_Length (der_rep);
	der_rep.insert (der_rep.begin (), 0x04);	
	return der_rep;
}


OCTETSTR DER_Encode (BigInt z) {
	OCTETSTR der_rep;
	while (!z.isZero ()) {
		der_rep.insert (der_rep.begin(), z.toOctet());	
		z /= BigInt (0x10);
		z /= BigInt (0x10);
	}
	if (der_rep[0]&0x80)
		der_rep.insert (der_rep.begin (), 0x00);
	DER_Insert_Length (der_rep);
	der_rep.insert (der_rep.begin (), 0x02);	
	return der_rep;
}


OCTETSTR DER_Encode (Point P) {
	OCTETSTR PC (1); PC[0]=0x04;
	OCTETSTR der_rep = (PC || FE2OSP (P.x)) || FE2OSP (P.y);
	DER_Insert_Length (der_rep);
	der_rep.insert (der_rep.begin (), 0x04);	
	return der_rep;
}


OCTETSTR DER_Encode (Curve C) {
	std::vector<OCTETSTR> v;
	v.push_back (DER_Encode (C.a2));
	v.push_back (DER_Encode (C.a6));
	OCTETSTR der_rep = DER_Seq_Encode (v);
	return der_rep;
}


OCTETSTR DER_Seq_Encode (OCTETSTR der_rep) {
	DER_Insert_Length (der_rep);
	der_rep.insert (der_rep.begin (), 0x30);
	return der_rep;
}


OCTETSTR DER_Encode (OCTETSTR der_rep) {
	DER_Insert_Length (der_rep);
	der_rep.insert (der_rep.begin (), 0x04);
	return der_rep;
}


OCTETSTR DER_Encode (EC_Domain_Parameters dp) {
	std::vector<OCTETSTR> v;

	v.push_back (DER_Encode (1)); // version
	OCTETSTR m = DER_Encode (dp.m); // m
	OCTETSTR basis(11);
	OCTETSTR param;
	switch (dp.basis) {
		case 1: // Gaussian Basis
     			// 06 Length (40*01+02+840+10045+01+02+03+01=)
			basis[0] = 0x06; basis[1] = 0x09; basis[2] = 0x2A;
			basis[3] = 0x86; basis[4] = 0x48; basis[5] = 0xCE;
			basis[6] = 0x3D; basis[7] = 0x01; basis[8] = 0x02;
			basis[9] = 0x03; basis[10] = 0x01;
			param = OCTETSTR (2);
			param[0] = 0x05; param[1] = 0x00;
			break;
		case 2: // Trinomial
     			// 06 Length (40*01+02+840+10045+01+02+03+02)
			basis[0] = 0x06; basis[1] = 0x09; basis[2] = 0x2A;
			basis[3] = 0x86; basis[4] = 0x48; basis[5] = 0xCE;
			basis[6] = 0x3D; basis[7] = 0x01; basis[8] = 0x02;
			basis[9] = 0x03; basis[10] = 0x02;
			param = DER_Encode (dp.trinomial_k);
			break;
		case 3: // Pentanomial Basis
     			// 06 Length (40*01+02+840+10045+01+02+03+03)
			basis[0] = 0x06; basis[1] = 0x09; basis[2] = 0x2A;
			basis[3] = 0x86; basis[4] = 0x48; basis[5] = 0xCE;
			basis[6] = 0x3D; basis[7] = 0x01; basis[8] = 0x02;
			basis[9] = 0x03; basis[10] = 0x03;
			param = DER_Encode (dp.pentanomial_k1) ||
				DER_Encode (dp.pentanomial_k2) || 
				DER_Encode (dp.pentanomial_k3);
			param = DER_Seq_Encode (param);
			break;
		default:
			throw (borzoiException("DER_Encode (EC_Domain_Parameters dp): Invalid Basis"));
			break;
	}
	v.push_back (DER_Seq_Encode ((m || basis) || param)); // FieldID

	v.push_back (DER_Encode (Curve (dp.a, dp.b))); // curve

	v.push_back (DER_Encode (dp.G)); // base 
	v.push_back (DER_Encode (dp.r)); // order 
	v.push_back (DER_Encode (dp.k)); // cofactor 

	OCTETSTR der_rep = DER_Seq_Encode (v);
	return der_rep;
}


EC_Domain_Parameters DER2ECDP (OCTETSTR der_rep) {
	if (der_rep[0] != 0x30)
		throw (borzoiException("DER2ECDP: Not a sequence"));

	std::vector<OCTETSTR> v = DER_Seq_Decode (der_rep);

	if (DER2UL (v[0]) != 0x01)
		throw (borzoiException("DER2ECDP: Unsupported Version"));

	std::vector<OCTETSTR> FieldID = DER_Seq_Decode (v[1]);
	unsigned long m = DER2UL (FieldID[0]);
	int basis = FieldID[1][10];
	unsigned long trinomial;
	unsigned long pentanomial_k1;
	unsigned long pentanomial_k2;
	unsigned long pentanomial_k3;
	std::vector<OCTETSTR> param;
	F2X pt;
	switch (basis) {
		case 1: // Gaussian Basis
			if ((FieldID[0][0]!=0x05)&(FieldID[0][1]!=0x00))
				throw (borzoiException("DER2ECDP: Gaussian Basis parameter not NULL"));
			break;
		case 2: // Trinomial
			trinomial = DER2UL (FieldID[2]);
			pt = Trinomial (m, trinomial, 0);
			setModulus (pt);
			break;
		case 3: // Pentanomial Basis
			param = DER_Seq_Decode (FieldID[2]);
			pentanomial_k1 = DER2UL (param[0]);
			pentanomial_k2 = DER2UL (param[1]);
			pentanomial_k3 = DER2UL (param[2]);
			pt = Pentanomial (m, pentanomial_k3, pentanomial_k2, pentanomial_k1, 0);
			setModulus (pt);
			break;
		default: // Error
			throw (borzoiException("DER2ECDP: Invalid Basis"));
			break;
	}

	Curve C = DER2Curve (v[2]);
	Point G = DER2Point (v[3]);
	BigInt r = DER2BigInt (v[4]);
	BigInt k = DER2BigInt (v[5]); 

	EC_Domain_Parameters dp;
	if (basis==2)
		dp = EC_Domain_Parameters (m, basis, trinomial, C, r, G, k);
	else dp = EC_Domain_Parameters (m, basis, pentanomial_k3, pentanomial_k2, pentanomial_k1, C, r, G, k);

	if (!dp.valid()) {
		throw (borzoiException("DER2ECDP: Invalid Domain"));
	}
	return dp;
}


HexEncoder& HexEncoder::operator= (const HexEncoder& he) {
	if (this != &he)
		v = he.v;
	return *this;
}


std::ostream& HexEncoder::put (std::ostream&s) const {
	OCTETSTR::size_type i;
	for (i = 0; i < v.size(); i++) {
		s.width(2);
		s.fill('0');
		s << std::hex << (int)v[i];
	}
	return s;
}

