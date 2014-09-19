CXX = g++
CXXFLAGS = -c -std=c++11 -Wall -Wextra
LIB_DIR = .
INCLUDE_DIR = .
OBJS = obj/main.o obj/command_line_interface.o obj/context.o obj/database_proxy.o obj/interface.o obj/utility.o

bin/sam: $(OBJS) | bin
	$(CXX) -g -o $@ $(OBJS) -L$(LIB_DIR) -lmysqlcppconn -lssl -lcrypto

obj/main.o: src/main.cpp src/context.h | obj
	$(CXX) $(CXXFLAGS) -o $@ $<

obj/command_line_interface.o: src/command_line_interface.cpp src/command_line_interface.h | obj
	$(CXX) $(CXXFLAGS) -o $@ $<

obj/context.o: src/context.cpp src/context.h src/interface.h | obj
	$(CXX) $(CXXFLAGS) -o $@ $<

obj/database_proxy.o: src/database_proxy.cpp src/database_proxy.h | obj
	$(CXX) $(CXXFLAGS) -I$(INCLUDE_DIR) -o $@ $<

obj/interface.o: src/interface.cpp src/context.h src/database_proxy.h src/interface.h src/utility.h | obj
	$(CXX) $(CXXFLAGS) -o $@ $<

obj/utility.o: src/utility.cpp src/utility.h src/common.h
	$(CXX) $(CXXFLAGS) -o $@ $<

src/command_line_interface.h: src/context.h
src/context.h: src/common.h
src/database_proxy.h: src/common.h
src/interface.h: src/common.h
src/utility.h: src/common.h
src/common.h:

bin:
	mkdir bin

obj:
	mkdir obj

clean:
	-rm obj/*.o
