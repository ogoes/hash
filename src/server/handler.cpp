#include "server/handler.h"

bool compare(server_db::Player p1, server_db::Player p2) {
  if (p1.wins() < p2.wins()) return true;
  if (p1.wins() > p2.wins()) return false;

  if (p1.lost() < p2.lost()) return true;
  if (p1.wins() > p2.wins()) return false;

  return p1.tieds() < p2.tieds();
}

void Handler::Handle(std::string signInResponseAsString, SockAttr socketAttr) {
  server_client::LogInSignUpResponse signInResponse;
  signInResponse.ParseFromString(signInResponseAsString);

  // if (signInResponse.status().status_code() != 100) return;

  server_client::Player pbPlayer = signInResponse.player();

  Player player(pbPlayer);

  if (signInResponse.already_logged()) {
    for (auto it = players.begin(); it != players.end(); ++it) {
      if ((*it) == player) {
        (*it).setBackupSocketFd(socketAttr.sockFd);

        (*it).receiveBackupMessage();  // ACK

        server_client::Response res;
        res.set_message("Todo certo!");
        res.set_status_code(200);

        (*it).sendBackupMessage((char*)res.SerializeAsString().c_str());
        logging("PLAYER LOGGADO");
        return;
      }
    }
  }

  for (auto it = players.begin(); it != players.end(); ++it) {
    std::cout << (*it).getPlayerInfo().username() << std::endl;
  }

  logging("PLAYER");

  player.setSocketFd(socketAttr.sockFd);
  std::string recvMessage = player.receiveMessage();

  logging(recvMessage);

  server_client::Operation operation;
  if (recvMessage.size() > 0) {
    operation.ParseFromString(recvMessage);
    // players.push_back(player);
  } else {
    return;
  }

  while (operation.operation() != server_client::Operation::EXIT) {
    std::string response;
    std::cout << operation.operation() << std::endl;
    switch (operation.operation()) {
      case server_client::Operation::RANKING:
        response = ranking();

        break;
      case server_client::Operation::PLAY:
        response = play(player);

        break;
      case server_client::Operation::ONLINE:
        response = online();

        break;
      case server_client::Operation::VIEW:
        response = view(player);

        break;
      case server_client::Operation::CHANGE:
        response = change(player);

        break;
      default:
        break;
    }

    player.sendMessage((char*)response.c_str());

    recvMessage = player.receiveMessage();

    if (recvMessage.size() > 0) {
      operation.ParseFromString(recvMessage);
    } else
      break;
  }
  auto it = std::find(players.begin(), players.end(), player);
  players.erase(it);

  return;
}

std::string Handler::signUp(std::string username, std::string password) {
  server_db::Request request;

  request.set_entity(server_db::Request::PLAYER);
  request.set_operation(server_db::Request::RETRIEVE);

  server_db::Player* player = request.mutable_player();
  // player->set_id(0);
  player->set_username(username);
  player->set_password(password);

  int sock = DatabaseCommunication::sendRequest(request);

  if (sock) {
    server_db::PlayerResponse response =
        DatabaseCommunication::receiveFromPlayerRequest(sock);

    int status_code = response.status().status_code();
    if (status_code == 200) {  // found -> logIn
      return logIn(username, password);
    }

    request.set_operation(server_db::Request::CREATE);
    sock = DatabaseCommunication::sendRequest(request);
    if (sock) {
      server_db::PlayerResponse response =
          DatabaseCommunication::receiveFromPlayerRequest(sock);

      server_client::LogInSignUpResponse logInResponse;
      server_client::Response* status = logInResponse.mutable_status();

      status_code = response.status().status_code();
      if (status_code == 201) {
        status->set_status_code(100);
        status->set_message("Player ativo!");

        server_client::Player* player = logInResponse.mutable_player();
        player->set_username(username);
        player->set_password(password);
        player->set_wins(0);
        player->set_tieds(0);
        player->set_lost(0);

        players.push_back(Player(*player));

      } else {
        status->set_status_code(status_code);
        status->set_message(response.status().message());
      }

      return logInResponse.SerializeAsString();
    }
  }

  return std::string("");
}

std::string Handler::logIn(std::string username, std::string password) {
  server_db::Request request;

  request.set_entity(server_db::Request::PLAYER);
  request.set_operation(server_db::Request::RETRIEVE);

  server_db::Player* player = request.mutable_player();
  // player->set_id(0);
  player->set_username(username);

  int sock = DatabaseCommunication::sendRequest(request);

  if (sock) {
    server_db::PlayerResponse response =
        DatabaseCommunication::receiveFromPlayerRequest(sock);

    int status_code = response.status().status_code();
    if (status_code == 404) {  // not found -> signUp
      return signUp(username, password);
    }

    server_client::LogInSignUpResponse logInResponse;
    server_client::Response* status = logInResponse.mutable_status();

    status->set_status_code(100);
    status->set_message("Player ativo!");

    server_client::Player* playerResponse = logInResponse.mutable_player();
    playerResponse->set_username(response.player().at(0).username());
    playerResponse->set_password(response.player().at(0).password());
    playerResponse->set_tieds(response.player().at(0).tieds());
    playerResponse->set_wins(response.player().at(0).wins());
    playerResponse->set_lost(response.player().at(0).lost());

    Player isLogged(*playerResponse);

    logInResponse.set_already_logged(false);

    for (auto it = players.begin(); it != players.end(); ++it) {
      if ((*it) == isLogged) {
        logInResponse.set_already_logged(true);
      }
    }

    if (!logInResponse.already_logged()) players.push_back(isLogged);

    return logInResponse.SerializeAsString();
  }

  return std::string("");
}

std::string Handler::ranking() {
  server_client::Ranking rankingResponse;

  server_db::Request request;

  request.set_entity(server_db::Request::PLAYER);
  request.set_operation(server_db::Request::RETRIEVE);

  server_db::Player* player = request.mutable_player();

  int sock = DatabaseCommunication::sendRequest(request);

  if (sock) {
    server_db::PlayerResponse response =
        DatabaseCommunication::receiveFromPlayerRequest(sock);

    server_client::Response* status = rankingResponse.mutable_status();

    int status_code = response.status().status_code();
    status->set_status_code(status_code);
    status->set_message(response.status().message());

    auto db_players = response.player();

    std::sort(db_players.begin(), db_players.end(), compare);

    for (auto it = db_players.begin(); it != db_players.end(); ++it) {
      server_client::Player* responsePlayer = rankingResponse.add_players();
      responsePlayer->CopyFrom((*it));
    }
  }

  return rankingResponse.SerializeAsString();
}

std::string Handler::play(Player& player) {
  server_client::Response ack;
  ack.set_message("100");
  ack.set_status_code(100);

  player.sendMessage((char*)ack.SerializeAsString().c_str());

  server_client::Play request;

  request.ParseFromString(player.receiveMessage());

  std::string response;

  switch (request.request_type()) {
    case server_client::Play::WAIT:
      break;
    case server_client::Play::CHALLENGE:
      Player challended(request.challended_player());

      auto it = std::find(players.begin(), players.end(), challended);
      response = inGame::challenge(player, (*it));

      break;
  }

  return response;
}

std::string Handler::online() {
  server_client::OnlinePlayers onlinePlayers;

  server_client::Response* status = onlinePlayers.mutable_status();

  status->set_status_code(200);
  status->set_message("Players encaminhados!");

  for (auto it = players.begin(); it != players.end(); ++it) {
    if (!(*it).inGame()) {
      server_client::Player* responsePlayer = onlinePlayers.add_players();

      responsePlayer->CopyFrom((*it).getPlayerInfo());
    }
  }

  return onlinePlayers.SerializeAsString();
}

std::string Handler::view(Player& player) {
  server_client::View playerResponse;

  server_db::Request request;

  request.set_entity(server_db::Request::PLAYER);
  request.set_operation(server_db::Request::RETRIEVE);

  server_db::Player* requestPlayer = request.mutable_player();

  requestPlayer->set_username(player.getPlayerInfo().username());
  requestPlayer->set_password(player.getPlayerInfo().password());
  requestPlayer->set_tieds(player.getPlayerInfo().tieds());
  requestPlayer->set_wins(player.getPlayerInfo().wins());
  requestPlayer->set_lost(player.getPlayerInfo().lost());
  requestPlayer->CopyFrom(player.getPlayerInfo());

  int sock = DatabaseCommunication::sendRequest(request);

  if (sock) {
    server_db::PlayerResponse response =
        DatabaseCommunication::receiveFromPlayerRequest(sock);

    server_client::Response* status = playerResponse.mutable_status();

    status->CopyFrom(response.status());

    auto db_players = response.player();

    server_client::Player* pl = playerResponse.mutable_player();
    pl->CopyFrom(db_players.at(0));
    player.updateInfo(*pl);
  }

  return playerResponse.SerializeAsString();
}

std::string Handler::change(Player& player) {
  server_client::View playerResponse;

  server_db::Request request;

  request.set_entity(server_db::Request::PLAYER);
  request.set_operation(server_db::Request::RETRIEVE);

  server_db::Player* requestPlayer = request.mutable_player();

  requestPlayer->set_username(player.getPlayerInfo().username());
  requestPlayer->set_password(player.getPlayerInfo().password());
  requestPlayer->set_tieds(player.getPlayerInfo().tieds());
  requestPlayer->set_wins(player.getPlayerInfo().wins());
  requestPlayer->set_lost(player.getPlayerInfo().lost());
  requestPlayer->CopyFrom(player.getPlayerInfo());

  int sock = DatabaseCommunication::sendRequest(request);

  if (sock) {
    server_db::PlayerResponse response =
        DatabaseCommunication::receiveFromPlayerRequest(sock);

    server_client::Response* status = playerResponse.mutable_status();

    status->CopyFrom(response.status());

    auto db_players = response.player();

    server_client::Player* pl = playerResponse.mutable_player();
    pl->set_username(db_players.at(0).username());
    pl->set_password(db_players.at(0).password());
    pl->set_tieds(db_players.at(0).tieds());
    pl->set_wins(db_players.at(0).wins());
    pl->set_lost(db_players.at(0).lost());

    player.updateInfo(*pl);
  }

  return playerResponse.SerializeAsString();
}
