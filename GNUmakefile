# $Id: GNUmakefile,v 1.1 1999/01/07 16:05:42 gunter Exp $
# --------------------------------------------------------------
# GNUmakefile for examples module.  Gabriele Cosmo, 06/04/98.
# --------------------------------------------------------------
Debug: CPPFLAGS += -g -D -pg -I$(ROOTSYS)/include/
Release: CPPFLAGS += -O3 -I$(ROOTSYS)/include/
EXTRALIBS = $(shell $(ROOTSYS)/bin/root-config --glibs)
name := mrkgeant
G4TARGET := $(name)
G4EXLIB := true
G4INSTALL := /home/mjbales/work/software/geant4/geant4.9.6.p03.install/share/Geant4-9.6.3/geant4make
#CPPVERBOSE := true


.PHONY: all clean
all: lib bin
Debug: lib bin
Release: lib bin
#
