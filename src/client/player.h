
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

// socket includes
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "communication/client_server.pb.h"

#define SockAddress struct sockaddr
#define SockIn struct sockaddr_in

#define RCV_BUFFER_SIZE (1024 * 8)

typedef struct {
  int sockFd;
  SockIn in;
  int inLen;
} SockAttr;

class Player {
 private:
  void connectSocket();
  void connectBackup();
  void signIn(bool (*)(Player*, std::string), std::string (*)(Player*));
  void signInBackup();

 public:
  static bool sendMessage(Player*, std::string);
  static bool sendMessageToBackup(Player*, std::string);

  static std::string receiveMessage(Player*);
  static std::string receiveMessageFromBackup(Player*);

  Player(std::string username);
  Player(std::string username, std::string password);

  bool connectToServer(std::string, long int);

  std::string receiveMessage();
  std::string receiveMessageFromBackup();

  bool sendMessage(std::string);
  bool sendMessageToBackup(std::string);

  bool operator==(const std::string&);
  bool operator==(const char*);

  bool setPlayerInfo(server_client::Player);

 private:
  bool connected;

  int socketFd;
  int backupSocket;

  std::string username;
  std::string password;

  std::string serverAddress;
  long int serverPort;

  server_client::Player* playerInfo;
};
