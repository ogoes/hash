#include "client/player.h"

Player::Player(std::string username) {
  this->username = username;
  this->password = std::string("");
  this->connected = false;
}

Player::Player(std::string username, std::string password) {
  this->username = username;
  this->password = password;
  this->connected = false;
}

bool Player::connectToServer(std::string address, long int port) {
  this->serverAddress = address;
  this->serverPort = port;

  this->connectSocket();
  this->signIn(sendMessage, receiveMessage);
  // this->connectBackup();
  // this->signIn(sendMessageToBackup, receiveMessageFromBackup);

  return true;
}

std::string Player::receiveMessage() {
  if (!this->connected) return std::string("");

  char buffer[RCV_BUFFER_SIZE];
  read(this->socketFd, buffer, RCV_BUFFER_SIZE);
  std::cout << "receive" << std::endl;
  return std::string(buffer);
}

std::string Player::receiveMessageFromBackup() {
  if (!this->connected) return std::string("");

  char buffer[RCV_BUFFER_SIZE];
  read(this->backupSocket, buffer, RCV_BUFFER_SIZE);
  return std::string(buffer);
}

bool Player::sendMessage(std::string message) {
  if (!this->connected) return false;

  std::cout << message << std::endl;

  if (send(this->socketFd, message.c_str(), message.size(), 0) < 0) {
    return false;
  }
  return true;
}
bool Player::sendMessageToBackup(std::string message) {
  if (!this->connected) return false;

  send(this->backupSocket, message.c_str(), message.size(), 0);
  return true;
}

bool Player::operator==(const std::string& username) {
  return username == this->username;
}

bool Player::operator==(const char* username) {
  return std::string(username) == this->username;
}

void Player::connectSocket() {
  if ((socketFd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    return;
  }

  SockIn dbAddr;

  dbAddr.sin_family = AF_INET;
  dbAddr.sin_port = htons(this->serverPort);

  if (inet_pton(AF_INET, this->serverAddress.c_str(), &dbAddr.sin_addr) <= 0) {
    return;
  }

  if (connect(this->socketFd, (struct sockaddr*)&dbAddr, sizeof(dbAddr)) < 0) {
    return;
  }

  connected = true;
}

void Player::connectBackup() {
  if ((backupSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    return;
  }

  SockIn dbAddr;

  dbAddr.sin_family = AF_INET;
  dbAddr.sin_port = htons(this->serverPort);

  if (inet_pton(AF_INET, this->serverAddress.c_str(), &dbAddr.sin_addr) <= 0) {
    return;
  }

  if (connect(this->backupSocket, (struct sockaddr*)&dbAddr, sizeof(dbAddr)) <
      0) {
    return;
  }

  connected = connected ? true : false;
}

void Player::signIn(bool (*sender)(Player*, std::string),
                    std::string (*receiver)(Player*)) {
  if (!this->connected) return;

  server_client::LogInSignUp request;
  request.set_username(this->username);
  request.set_password(this->password);

  request.set_request_type(server_client::LogInSignUp::LOGIN);

  if (!sender(this, request.SerializeAsString())) exit(0);

  server_client::LogInSignUpResponse response;

  response.ParseFromString(receiver(this));

  std::cout << response.status().status_code() << std::endl;

  if (response.status().status_code() != 100) this->connected = false;
  return;
}

bool Player::sendMessage(Player* pl, std::string message) {
  return pl->sendMessage(message);
}
bool Player::sendMessageToBackup(Player* pl, std::string message) {
  return pl->sendMessageToBackup(message);
}
std::string Player::receiveMessage(Player* pl) { return pl->receiveMessage(); }
std::string Player::receiveMessageFromBackup(Player* pl) {
  return pl->receiveMessageFromBackup();
}

bool Player::setPlayerInfo(server_client::Player infos) {
  this->playerInfo->Clear();
  this->playerInfo->CopyFrom(infos);
  return true;
}
