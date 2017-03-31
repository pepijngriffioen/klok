GCC = g++ -ggdb -Wall
INCLUDEDIR = -I/usr/include/X11
LIB = -lX11
LIBDIR = -L/usr/X11R6/lib/

klok: klok.o Xsowl.o
	$(GCC) -o klok klok.o Xsowl.o $(LIBDIR) $(LIB)
klok.o: klok.cc Xsowl.h
	$(GCC) -c klok.cc $(INCLUDEDIR)
Xsowl.o: Xsowl.cc Xsowl.h Xsowldef.h
	$(GCC) -c Xsowl.cc $(INCLUDEDIR)
