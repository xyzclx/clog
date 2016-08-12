
CC				:= g++
SRC_FILES 		:= test/test.cpp src/clog.cpp
INC_FILES		:= include/clog/clog.h
INC_DIR			:= -Iinclude 
LIBS 			:= -lboost_log_setup -lboost_log -lboost_thread -lboost_date_time -pthread
CPPFLAGS		:= -DBOOST_LOG_DYN_LINK 

.PHONY:			all

all:			unit_test

unit_test:		$(SRC_FILES)
	$(CC) $(CPPFLAGS) -o $@ $(INC_DIR) $^ $(LIBS)

$(SRC_FILES):	$(INC_FILES)

run:			unit_test
	./unit_test

