default:
	cd ../../os; make
	cd makeprocs; make
	cd inject_h2o; make
	cd inject_so4; make
	cd react_h2so4; make
	cd react_h2x2_o2; make
	cd react_so2_o2; make

clean:
	cd makeprocs; make clean
	cd inject_h2o; make clean
	cd inject_so4; make clean
	cd react_h2so4; make clean
	cd react_h2x2_o2; make clean
	cd react_so2_o2; make clean
	cd ../../os; make clean


run:
	cd ../../bin; dlxsim -x os.dlx.obj -a -u makeprocs.dlx.obj 3 3; ee469_fixterminal
