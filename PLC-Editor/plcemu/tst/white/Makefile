
.PHONY :  vm app

all: vm app
	

vm: 
	make -B -C vm -f vm.mk all run report
	
app: 
	make -B -C app -f app.mk all run report


clean: clean_vm  clean_app


clean_vm:
	make -C vm -f vm.mk clean
	
clean_app:
	make -C app -f app.mk clean	
