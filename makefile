CXX=g++
CXXFLAGS=-pthread -Wall -Wno-switch -Wpedantic -Wno-unused-function -lbidgcc000
TARGET=MyApp

DEBUG_FLAG=-fdiagnostics-color=always -g
PATH_SRC=src
PATH_INLUCE=include
PATH_OBJ=obj/src
PATH_OBJ_CLIENT=obj/client
PATH_LIB=tws_api/IBJts/source/cppclient
PATH_CLIENT=$(PATH_LIB)/client
PATH_LIBOBJ=$(PATH_CLIENT)/libTwsSocketClient.so

SRCS := $(wildcard $(PATH_SRC)/*.cpp)
OBJS := $(SRCS:$(PATH_SRC)/%cpp=$(PATH_OBJ)/%o)

SRCS_CLIENT := $(wildcard $(PATH_CLIENT)/*.cpp)
OBJS_CLIENT := $(SRCS_CLIENT:$(PATH_CLIENT)/%cpp=$(PATH_OBJ_CLIENT)/%o)

all: $(TARGET)

$(TARGET): $(OBJS) $(OBJS_CLIENT)
	$(CXX) $(OBJS) $(OBJS_CLIENT) $(CXXFLAGS) -o $(TARGET)

$(PATH_OBJ)/%.o: $(PATH_SRC)/%.cpp $(PATH_INLUCE)/%.h
	${CXX} $(DEBUG_FLAG) -c $< -I $(PATH_INLUCE) -I $(PATH_LIB) -I $(PATH_CLIENT) -o $@

$(PATH_OBJ_CLIENT)/%.o: $(PATH_CLIENT)/%.cpp $(PATH_CLIENT)/%.h
	${CXX} $(DEBUG_FLAG) -c $< -I $(PATH_LIB) -I $(PATH_CLIENT) -I $(PATH_INLUCE) -o $@

run:
	./$(TARGET)

clean:
	rm -f $(TARGET) $(PATH_OBJ)/*.o $(PATH_OBJ_CLIENT)/*.o

cleanSrc:
	rm -f $(TARGET) $(PATH_OBJ)/*.o

cleanClient:
	rm -f $(TARGET) $(PATH_OBJ_CLIENT)/*.o


