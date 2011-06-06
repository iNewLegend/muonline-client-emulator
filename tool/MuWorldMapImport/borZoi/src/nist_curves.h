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

#include "borzoi_util.h"

#ifndef BORZOI_NIST_CURVES_H
#define BORZOI_NIST_CURVES_H

// These are standard curves recommended by NIST
//
// An example of using the degree 163 binary field:
// 	use_NIST_B_163 (); // initialize the irreducible polynomial
// 	EC_Domain_Parameters dp = NIST_B_163; // set the domain parameters


// Field polynomial: p(t) = t^163 + t^7 + t^6 + t^3 + 1
inline void use_NIST_B_163 () { F2X pt=Pentanomial (163, 7, 6, 3, 0); setModulus (pt); };

// Degree 163 Binary Field from fips186-2
// Pseudorandom curve E: y^2 + xy = x^3 + x^2 + b,
// b = 2 0a601907 b8c953ca 1481eb10 512f7874 4a3205fd
// Base point order:
// r = 5846006549323611672814742442876390689256843201587
// Base point G:
// Gx = 3 f0eba162 86a2d57e a0991168 d4994637 e8343e36
// Gy = 0 d51fbc6c 71a0094f a2cdd545 b11c5c0c 797324f1
// Cofactor f = 2
#define NIST_B_163 EC_Domain_Parameters (163, 3, 7, 6, 3, Curve ("1", "20a601907b8c953ca1481eb10512f78744a3205fd"), decto_BigInt ("5846006549323611672814742442876390689256843201587"), Point ("3f0eba16286a2d57ea0991168d4994637e8343e36", "0d51fbc6c71a0094fa2cdd545b11c5c0c797324f1"), decto_BigInt ("2"));


// Field polynomial: p(t) = t^233 + t^74 + 1
inline void use_NIST_B_233 () { F2X pt=Trinomial (233, 74, 0); setModulus (pt); };


// Degree 233 Binary Field from fips186-2
// Pseudorandom curve E: y^2 + xy = x^3 + x^2 + b,
// b = 066 647ede6c 332c7f8c 0923bb58 213b333b 20e9ce42 81fe115f 7d8f90ad
// Base point order:
// r = 6901746346790563787434755862277025555839812737345013555379383634485463
// Base point G:
// Gx = 0fa c9dfcbac 8313bb21 39f1bb75 5fef65bc 391f8b36 f8f8eb73 71fd558b
// Gy = 100 6a08a419 03350678 e58528be bf8a0bef f867a7ca 36716f7e 01f81052
// Cofactor f = 2
#define NIST_B_233 EC_Domain_Parameters (233, 2, 74, Curve ("1", "066 647ede6c 332c7f8c 0923bb58 213b333b 20e9ce42 81fe115f 7d8f90ad"), decto_BigInt ("6901746346790563787434755862277025555839812737345013555379383634485463"), Point ("0fa c9dfcbac 8313bb21 39f1bb75 5fef65bc 391f8b36 f8f8eb73 71fd558b", "100 6a08a419 03350678 e58528be bf8a0bef f867a7ca 36716f7e 01f81052"), decto_BigInt("2"));


// Field polynomial: p(t) = t^283 + t^12 + t^7 + t^5 + 1
inline void use_NIST_B_283 () { F2X pt=Pentanomial (283, 12, 7, 5, 0); setModulus (pt); };


// Degree 283 Binary Field from fips186-2
// Pseudorandom curve E: y^2 + xy = x^3 + x^2 + b,
// b = 27b680a c8b8596d a5a4af8a 19a0303f ca97fd76 45309fa2 a581485a f6263e31 3b79a2f5
// Base point order:
// r = 7770675568902916283677847627294075626569625924376904889109196526770044277787378692871
// Base point G:
// Gx = 5f93925 8db7dd90 e1934f8c 70b0dfec 2eed25b8 557eac9c 80e2e198 f8cdbecd 86b12053
// Gy = 3676854 fe24141c b98fe6d4 b20d02b4 516ff702 350eddb0 826779c8 13f0df45 be8112f4
// Cofactor f = 2
#define NIST_B_283 EC_Domain_Parameters (283, 3, 12, 7, 5, Curve ("1", "27b680a c8b8596d a5a4af8a 19a0303f ca97fd76 45309fa2 a581485a f6263e31 3b79a2f5"), decto_BigInt ("7770675568902916283677847627294075626569625924376904889109196526770044277787378692871"), Point ("5f93925 8db7dd90 e1934f8c 70b0dfec 2eed25b8 557eac9c 80e2e198 f8cdbecd 86b12053", "3676854 fe24141c b98fe6d4 b20d02b4 516ff702 350eddb0 826779c8 13f0df45 be8112f4"), decto_BigInt ("2"));


// Field polynomial: p(t) = t^409 + t^87 + 1
inline void use_NIST_B_409() { F2X pt=Trinomial (409, 87, 0); setModulus (pt); };


// Degree 409 Binary Field from fips186-2
// Pseudorandom curve E: y^2 + xy = x^3 + x^2 + b,
// b = 021a5c2 c8ee9feb 5c4b9a75 3b7b476b 7fd6422e f1f3dd67 4761fa99 d6ac27c8 a9a197b2 72822f6c d57a55aa 4f50ae31 7b13545f
// Base point order:
// r = 661055968790248598951915308032771039828404682964281219284648798304157774827374805208143723762179110965979867288366567526771
// Base point G:
// Gx = 15d4860 d088ddb3 496b0c60 64756260 441cde4a f1771d4d b01ffe5b 34e59703 dc255a86 8a118051 5603aeab 60794e54 bb7996a7
// Gy = 061b1cf ab6be5f3 2bbfa783 24ed106a 7636b9c5 a7bd198d 0158aa4f 5488d08f 38514f1f df4b4f40 d2181b36 81c364ba 0273c706
// Cofactor f = 2
#define NIST_B_409 EC_Domain_Parameters (409, 2, 87, Curve ("1", "021a5c2 c8ee9feb 5c4b9a75 3b7b476b 7fd6422e f1f3dd67 4761fa99 d6ac27c8 a9a197b2 72822f6c d57a55aa 4f50ae31 7b13545f"), decto_BigInt ("661055968790248598951915308032771039828404682964281219284648798304157774827374805208143723762179110965979867288366567526771"), Point ("15d4860 d088ddb3 496b0c60 64756260 441cde4a f1771d4d b01ffe5b 34e59703 dc255a86 8a118051 5603aeab 60794e54 bb7996a7", "061b1cf ab6be5f3 2bbfa783 24ed106a 7636b9c5 a7bd198d 0158aa4f 5488d08f 38514f1f df4b4f40 d2181b36 81c364ba 0273c706"), decto_BigInt ("2"));


// Field polynomial: p(t) = t^571 + t^10 + t^5 + t^2 + 1
inline void use_NIST_B_571 () { F2X pt=Pentanomial (571, 10, 5, 2, 0); setModulus (pt); };


// Degree 571 Binary Field from fips186-2
// Pseudorandom curve E: y^2 + xy = x^3 + x^2 + b,
// b = 2f40e7e 2221f295 de297117 b7f3d62f 5c6a97ff cb8ceff1 cd6ba8ce 4a9a18ad 84ffabbd 8efa5933 2be7ad67 56a66e29 4afd185a 78ff12aa 520e4de7 39baca0c 7ffeff7f 2955727a
// Base point order:
// r = 3864537523017258344695351890931987344298927329706434998657235251451519142289560424536143999389415773083133881121926944486246872462816813070234528288303332411393191105285703
// Base point G:
// Gx = 303001d 34b85629 6c16c0d4 0d3cd775 0a93d1d2 955fa80a a5f40fc8 db7b2abd bde53950 f4c0d293 cdd711a3 5b67fb14 99ae6003 8614f139 4abfa3b4 c850d927 e1e7769c 8eec2d19
// Gy = 37bf273 42da639b 6dccfffe b73d69d7 8c6c27a6 009cbbca 1980f853 3921e8a6 84423e43 bab08a57 6291af8f 461bb2a8 b3531d2f 0485c19b 16e2f151 6e23dd3c 1a4827af 1b8ac15b
// Cofactor f = 2
#define NIST_B_571 EC_Domain_Parameters (571, 3, 10, 5, 2, Curve ("1", "2f40e7e 2221f295 de297117 b7f3d62f 5c6a97ff cb8ceff1 cd6ba8ce 4a9a18ad 84ffabbd 8efa5933 2be7ad67 56a66e29 4afd185a 78ff12aa 520e4de7 39baca0c 7ffeff7f 2955727a"), decto_BigInt ("3864537523017258344695351890931987344298927329706434998657235251451519142289560424536143999389415773083133881121926944486246872462816813070234528288303332411393191105285703"), Point ("303001d 34b85629 6c16c0d4 0d3cd775 0a93d1d2 955fa80a a5f40fc8 db7b2abd bde53950 f4c0d293 cdd711a3 5b67fb14 99ae6003 8614f139 4abfa3b4 c850d927 e1e7769c 8eec2d19", "37bf273 42da639b 6dccfffe b73d69d7 8c6c27a6 009cbbca 1980f853 3921e8a6 84423e43 bab08a57 6291af8f 461bb2a8 b3531d2f 0485c19b 16e2f151 6e23dd3c 1a4827af 1b8ac15b"), decto_BigInt ("2"));


#endif	// BORZOI_NIST_CURVES_H
