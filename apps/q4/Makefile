default:
	cd ../../os; make
	cd makeprocs; make
	cd consumer; make
	cd producer; make

clean:
	cd makeprocs; make clean
	cd consumer; make clean
	cd producer; make clean
	cd ../../os; make clean


run:
	cd ../../bin; dlxsim -x os.dlx.obj -a -u makeprocs.dlx.obj 3; ee469_fixterminal
