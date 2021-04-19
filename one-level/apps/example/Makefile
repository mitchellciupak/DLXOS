default:
	cd makeprocs; make
	cd hello_world; make

clean:
	cd makeprocs; make clean
	cd hello_world; make clean

run:
	cd ../../bin; dlxsim -x os.dlx.obj -a -u makeprocs.dlx.obj 1; ee469_fixterminal
debug:
	cd ../../bin; dlxsim -D m -x os.dlx.obj -a -D m -u makeprocs.dlx.obj 1; ee469_fixterminal
