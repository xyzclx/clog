
CC				:= g++
MKDIR			:= mkdir -p
SRC_FILES 		:= test/test.cpp src/clog.cpp
INC_FILES		:= include/clog/clog.h
INC_DIR			:= -Iinclude 
LIBS 			:= -lboost_log_setup -lboost_log -lboost_thread -lboost_date_time -pthread

.PHONY:			all

all:			bin/unit_test

bin/unit_test:		$(SRC_FILES)
	$(MKDIR) bin
	$(CC) $(CPPFLAGS) -o $@ $(INC_DIR) $^ $(LIBS)

$(SRC_FILES):	$(INC_FILES)
	touch $@

run:			bin/unit_test
	./bin/unit_test

