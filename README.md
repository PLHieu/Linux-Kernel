# Linux-Kernel
#Move to the container directory

#Run makefile to create randomdriver.ko, command:
make

#Add driver to kernel, command:
sudo insmod randomdriver.ko

#To test, build test.c using command:
gcc test.c -o test

#Test using root permission, command:
sudo ./test
