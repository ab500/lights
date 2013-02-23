#ARCH=arm
#CROSS_COMPILE=arm-linux-gnueabi-gcc
DRIVER = spike

TARGET = xmaspi
OBJS = xmas.o

obj-m += $(TARGET).o
xmaspi-objs = xmas.o

# Set this is your linux kernel checkout.
KDIR := /home/androbin/rpi/linux
PWD := $(shell pwd)

default:
		$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
		$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) clean

.PHONY: clean default
