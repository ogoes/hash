COMP = g++
FLAGS = -I ../ `pkg-config --cflags --libs sqlite3 protobuf` --std=c++17

LIBS = ./database.cpp ./handler.cpp ../communication/server_database.pb.cc
MODELS = ./player_model.cpp ./game_model.cpp

all:
	$(COMP) $(FLAGS) $(LIBS) $(MODELS) db_controller.cpp -o db_daemon

