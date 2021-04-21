default:
	cd makeprocs; make
	cd test_malloc; make

clean:
	cd makeprocs; make clean
	cd test_malloc; make clean

run:
	cd ../../bin; dlxsim -x os.dlx.obj -a -u makeprocs.dlx.obj 1; ee469_fixterminal
debug:
	cd ../../bin; dlxsim -D m -x os.dlx.obj -a -D m -u makeprocs.dlx.obj 2; ee469_fixterminal
