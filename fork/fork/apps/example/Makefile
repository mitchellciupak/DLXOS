default:
	cd makeprocs; make
	cd hello_world; make
	cd p1; make
	cd p2; make
	cd p3; make
	cd p4; make
	cd p5; make
	cd p6; make
clean:
	cd makeprocs; make clean
	cd hello_world; make clean
	cd p1; make clean
	cd p2; make clean
	cd p3; make clean
	cd p4; make clean
	cd p5; make clean
	cd p6; make clean

run_test1:
	cd ../../bin; dlxsim -x os.dlx.obj -a -u makeprocs.dlx.obj 1 1; ee469_fixterminal

run_test2:
	cd ../../bin; dlxsim -x os.dlx.obj -a -u makeprocs.dlx.obj 2 1; ee469_fixterminal

run_test3:
	cd ../../bin; dlxsim -x os.dlx.obj -a -u makeprocs.dlx.obj 3 1; ee469_fixterminal

run_test4:
	cd ../../bin; dlxsim -x os.dlx.obj -a -u makeprocs.dlx.obj 4 1; ee469_fixterminal

run_test5:
	cd ../../bin; dlxsim -x os.dlx.obj -a -D m -u makeprocs.dlx.obj 5 100; ee469_fixterminal

run_test6:
	cd ../../bin; dlxsim -x os.dlx.obj -a -u makeprocs.dlx.obj 6 30; ee469_fixterminal
