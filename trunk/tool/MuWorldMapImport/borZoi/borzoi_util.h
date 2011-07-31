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


#include <iostream>
#include <string>
#include <vector>
#include "borzoi_math.h"


#ifndef BORZOI_UTIL_H
#define BORZOI_UTIL_H

typedef unsigned char OCTET;
typedef std::vector<OCTET> OCTETSTR;


class borzoiException {
	std::string err_str;
public:
	inline borzoiException () {err_str="borzoiException";}
	inline borzoiException (std::string s) {err_str="borzoiException"+s;}
	inline void debug_print () {std::cerr << err_str.c_str() << std::endl;}
};

class Point {
public:
	F2M x;
	F2M y;
	inline Point () {}
	inline Point (F2M p_x, F2M p_y) { x = p_x; y = p_y; }
	inline Point (std::string p_x, std::string p_y) {str_to_F2M(p_x,x); str_to_F2M(p_y,y); }
	Point negative ();
	inline long isZero () {return (x.isZero () & y.isZero ());}
	std::ostream& put (std::ostream&s);
	Point& operator= (const Point& p);
};

inline bool operator== (Point a, Point b) {return (((a.x==b.x)&(a.y==b.y))>0);}

inline std::ostream& operator<< (std::ostream&s, Point p) { return p.put (s); }


// Elliptic Curve of form Y^2 + XY = x^3 + a2x^2 + a6 over F2M
class Curve {
public:
	F2M a2;
	F2M a6;
	inline Curve () {}
	inline Curve (F2M a2_val, F2M a6_val) { a2 = a2_val; a6 = a6_val; }
	inline Curve (std::string a2_val, std::string a6_val) { str_to_F2M (a2_val, a2); str_to_F2M (a6_val, a6); }
	std::ostream& put (std::ostream&s);
	Point add (Point p1, Point p2);
	Point dbl (Point p1);
	inline Point sub (Point p1, Point p2) {return add (p1,p2.negative());}
	Point mul (BigInt k, Point P);
};

inline std::ostream& operator<< (std::ostream&s, Curve c) { return c.put (s); }


// Conversion Functions
OCTETSTR BS2OSP (F2M b);

inline OCTETSTR FE2OSP (F2M x) { return BS2OSP (x); }

BigInt OS2IP (OCTETSTR o);

OCTETSTR I2OSP (BigInt i);

inline BigInt FE2IP (F2M j) { return OS2IP (FE2OSP(j)); }

F2M I2FEP (BigInt z);


BigInt operator|| (const BigInt& a, const BigInt& b); // concantenate two BigInts
std::vector<OCTET> operator|| (const std::vector<OCTET>& a, const std::vector<OCTET>& b); // concantenate two OCTETSTR

std::vector<OCTET> operator^ (std::vector<OCTET> a, std::vector<OCTET> b); // xor two OCTETSTR


#endif	// BORZOI_UTIL_H
