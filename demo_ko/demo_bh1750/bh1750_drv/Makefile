#模块名
module_name=bh1750_drv

#将文件加入内核模块中
obj-m+=$(module_name).o

#内核路径
kerdir=/home/linux/imx6ull/kernel/linux-imx-rel_imx_4.1.15_2.1.0_ga_alientek

#驱动模块代码路径
curdir=$(shell pwd)

all:
	make -C $(kerdir) modules M=$(curdir)
	cp $(module_name).ko ~/nfs/rootfs

# 声明伪目标，避免同名文件干扰
.PHONY: all clean distclean

# 清理内核模块编译生成的所有文件
clean:
	rm -rf *.o *.ko *.mod.c *.mod.o *.order *.symvers .tmp_versions .*.cmd

# 彻底清理，包括nfs目录的ko文件
distclean: clean
	rm -f ~/nfs/rootfs/$(obj-m:.o=.ko)