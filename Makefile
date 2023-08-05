mname := primeDev
$(mname)-objs := main.o
obj-m := $(mname).o

KERNELDIR := /lib/modules/`uname -r`/build

all:
	$(MAKE) -C $(KERNELDIR) M=`pwd` modules
	sudo insmod primeDev.ko
	gcc -o ./test_files/test ./test_files/test.c

clean:
	sudo rmmod primeDev
	$(MAKE) -C $(KERNELDIR) M=`pwd` clean
	rm ./test_files/test
	sudo dmesg | grep primeDev


