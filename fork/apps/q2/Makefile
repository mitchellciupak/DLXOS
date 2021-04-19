default:
	cd makeprocs; make
	cd p1; make
	cd p2; make
	cd p3; make
	cd p4; make
	cd p6; make

clean:
	cd makeprocs; make clean
	cd p1; make clean
	cd p2; make clean
	cd p3; make clean
	cd p4; make clean
	cd p6; make clean

run:
	cd ../../bin; dlxsim -x os.dlx.obj -a -u makeprocs.dlx.obj; ee469_fixterminal
debug:
	cd ../../bin; dlxsim -D m -x os.dlx.obj -a -D m -u makeprocs.dlx.obj; ee469_fixterminal
