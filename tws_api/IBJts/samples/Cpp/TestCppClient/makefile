CXX=g++
CXXFLAGS=-pthread -Wall -Wno-switch -Wpedantic -Wno-unused-function -std=c++20
DEBUG_FLAG=-fdiagnostics-color=always -g
ROOT_DIR=../../../source/cppclient
BASE_SRC_DIR=${ROOT_DIR}/client
INCLUDES=-I${BASE_SRC_DIR} -I${ROOT_DIR}
SOURCE_DIR=${BASE_SRC_DIR}
SOURCE_LIB=TwsSocketClient
LIB_DIR=./../../../../../lib
LIB_NAME_A=bid
TARGET=TestCppClient

$(TARGET):
	$(CXX) $(CXXFLAGS) $(DEBUG_FLAG) $(INCLUDES) $(BASE_SRC_DIR)/*.cpp ./*.cpp -L$(LIB_DIR) -l$(LIB_NAME_A) -o$(TARGET)

$(TARGET)Dynamic:
	$(CXX) $(CXXFLAGS) $(INCLUDES) ./*.cpp -L$(LIB_DIR) -l$(LIB_NAME_A) -l$(SOURCE_LIB) -o$(TARGET)Dynamic

run$(TARGET):
	LD_LIBRARY_PATH=$(PWD)/$(LIB_DIR):$(LD_LIBRARY_PATH) ./$(TARGET)

run$(TARGET)Dynamic:
	LD_LIBRARY_PATH=$(PWD)/$(LIB_DIR):$(PWD)/$(SOURCE_DIR):$(LD_LIBRARY_PATH) ./$(TARGET)Dynamic

clean:
	rm -f $(TARGET) $(TARGET)Static $(TARGET)Dynamic *.o

