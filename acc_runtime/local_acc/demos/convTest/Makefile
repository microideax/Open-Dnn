runtime:runtime.o acc_ctrl.o
	g++ -o runtime runtime.o acc_ctrl.o -I ../../api_lib/inc
runtime.o:runtime.cpp ../../api_lib/src/acc_ctrl.cpp
	g++ -c runtime.cpp ../../api_lib/src/acc_ctrl.cpp -I ../../api_lib/inc
acc_ctrl.o:../../api_lib/src/acc_ctrl.cpp
	g++ -c ../../api_lib/src/acc_ctrl.cpp -I ../../api_lib_inc

clean:
	rm -f *.o runtime
