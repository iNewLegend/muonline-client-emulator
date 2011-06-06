## Process this file with automake to produce Makefile.in

EXTRA_DIST = Makefile.tru64 math_mpi.cpp mpi.c mpi.h math_ntl.cpp

INCLUDES = -I$(top_srcdir)/src -I$(prefix)/include

lib_LTLIBRARIES = libborzoi.la

#libborzoiincludedir = $(includedir) $(oldincludedir)
libborzoiincludedir = $(includedir)


libborzoi_la_SOURCES = \
	aes.cpp \
	asn.cpp \
	borzoi.h \
	borzoi_math.h \
	borzoi_util.h \
	curve.cpp \
	ec_key_pair.cpp \
	ecdsa.cpp \
	ecies.cpp \
	eckas_dh1.cpp \
	ec_primitives.cpp \
	hash.cpp \
	nist_curves.h \
	rijndael-alg-fst.c \
	rijndael-alg-fst.h \
	rng.c \
	rng.h \
	utilities.cpp
#	utilities.cpp \
#	@BORZOI_SOURCES@

libborzoiinclude_HEADERS = \
	borzoi.h \
	borzoi_math.h \
	borzoi_util.h \
	mpi.h \
	nist_curves.h \
	rijndael-alg-fst.h \
	rng.h

libborzoi_la_LIBADD = \
	@BORZOI_DEPENDENCIES@

libborzoi_la_DEPENDENCIES = $(libborzoi_la_LIBADD)

libborzoi_la_LDFLAGS = \
	 -version-info 0:0:0 \
	 -rpath $(libdir)

bin_PROGRAMS = borzoi

borzoi_SOURCES = test.cpp

borzoi_LDADD = @BORZOI_LIBS@



