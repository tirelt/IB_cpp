CXX=g++
CXXFLAGS=-pthread -Wall -Wno-switch -Wpedantic -Wno-unused-function -std=c++11
ROOT_DIR=tws_api/IBJts/source/cppclient
BASE_SRC_DIR=${ROOT_DIR}/client
INCLUDES=-I${BASE_SRC_DIR} -I${ROOT_DIR}
SOURCE_DIR=${BASE_SRC_DIR}
SOURCE_LIB=libTwsSocketClient.so
LIB_DIR=$(SOURCE_DIR)/lib
LIB_NAME_A=libbid.a
LIB_NAME_SO=libbid.so
TARGET=MyApp

$(TARGET)Static:
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(BASE_SRC_DIR)/*.cpp ./src/*.cpp -lbidgcc000 -o$(TARGET)Static
	
$(TARGET)Dynamic:
	$(CXX) $(CXXFLAGS) $(INCLUDES)  ./src/*.cpp -lbidgcc000 -L$(SOURCE_DIR) -l:$(SOURCE_LIB) -o$(TARGET)Dynamic

run$(TARGET)Static:
	./$(TARGET)Static

run$(TARGET)Dynamic:
	LD_LIBRARY_PATH=$(PWD)/$(SOURCE_DIR):$(LD_LIBRARY_PATH) ./$(TARGET)Dynamic

clean:
	rm -f $(TARGET) $(TARGET)Static $(TARGET)Dynamic *.o

