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


#ifdef _WIN32
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0400
#endif
#include <windows.h>
#include <wincrypt.h>
#else
#include <fcntl.h>
#include <unistd.h>
#endif
#include "rng.h"


unsigned long gen_random () {
	unsigned char bytes[4];
	unsigned long data;

#ifdef _WIN32
	HCRYPTPROV hCryptProv;

	CryptAcquireContext(&hCryptProv, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT);
	CryptGenRandom(hCryptProv, 4, bytes);
	if (hCryptProv)
		CryptReleaseContext(hCryptProv, 0);
#else
	int rand_fd;

	rand_fd = open ("/dev/random", O_RDONLY);
	read (rand_fd, &bytes, 4);
	close (rand_fd);

#endif

	data |= bytes[0];
	data *= 0x100;
	data |= bytes[1];
	data *= 0x100;
	data |= bytes[2];
	data *= 0x100;
	data |= bytes[3];

	return data;
}


