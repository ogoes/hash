#include "server/game.h"

using namespace inGame;

Game::Game(Player& player1, Player& player2) {
  this->player1 = &player1;
  this->player2 = &player2;

  resultPB.set_result(in_game::Result::TIEDS);

  for (int i = 0; i < 9; ++i) {
    in_game::Position* element = state.add_elements();
    element->set_column(i % 3);
    element->set_column(i / 3);

    element->set_value(in_game::Position::NONE);
  }

  this->start();
}

Game::~Game() {}

void Game::start() {
  state.set_turn(false);
  player1->sendMessage((char*)state.SerializeAsString().c_str());
  player1->receiveMessage();  // ACK

  state.set_turn(true);
  state.set_finished(false);

  int col, line;

  while (this->finished().first) {
    state.CopyFrom(hit(player2));

    if (this->finished().first) break;

    state.CopyFrom(hit(player1));
  }

  state.set_finished(true);

  player1->sendMessage((char*)state.SerializeAsString().c_str());
  player1->receiveMessage();  // ACK
  player2->sendMessage((char*)state.SerializeAsString().c_str());
  player2->receiveMessage();  // ACK

  winner();
}

in_game::State Game::hit(Player* player) {
  player->sendMessage((char*)state.SerializeAsString().c_str());
  state.Clear();
  in_game::Hit hit;
  hit.ParseFromString(player->receiveMessage());  // Jogada

  int col = hit.position().column();
  int line = hit.position().line();

  in_game::State newState;
  for (int i = 0; i < 9; ++i) {
    in_game::Position* adde = newState.add_elements();
    adde->CopyFrom(state.elements()[i]);

    if (i == ((line * 3) + col)) {
      adde->set_value(player->symbol());
    }
  }

  state.set_turn(true);
  return newState;
}

std::pair<bool, in_game::Position::ValueType> Game::finished() {
  auto position = state.elements();
  std::vector<std::pair<bool, in_game::Position::ValueType>> results;

  bool firstDiagonal = position[0].value() != in_game::Position::NONE &&
                       position[0].value() == position[4].value() &&
                       position[4].value() == position[8].value();

  std::pair<bool, in_game::Position::ValueType> fd(firstDiagonal,
                                                   position[0].value());

  results.push_back(fd);

  bool secondDiagonal = position[2].value() == position[4].value() &&
                        position[4].value() == position[6].value();
  std::pair<bool, in_game::Position::ValueType> sd(secondDiagonal,
                                                   position[2].value());
  results.push_back(sd);

  bool firstLine = position[0].value() != in_game::Position::NONE &&
                   position[0].value() == position[1].value() &&
                   position[1].value() == position[2].value();
  std::pair<bool, in_game::Position::ValueType> fl(firstLine,
                                                   position[0].value());
  results.push_back(fl);

  bool secondLine = position[3].value() != in_game::Position::NONE &&
                    position[3].value() == position[4].value() &&
                    position[4].value() == position[5].value();
  std::pair<bool, in_game::Position::ValueType> sl(secondLine,
                                                   position[3].value());
  results.push_back(sl);

  bool thirdLine = position[6].value() != in_game::Position::NONE &&
                   position[6].value() == position[7].value() &&
                   position[7].value() == position[8].value();
  std::pair<bool, in_game::Position::ValueType> tl(thirdLine,
                                                   position[6].value());
  results.push_back(tl);

  bool firstColumn = position[0].value() != in_game::Position::NONE &&
                     position[0].value() == position[3].value() &&
                     position[3].value() == position[6].value();
  std::pair<bool, in_game::Position::ValueType> fc(firstColumn,
                                                   position[0].value());
  results.push_back(fc);

  bool secondColumn = position[1].value() != in_game::Position::NONE &&
                      position[1].value() == position[4].value() &&
                      position[4].value() == position[7].value();
  std::pair<bool, in_game::Position::ValueType> sc(secondColumn,
                                                   position[1].value());
  results.push_back(sc);

  bool thirdColumn = position[2].value() != in_game::Position::NONE &&
                     position[2].value() == position[5].value() &&
                     position[5].value() == position[8].value();
  std::pair<bool, in_game::Position::ValueType> tc(thirdColumn,
                                                   position[2].value());
  results.push_back(tc);

  for (auto it = results.begin(); it != results.end(); ++it) {
    if ((*it).first) return (*it);  // existe um ganhador
  }

  // não existe ganhador (empatou ou não terminou)
  std::pair<bool, in_game::Position::ValueType> ended(true,
                                                      in_game::Position::NONE);
  for (auto it = results.begin(); it != results.end(); ++it) {
    bool none = (*it).second == in_game::Position::NONE;

    ended.first = none ? false : ended.first;
  }

  return ended;
}

in_game::Result::Tieds Game::winner() {
  std::pair<bool, in_game::Position::ValueType> res;

  res = finished();

  in_game::Result::Tieds tied =
      res.first ? in_game::Result::TIEDS : in_game::Result::WINNER;

  in_game::Result::Winner player = res.second == player1->symbol()
                                       ? in_game::Result::PLAYER1
                                       : in_game::Result::PLAYER2;

  std::string username = player == in_game::Result::PLAYER1
                             ? player1->getPlayerInfo().username()
                             : player2->getPlayerInfo().username();

  this->resultPB.set_result(tied);
  this->resultPB.set_winner(player);
  this->resultPB.set_username(username);

  return tied;
}
in_game::Result Game::result() { return resultPB; }

std::string inGame::play(Player& player1, Player& player2) {
  player1.waiting(false);
  player1.inGame(true);

  player2.waiting(false);
  player2.inGame(true);

  std::unique_ptr<Game> game(new Game(player1, player2));

  in_game::Result resultado = game->result();

  server_db::Request updateRequest;
  updateRequest.set_entity(server_db::Request::PLAYER);
  updateRequest.set_operation(server_db::Request::UPDATE);
  server_db::Player* player = updateRequest.mutable_player();

  if (resultado.result() == in_game::Result::TIEDS) {
    player->CopyFrom(player1.getPlayerInfo());
    player->set_tieds(player->tieds() + 1);

    int sock = DatabaseCommunication::sendRequest(updateRequest);
    DatabaseCommunication::receiveFromPlayerRequest(sock);

    player->Clear();
    player->CopyFrom(player2.getPlayerInfo());
    player->set_tieds(player->tieds() + 1);

    sock = DatabaseCommunication::sendRequest(updateRequest);
    DatabaseCommunication::receiveFromPlayerRequest(sock);
  } else if (resultado.winner() == in_game::Result::PLAYER1) {
    player->CopyFrom(player1.getPlayerInfo());
    player->set_wins(player->wins() + 1);

    int sock = DatabaseCommunication::sendRequest(updateRequest);
    DatabaseCommunication::receiveFromPlayerRequest(sock);

    player->Clear();
    player->CopyFrom(player2.getPlayerInfo());
    player->set_lost(player->lost() + 1);

    sock = DatabaseCommunication::sendRequest(updateRequest);
    DatabaseCommunication::receiveFromPlayerRequest(sock);

  } else if (resultado.winner() == in_game::Result::PLAYER2) {
    player->CopyFrom(player2.getPlayerInfo());
    player->set_wins(player->wins() + 1);

    int sock = DatabaseCommunication::sendRequest(updateRequest);
    DatabaseCommunication::receiveFromPlayerRequest(sock);

    player->Clear();
    player->CopyFrom(player1.getPlayerInfo());
    player->set_lost(player->lost() + 1);

    sock = DatabaseCommunication::sendRequest(updateRequest);
    DatabaseCommunication::receiveFromPlayerRequest(sock);
  }

  player2.sendMessage((char*)resultado.SerializeAsString().c_str());

  player1.inGame(false);
  player2.inGame(false);

  return resultado.SerializeAsString();
}
std::string inGame::challenge(Player& player1, Player& player2) {
  server_client::Keep keeping;

  keeping.set_type(server_client::Keep::CHALLENGE);

  player2.sendBackupMessage((char*)keeping.SerializeAsString().c_str());

  keeping.ParseFromString(player2.receiveBackupMessage());

  server_client::ChallengeResponse response;

  if (keeping.type() == server_client::Keep::ACK) {
    server_client::ChallengeRequest challengeRequest;

    server_client::Player* challenger =
        challengeRequest.mutable_challender_player();
    challenger->CopyFrom(player1.getPlayerInfo());

    player2.sendBackupMessage(
        (char*)challengeRequest.SerializeAsString().c_str());

    response.ParseFromString(player2.receiveBackupMessage());

    if (response.answer() == server_client::ChallengeResponse::ACCEPT) {
      player1.sendMessage((char*)response.SerializeAsString().c_str());
      player1.receiveMessage();

      return inGame::play(player1, player2);
    }

  } else {
    response.set_answer(server_client::ChallengeResponse::DENY);
  }

  return response.SerializeAsString();
}
