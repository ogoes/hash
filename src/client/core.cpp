#include "client/core.h"

Core::Core() {}

Core::~Core() {}

void Core::createPlayer(std::string username, std::string password) {
  Core::player = std::make_shared<Player>(username, password);
}

bool Core::connectToServer(std::string address, long int port) {
  return player->connectToServer(address, port);
}

std::vector<server_client::Player> Core::ranking() {
  server_client::Operation request;
  request.set_operation(server_client::Operation::RANKING);

  player->sendMessage(request.SerializeAsString());

  server_client::Ranking response;
  response.ParseFromString(player->receiveMessage());
  std::cout << "recebeu" << std::endl;

  std::vector<server_client::Player> ret;

  auto players = response.players();

  for (auto it = players.begin(); it != players.end(); ++it) {
    ret.push_back(*it);
  }

  return ret;
}
std::vector<server_client::Player> Core::online() {
  server_client::Operation request;
  request.set_operation(server_client::Operation::ONLINE);

  server_client::OnlinePlayers response;
  response.ParseFromString(player->receiveMessage());

  std::vector<server_client::Player> ret;

  auto players = response.players();

  for (auto it = players.begin(); it != players.end(); ++it) {
    ret.push_back(*it);
  }

  return ret;
}
server_client::Player Core::view() {
  server_client::Operation request;
  request.set_operation(server_client::Operation::VIEW);

  player->sendMessage(request.SerializeAsString());

  server_client::View viewer;
  viewer.ParseFromString(player->receiveMessage());

  player->setPlayerInfo(viewer.player());
  return viewer.player();
}

in_game::State Core::hit() {}
in_game::Result Core::result() {}
