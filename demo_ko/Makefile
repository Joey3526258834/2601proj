# 声明所有伪目标，避免同名文件干扰执行
.PHONY: all clean distclean

# ------------------------------
# 原编译逻辑完全不变，执行make和之前效果一模一样
# ------------------------------
all:
	make -C demo_adxl345
	make -C demo_bh1750

# ------------------------------
# 增强版clean：同时清理子目录 + 顶层目录
# ------------------------------
clean:
	# 1. 递归清理所有中间层子目录（会自动调用子目录的clean，再到最内层驱动）
	make -C demo_adxl345 clean
	make -C demo_bh1750 clean
	
	# 2. 清理当前顶层demo_ko目录的所有内核编译垃圾文件
	rm -rf *.o *.ko *.mod.c *.mod.o *.order *.symvers .tmp_versions .*.cmd

# ------------------------------
# 修复版distclean：彻底清理所有内容
# ------------------------------
distclean: clean
	# 递归执行所有子目录的彻底清理
	make -C demo_adxl345 distclean
	make -C demo_bh1750 distclean
	
	# 删除NFS根文件系统中所有的驱动模块文件（原bug修复：不再依赖未定义的modulename）
	rm -f ~/nfs/rootfs/*.ko
