#include <cstring>
#include <iostream>
#include <string>
#include <vector>

#include "communication/client_server.pb.h"
#include "communication/in_game.pb.h"

#include "server/db_connection.h"


class Player {
 public:
  Player(server_client::Player);
  ~Player();

  void updateInfo(server_client::Player);

  Player* setSocketFd(int);
  Player* setBackupSocketFd(int);


  bool sendMessage(char*);
  bool sendBackupMessage(char*);
  std::string receiveMessage();
  std::string receiveBackupMessage();


  bool operator<(const Player&);
  bool operator==(const Player&);

  bool inGame();
  void inGame(bool);


  bool waiting();
  void waiting(bool);

  in_game::Position::ValueType symbol();
  void symbol(in_game::Position::ValueType);

  in_game::Player getGameInfo();
  server_client::Player getPlayerInfo();

 private:
  int socket;
  int backupSocket;
  in_game::Player* inGameInfo;
  server_client::Player* playerInfo;
  bool isInGame;
  bool isWaiting;
  in_game::Position::ValueType pSymbol;
};
