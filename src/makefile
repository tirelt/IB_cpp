CXX=g++
ROOT_DIR=../tws_api/IBJts/source/cppclient
BASE_SRC_DIR=${ROOT_DIR}/client
INCLUDES=-I${BASE_SRC_DIR} -I${ROOT_DIR}
SOURCE_DIR=${BASE_SRC_DIR}
TARGET=MyApp

$(TARGET)Static:
	$(CXX) $(INCLUDES) $(BASE_SRC_DIR)/*.cpp ./*.cpp -lbidgcc000 -o$(TARGET)Static
	
clean:
	rm -f $(TARGET) $(TARGET)Static $(TARGET)Dynamic *.o

