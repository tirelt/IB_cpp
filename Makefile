CXX=g++
CXXFLAGS=-pthread -Wall -Wno-switch -Wpedantic -Wno-unused-function -std=c++11
ROOT_DIR=tws_api/IBJts/source/cppclient
BASE_SRC_DIR=${ROOT_DIR}/client
INCLUDES=-I${BASE_SRC_DIR} -I${ROOT_DIR}
SOURCE_DIR=${BASE_SRC_DIR}

LIB_DIR=$(SOURCE_DIR)/lib
LIB_NAME_A=libbid.a
LIB_NAME_SO=libbid.so
TARGET=MyApp

PATH_SRC=src
PATH_INLUCE=include
PATH_OBJ=obj
PATH_LIB=tws_api/IBJts/source/cppclient
PATH_CLIENT=$(PATH_LIB)/client
PATH_LIBOBJ=$(PATH_CLIENT)/libTwsSocketClient.so

SRCS := $(wildcard $(PATH_SRC)/*.cpp)
OBJS := $(SRCS:$(PATH_SRC)/%cpp=$(PATH_OBJ)/%o)

all: $(TARGET)
	
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) $(PATH_LIBOBJ) -lbidgcc000 -o $(TARGET)

$(PATH_OBJ)/%.o: $(PATH_SRC)/%.cpp $(PATH_INLUCE)/%.h
	${CXX} -c $< -I $(PATH_INLUCE) -I $(PATH_LIB) -I $(PATH_CLIENT) -o $@

run:
	./$(TARGET)

