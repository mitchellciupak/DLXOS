default:
	cd ../../os; make
	cd makeprocs; make
	cd s2_inj; make
	cd s_rea; make
	cd co_rea; make
	cd so4_rea; make
	cd co_inj; make

clean:
	cd makeprocs; make clean
	cd s2_inj; make clean
	cd co_inj; make clean
	cd s_rea; make clean
	cd co_rea; make clean
	cd so4_rea; make clean
	cd ../../os; make clean


run:
	cd ../../bin; dlxsim -x os.dlx.obj -a -u makeprocs.dlx.obj 3 8; ee469_fixterminal

all:
	make clean
	make
	make run
	
