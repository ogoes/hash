#include "database/models.h"

using namespace models;

Game::Game() {}
Game::~Game() {
  delete this->player1;
  delete this->player2;
}

Game* Game::from_db(db_data_t data) {
  Game* game = new Game();

  int f_player_id = std::atoi(data.at(1).c_str());
  Player* f_player = (new Player())->set_id(f_player_id);
  std::vector<db_data_t> db_f_player =
      Database::query(f_player->query_builder("*").c_str());
  delete f_player;

  int s_player_id = std::atoi(data.at(2).c_str());
  Player* s_player = (new Player())->set_id(s_player_id);
  std::vector<db_data_t> db_s_player =
      Database::query(s_player->query_builder("*").c_str());
  delete s_player;

  return game->set_id(std::atoi(data.at(0).c_str()))
      ->set_first_player(Player::from_db(db_f_player.at(0)))
      ->set_second_player(Player::from_db(db_s_player.at(0)))
      ->set_result((Result)std::atoi(data.at(3).c_str()))
      ->set_winner((Winner)std::atoi(data.at(4).c_str()));
}

uint Game::get_id() { return this->has_id ? this->id : 0; }
Player* Game::get_first_player() {
  return this->has_player1 ? this->player1 : nullptr;
}
Player* Game::get_second_player() {
  return this->has_player2 ? this->player2 : nullptr;
}
Result Game::get_result() {
  return this->has_result ? (Result)((int)this->result + 1) : (Result)0;
}
Player* Game::get_winner() {
  if (this->has_result && this->result == HAS_WINNER) {
    return this->winner == PLAYER1 ? this->get_first_player()
                                   : this->get_second_player();
  }
  return nullptr;
}

Game* Game::set_id(uint id) {
  if (id) {
    this->has_id = true;

    this->id = id;
  }

  return this;
}
Game* Game::set_first_player(Player* player) {
  if (player) {
    this->has_player1 = true;

    this->player1 = player;
  }
  return this;
}
Game* Game::set_second_player(Player* player) {
  if (player) {
    this->has_player2 = true;

    this->player2 = player;
  }
  return this;
}
Game* Game::set_result(Result result) {
  if (result) {
    this->has_result = true;

    this->result = result;
  }
  return this;
}
Game* Game::set_winner(Winner winner) {
  if (winner) {
    this->has_winner = true;

    this->winner = winner;
  }
  return this;
}

std::string Game::insert_into() {
  std::string query(
      "INSERT INTO Game (id, player1, player2, result, winner) VALUES "
      "(NULL");
  std::string attr;

  query += std::string(",");
  attr = this->has_player1 ? std::to_string(this->player1->get_id())
                           : std::string("NULL");
  query += attr;

  query += std::string(",");
  attr = this->has_player2 ? std::to_string(this->player2->get_id())
                           : std::string("NULL");
  query += attr;

  query += std::string(",");
  attr = this->has_result ? std::to_string(this->result) : std::string("0");
  query += attr;

  query += std::string(",");
  attr = this->has_winner ? std::to_string(this->winner) : std::string("0");
  query += attr;

  query += std::string(");");

  return query;
}

std::string Game::query_builder(const char* select) {
  std::string query("SELECT ");

  query += std::string(select);

  query += std::string(" FROM Game");

  if (this->has_id) {
    query += " WHERE ";

    std::string where;

    where = this->has_id ? std::string("id = ") + std::to_string(this->id)
                         : std::string("");
    query += where;
  }

  query += std::string(";");

  return query;
}

std::string Game::update() {
  std::string query("UPDATE Player SET ");

  bool has_prev_attr = false;

  std::string set;
  std::string where;

  if (this->has_id) {
    where = std::string(" WHERE id = ") + std::to_string(this->id);
  }

  if (where.size() == 0) {
    std::cerr << "Operação necessita de uma parâmetro WHERE" << std::endl;
    return NULL;
  }

  query += set;

  if (this->has_player1) {
    set = has_prev_attr ? std::string(", ") : std::string("");

    set += std::string("player1 = ") + std::to_string(this->player1->get_id());
    has_prev_attr = true;
  }
  query += set;

  if (this->has_player2) {
    set = has_prev_attr ? std::string(", ") : std::string("");

    set += std::string("player2 = ") + std::to_string(this->player2->get_id());
    has_prev_attr = true;
  }
  query += set;

  if (this->has_result) {
    set = has_prev_attr ? std::string(", ") : std::string("");

    set += std::string("result = ") + std::to_string(this->get_result());
    has_prev_attr = true;
  }
  query += set;

  if (this->has_winner) {
    set = has_prev_attr ? std::string(", ") : std::string("");

    Player* winner = this->get_winner();

    set += std::string("winner = ") + std::to_string(winner->get_id());
    has_prev_attr = true;
  }
  query += set;

  return query + where + std::string(";");
}

std::string Game::delete_from() {
  std::string query("DELETE FROM Game WHERE ");

  if (!this->has_id) {
    std::cerr << "Operação necessita de uma parâmetro WHERE" << std::endl;
    return std::string("");
  }

  std::string where = std::string("id = ") + std::to_string(this->id);

  query += where;

  return query + std::string(";");
}
