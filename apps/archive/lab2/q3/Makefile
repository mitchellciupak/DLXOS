default:
	cd makeprocs; make
	cd cond_spawn; make

clean:
	cd makeprocs; make clean
	cd cond_spawn; make clean

run:
	cd ../../bin; dlxsim -x os.dlx.obj -a -u makeprocs.dlx.obj 3; ee469_fixterminal
