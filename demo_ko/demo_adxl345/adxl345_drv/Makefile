obj-m+=adxl345_drv.o
curpath=$(shell pwd)
kerdir=/home/linux/imx6ull/kernel/linux-imx-rel_imx_4.1.15_2.1.0_ga_alientek

all:
	make -C $(kerdir) M=$(curpath) modules
	cp adxl345_drv.ko ~/nfs/rootfs
cp:
	cp adxl345_drv.ko ~/nfs/rootfs
# 声明伪目标，避免同名文件干扰
.PHONY: all clean distclean

# 清理内核模块编译生成的所有文件
clean:
	rm -rf *.o *.ko *.mod.c *.mod.o *.order *.symvers .tmp_versions .*.cmd

# 彻底清理，包括nfs目录的ko文件
distclean: clean
	rm -f ~/nfs/rootfs/$(obj-m:.o=.ko)