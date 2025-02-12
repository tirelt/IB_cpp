CXX=g++
CXXFLAGS=-pthread -Wall -Wno-switch -Wpedantic -Wno-unused-function -std=c++20
DEBUG_FLAG=-fdiagnostics-color=always -g
TARGET=MyApp

LIB_DIR=lib
LIB_NAME=bid
PATH_SRC=src
PATH_OBJ=obj/src
PATH_OBJ_CLIENT=obj/client
PATH_LIB=tws_api/IBJts/source/cppclient
PATH_CLIENT=$(PATH_LIB)/client
PATH_LIBOBJ=$(LIB_DIR)/libTwsSocketClient.so

SRCS := $(wildcard $(PATH_SRC)/*.cpp)
OBJS := $(SRCS:$(PATH_SRC)/%cpp=$(PATH_OBJ)/%o)

SRCS_CLIENT := $(wildcard $(PATH_CLIENT)/*.cpp)
OBJS_CLIENT := $(SRCS_CLIENT:$(PATH_CLIENT)/%cpp=$(PATH_OBJ_CLIENT)/%o)

all: $(TARGET)

$(TARGET): dir $(OBJS) $(OBJS_CLIENT)
	$(CXX) $(OBJS) $(OBJS_CLIENT) $(CXXFLAGS) -L$(LIB_DIR) -l$(LIB_NAME) -o $(TARGET)

$(PATH_OBJ)/%.o: $(PATH_SRC)/%.cpp $(PATH_INLUCE)/%.h
	${CXX} $(CXXFLAGS) $(DEBUG_FLAG) -c $< -L$(LIB_DIR) -l$(LIB_NAME) -I$(PATH_CLIENT) -I$(PATH_INLUCE) -o $@

$(PATH_OBJ_CLIENT)/%.o: $(PATH_CLIENT)/%.cpp $(PATH_CLIENT)/%.h
	${CXX} $(CXXFLAGS) $(DEBUG_FLAG) -c $< -L$(LIB_DIR) -l$(LIB_NAME) -I$(PATH_CLIENT) -o $@

dir: 
	mkdir -p $(PATH_OBJ) $(PATH_OBJ_CLIENT)
run:
	./$(TARGET)

clean:
	rm -f $(TARGET) $(PATH_OBJ)/*.o $(PATH_OBJ_CLIENT)/*.o

cleanSrc:
	rm -f $(TARGET) $(PATH_OBJ)/*.o

cleanClient:
	rm -f $(TARGET) $(PATH_OBJ_CLIENT)/*.o


