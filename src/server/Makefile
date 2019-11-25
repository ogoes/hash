COMP = g++
FLAGS = -I ../ `pkg-config --cflags --libs protobuf` --std=c++17

LIBS = ./db_connection.cpp ./game.cpp ./handler.cpp ../communication/server_database.pb.cc ../communication/in_game.pb.cc ../communication/client_server.pb.cc ./player.cpp

all:
	$(COMP) $(FLAGS) $(LIBS) server.cpp -o server


log:
	$(COMP) -D LOG $(FLAGS) $(LIBS)  server.cpp -o server

