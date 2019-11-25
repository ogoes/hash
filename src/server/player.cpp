
#include "server/player.h"

Player::Player(server_client::Player infos) {
  playerInfo = new server_client::Player(infos);
  inGameInfo = new in_game::Player();
  inGameInfo->set_id(playerInfo->id());
  inGameInfo->set_username(playerInfo->username());
}

Player::~Player() {
  delete inGameInfo;
  delete playerInfo;
}

Player* Player::setSocketFd(int fd) {
  socket = fd;
  return this;
}

Player* Player::setBackupSocketFd(int fd) {
  backupSocket = fd;
  return this;
}

bool Player::sendMessage(char* message) {
  send(socket, message, strlen(message), 0);
  return true;
}

std::string Player::receiveMessage() {
  char buffer[RCV_BUFFER_SIZE];
  read(socket, buffer, RCV_BUFFER_SIZE);

  return std::string(buffer);
}

bool Player::sendBackupMessage(char* message) {
  send(backupSocket, message, strlen(message), 0);
  return true;
}

std::string Player::receiveBackupMessage() {
  char buffer[RCV_BUFFER_SIZE];
  read(backupSocket, buffer, RCV_BUFFER_SIZE);

  return std::string(buffer);
}

bool Player::operator<(const Player& other) {
  const server_client::Player infos = *other.playerInfo;

  if (playerInfo->wins() < infos.wins()) return true;
  if (playerInfo->wins() > infos.wins()) return false;

  if (playerInfo->lost() < infos.lost()) return true;
  if (playerInfo->wins() > infos.wins()) return false;

  return playerInfo->tieds() < infos.tieds();
}

bool Player::operator==(const Player& other) {
  const server_client::Player infos = *other.playerInfo;

  return playerInfo->username() == infos.username();
}

in_game::Player Player::getGameInfo() { return *inGameInfo; }

server_client::Player Player::getPlayerInfo() { return *playerInfo; }

void Player::updateInfo(server_client::Player infos) {
  delete playerInfo;
  delete inGameInfo;

  playerInfo = new server_client::Player(infos);
  inGameInfo = new in_game::Player();
  inGameInfo->set_id(playerInfo->id());
  inGameInfo->set_username(playerInfo->username());
}

bool Player::inGame() { return isInGame; }
void Player::inGame(bool is) { isInGame = is; }

bool Player::waiting() { return isWaiting; }
void Player::waiting(bool is) { isWaiting = is; }

in_game::Position::ValueType Player::symbol() { return this->pSymbol; }
void Player::symbol(in_game::Position::ValueType sym) { this->pSymbol = sym; }
