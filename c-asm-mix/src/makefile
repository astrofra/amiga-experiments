.prl.gst:
	sc NOLINK CODE=F DATA=F OPTI MGST $*.gst $*.prl

.c.o:
	sc NOLINK CODE=F DATA=F OPTI TO $@ $*.c

OBJ = main.o mul_by_ten.o

main.exe: $(OBJ) mul_by_ten.s includes.gst
	sc LINK CODE=F DATA=F OBJ $(OBJ) TO main.exe

includes.gst: includes.prl
main.o: main.c includes.gst
mul_by_ten.o: mul_by_ten.s