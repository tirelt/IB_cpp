CXX=g++
CXXFLAGS=-pthread -Wall -Wno-switch -Wno-unused-function -std=c++20 -shared -fPIC
ROOT_DIR=.
BASE_SRC_DIR=${ROOT_DIR}
INCLUDES=${ROOT_DIR}
LIB_DIR=${ROOT_DIR}
LIB_DIR=./../../../../../lib
LIB_NAME=bid
TARGET=libTwsSocketClient.so

$(LIB_DIR)/lib/$(TARGET):
	$(CXX) $(CXXFLAGS) -I$(INCLUDES) $(BASE_SRC_DIR)/*.cpp -L${LIB_DIR} -l$(LIB_NAME) -o$(TARGET)
	mv $(TARGET) $(LIB_DIR)/$(TARGET)

clean:
	rm -f $(LIB_DIR)/$(TARGET) *.o

