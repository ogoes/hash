COMP = g++
FLAGS = -I ../ `pkg-config --cflags --libs protobuf` `wx-config --cxxflags --libs`

LIBS =  ./player.cpp ./core.cpp ./interface.cpp ../communication/client_server.pb.cc ../communication/in_game.pb.cc

all:
	$(COMP) $(FLAGS) $(LIBS) client.cpp -o client


log:
	$(COMP) -D LOG $(FLAGS) $(LIBS)  -o client

core: player
	$(COMP) $(FLAGS) core.cpp player.o ../communication/in_game.pb.cc -a -o core.o


player:
	$(COMP) $(FLAGS) core.cpp  ../communication/in_game.pb.cc -a 

