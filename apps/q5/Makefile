default:
	cd ../../os; make
	cd makeprocs; make
	cd injector; make
	cd reactor; make

clean:
	cd makeprocs; make clean
	cd injector; make clean
	cd reactor; make clean
	cd ../../os; make clean


run:
	cd ../../bin; dlxsim -x os.dlx.obj -a -u makeprocs.dlx.obj 3 3; ee469_fixterminal
