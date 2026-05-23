modulename:=dht11_drv

kerdir:=/home/linux/imx6ull/kernel/linux-imx-rel_imx_4.1.15_2.1.0_ga_alientek


curdir:=$(shell pwd)

obj-m+=$(modulename).o 

all:
	make -C $(kerdir) M=$(curdir) modules
	cp $(modulename).ko ~/nfs/rootfs 

.PHONY:
clean:
	make -C $(kerdir) M=$(curdir) modules clean 
distclean:
	make -C $(kerdir) M=$(curdir) modules clean 
	rm ~/nfs/rootfs/$(modulename).ko
