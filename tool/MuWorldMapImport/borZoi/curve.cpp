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


Point Point::negative () {
	Point p = *this;
	p.y += p.x;
	return p;
}


Point& Point::operator= (const Point& p) {
	if (this != &p) { // avoid self assignment
		x = p.x; y = p.y;
	}
	return *this;
}


std::ostream& Point::put (std::ostream&s) {
	s<<"x:0x";
	OCTETSTR x_buf = FE2OSP (x);
	//for (int i=x_buf.size()-1; i>=0; i--) {
	for (int i=0; i<x_buf.size(); i++) {
		s<<std::hex<<((x_buf[i]&0xf0)>>4);
		s<<std::hex<<(x_buf[i]&0xf);
	}
	s<<" y:0x";
	OCTETSTR y_buf = FE2OSP (y);
	//for (int j=y_buf.size()-1; j>=0; j--) {
	for (int j=0; j<y_buf.size(); j++) {
		s<<std::hex<<((y_buf[j]&0xf0)>>4);
		s<<std::hex<<(y_buf[j]&0xf);
	}
	
	return s;
}


std::ostream& Curve::put (std::ostream&s) {
	s << "Y^2 + XY = x^3 + a2x^2 + a6\n";
	s << "a2: ";
	OCTETSTR a2_buf = FE2OSP (a2);
	//for (int i=a2_buf.size()-1; i>=0; i--) {
	for (int i=0; i<a2_buf.size(); i++) {
		s<<std::hex<<((a2_buf[i]&0xf0)>>4);
		s<<std::hex<<(a2_buf[i]&0xf);
	}
	s << '\n';
	s << "a6: ";
	OCTETSTR a6_buf = FE2OSP (a6);
	//for (int j=a6_buf.size()-1; j>=0; j--) {
	for (int j=0; j<a6_buf.size(); j++) {
		s<<std::hex<<((a6_buf[j]&0xf0)>>4);
		s<<std::hex<<(a6_buf[j]&0xf);
	}
	s << '\n';
	return s;
}


// Based on the equations in Chap. 5 of Implementing Elliptic Curve Cryptography
// by Mike Rosing
Point Curve::add (Point p1, Point p2) {
	if (p1.isZero ())
		return p2;
	if (p2.isZero ())
		return p1;

	F2M x1 = p1.x + p2.x;
	F2M y1 = p1.y + p2.y;
	Point p3;
	if (x1.isZero())
		return p3;

	F2M theta = x1.inverse() * y1;
	//F2M theta2 = theta * theta;

	p3.x = theta.sqr() + theta + p1.x + p2.x + a2;
	p3.y = (p1.x + p3.x)*theta + p3.x + p1.y;
	return p3;
}


// Based on the equations in Chap. 5 of Implementing Elliptic Curve Cryptography
// by Mike Rosing
Point Curve::dbl (Point p1) {
	Point p3;
	if ((p1.x).isZero ())
		return p3;

	F2M theta = p1.x + (p1.x.inverse () * p1.y);
	//F2M theta2 = theta*theta;

	p3.x = theta.sqr() + theta + a2;
	p3.y = p1.x.sqr() + ((theta + F2M(1)) * p3.x);

	return p3;
}


// Based on Algorithm IV.1 on p. 63 of Elliptic Curves in Cryptography 
// by I. F. Blake, G. Seroussi, N. P. Smart.
Point Curve::mul (BigInt k, Point P) { 
	Point Q;

	for (long j=k.numBits()-1; j>=0; j--) {
		Q = dbl (Q);
		if (k.getBit(j)) {
			Q = add (Q, P);
		}
	}

	return Q;
}


