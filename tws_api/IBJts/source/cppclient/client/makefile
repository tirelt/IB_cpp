CXX=g++
CXXFLAGS=-pthread -Wall -Wno-switch -Wno-unused-function -std=c++11 -shared -fPIC
ROOT_DIR=.
BASE_SRC_DIR=${ROOT_DIR}
INCLUDES=-I${ROOT_DIR}
LIB_DIR=lib
LIB_NAME=libbid.so
TARGET=libTwsSocketClient.so

$(TARGET):
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(BASE_SRC_DIR)/*.cpp -lbidgcc000 -o$(TARGET)

clean:
	rm -f $(TARGET) *.o

