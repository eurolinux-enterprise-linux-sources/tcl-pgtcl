# pgtclng/Makefile for MinGW32 (gcc-4.5.x)
# $Id: mingw.mak 299 2011-09-19 00:54:51Z lbayuk $
# This is a simple Makefile for building pgtclng using the MinGW tools on
# Windows.
# You must edit the PGSQL and TCL symbols below, at least, before using.
#
# This builds a stubs-enabled DLL for pgtcl-ng. If you want a non-stubs-
# enabled version, remove the definition of USE_TCL_STUBS and change the
# TCLLIB definition below.
#
# When loading the built libpgtcl.dll into a Tcl shell or application,
# libpq.dll must be on your PATH.

PACKAGE_VERSION = 2.0.0

# Path to PostgreSQL top-level installation directory:
PGSQL=e:/local/pgsql

# Path to Tcl installation base directory:
TCL=c:/local/tcl8.5
# Tcl stub library name:
TCLLIB=tclstub85

# PostgreSQL version tests:
# If you are building with a version older than PostgreSQL-8.3.0 you
# may need to undefine some of these.
PGDEFS=\
 -DHAVE_LO_TRUNCATE=1\
 -DHAVE_PQDESCRIBEPREPARED=1\
 -DHAVE_PQENCRYPTPASSWORD=1\
 -DHAVE_PQESCAPEBYTEACONN=1\
 -DHAVE_PQESCAPESTRINGCONN=1

# Stubs enabled:
STUBS=-DUSE_TCL_STUBS

# ===========================

PG_INCLUDES = -I"$(PGSQL)/include"
PG_LIBS = -L"$(PGSQL)/lib" -lpq
TCL_INCLUDES = -I"$(TCL)/include"
TCL_LIBS = -L"$(TCL)/lib" -l$(TCLLIB)
OBJS = pgtcl.o pgtclCmds.o pgtclId.o pgtclres.o

CC = gcc
CFLAGS_EXTRA = -O2 -Wall
# Note: enable-runtime-pseudo-reloc-v2 option is via the MinGW mailing
# list. Without it, programs crash when calling PQisnonblocking.
# enable-auto-import avoids a warning related to that same function.
LDFLAGS_EXTRA = -Wl,-enable-runtime-pseudo-reloc-v2 -Wl,-enable-auto-import

INCLUDES =  -I"$(PGSQL)/include" -I"$(TCL)/include"

DEFS = -DPACKAGE_VERSION=\"$(PACKAGE_VERSION)\" $(PGDEFS) $(STUBS)

CFLAGS = $(CFLAGS_EXTRA) $(INCLUDES) $(DEFS)

LDFLAGS = $(LDFLAGS_EXTRA)

all: dll

dll: $(OBJS)
	$(CC) -shared -o libpgtcl.dll $(LDFLAGS) $(OBJS) $(TCL_LIBS) $(PG_LIBS)

pgtclres.o: pgtclres.rc
	windres pgtclres.rc pgtclres.o

clean:
	-erase $(OBJS)
