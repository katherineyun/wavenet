CC = g++
OBJ = conv_module.o
CFLAGS = -c -Wall
INC = -I/home/myun7/hls4ml-tutorial/model_1/hls4ml_prj/firmware -I/usr/include/c++ -I/opt/xilinx/Vitis/2020.2/aietools/tps/lnx64/gcc/include/c++/6.2.0
TARGET = conv_module

conv: $(OBJ)
	$(CC) $(INC) $(OBJ) -o $(TARGET)

conv_module.o:$(TARGET).cpp
	$(CC) $(CFLAGS) $(TARGET).cpp

clean:
	rm *.o $(TARGET)