CXX=g++
CXXFLAGS=-pthread -Wall -Wno-switch -Wpedantic -Wno-unused-function -std=c++20
DEBUG_FLAG=-fdiagnostics-color=always -g
TARGET=MyApp
TARGET_TEST=Test
LIB_DIR=lib
LIB_NAME=bid
PATH_SRC=src
PATH_TEST=tws_api/IBJts/samples/Cpp/TestCppClient
PATH_OBJ=obj/src
PATH_OBJ_CLIENT=obj/client
PATH_OBJ_TEST=obj/test
PATH_LIB=tws_api/IBJts/source/cppclient
PATH_CLIENT=$(PATH_LIB)/client
PATH_LIBOBJ=$(LIB_DIR)/libTwsSocketClient.so
INCLUDES=-I${PATH_LIB} -I${PATH_CLIENT}

SRCS := $(wildcard $(PATH_SRC)/*.cpp)
OBJS := $(SRCS:$(PATH_SRC)/%cpp=$(PATH_OBJ)/%o)

SRCS_TEST := $(wildcard $(PATH_TEST)/*.cpp)
OBJS_TEST := $(SRCS_TEST:$(PATH_TEST)/%cpp=$(PATH_OBJ_TEST)/%o)

SRCS_CLIENT := $(wildcard $(PATH_CLIENT)/*.cpp)
OBJS_CLIENT := $(SRCS_CLIENT:$(PATH_CLIENT)/%cpp=$(PATH_OBJ_CLIENT)/%o)

$(TARGET): dir $(OBJS) $(OBJS_CLIENT) 
	$(CXX) $(OBJS) $(OBJS_CLIENT) $(CXXFLAGS) -L$(LIB_DIR) -l$(LIB_NAME) -o$(TARGET)

$(TARGET_TEST): dir $(OBJS_TEST) $(OBJS_CLIENT) 
	$(CXX) $(OBJS_TEST) $(OBJS_CLIENT) $(CXXFLAGS) -L$(LIB_DIR) -l$(LIB_NAME) -o$(TARGET)

$(PATH_OBJ)/%.o: $(PATH_SRC)/%.cpp $(PATH_SRC)/%.h
	${CXX} $(CXXFLAGS) $(DEBUG_FLAG) -c $< $(INCLUDES) -I$(PATH_SRC)  -o $@

$(PATH_OBJ_TEST)/%.o: $(PATH_TEST)/%.cpp $(PATH_TEST)/%.h
	${CXX} $(CXXFLAGS) $(DEBUG_FLAG) -c $< $(INCLUDES) -I$(PATH_TEST)  -o $@

$(PATH_OBJ_CLIENT)/%.o: $(PATH_CLIENT)/%.cpp $(PATH_CLIENT)/%.h
	${CXX} $(CXXFLAGS) $(DEBUG_FLAG) -c $< -I$(INCLUDES) -o $@

dir: 
	mkdir -p $(PATH_OBJ) $(PATH_OBJ_CLIENT) $(PATH_OBJ_TEST)
run:
	./$(TARGET)

clean:
	rm -f $(TARGET) $(PATH_OBJ)/*.o $(PATH_OBJ_CLIENT)/*.o $(PATH_OBJ_TEST)/*.o 

cleanSrc:
	rm -f $(TARGET) $(PATH_OBJ)/*.o

cleanClient:
	rm -f $(PATH_OBJ_CLIENT)/*.o

cleanTest:
	rm -f $(TARGET_TEST) $(PATH_OBJ_TEST)/*.o

profile:
	xcrun xctrace record --template "Time Profiler" --launch MyApp --output MyApp.trace
