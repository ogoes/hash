#include "database/models.h"

using namespace models;

Player::Player() {}
Player::~Player() {}

Player* Player::set_id(uint id) {
  this->id = id;
  this->has_id = true;

  return this;
}

Player* Player::set_username(std::string username) {
  this->username = username;
  this->has_username = true;

  return this;
}

Player* Player::set_password(std::string password) {
  this->password = password;
  this->has_password = true;

  return this;
}

Player* Player::set_wins(uint wins) {
  this->wins = wins;
  this->has_wins = true;

  return this;
}

Player* Player::set_tieds(uint tieds) {
  this->tieds = tieds;
  this->has_tieds = true;

  return this;
}

Player* Player::set_lost(uint lost) {
  this->lost = lost;
  this->has_lost = true;

  return this;
}

uint Player::get_id() { return this->has_id ? this->id : 0; }
std::string Player::get_username() {
  return this->has_username ? this->username : std::string("");
}
uint Player::get_wins() { return this->has_wins ? this->wins : 0; }
uint Player::get_tieds() { return this->has_tieds ? this->tieds : 0; }
uint Player::get_lost() { return this->has_lost ? this->lost : 0; }

std::string Player::insert_into() {
  std::string query(
      "INSERT INTO Player (username, password, wins, tieds, lost) VALUES "
      "(");
  std::string attr;

  attr = this->has_username
             ? std::string("\'") + this->username + std::string("\'")
             : std::string("NULL");
  query += attr;

  query += std::string(",");
  attr = this->has_password
             ? std::string("\'") + this->password + std::string("\'")
             : std::string("NULL");
  query += attr;

  query += std::string(",");
  attr = this->has_wins ? std::to_string(this->wins) : std::string("0");
  query += attr;

  query += std::string(",");
  attr = this->has_tieds ? std::to_string(this->tieds) : std::string("0");
  query += attr;

  query += std::string(",");
  attr = this->has_lost ? std::to_string(this->lost) : std::string("0");
  query += attr;

  query += std::string(");");

  return query;
}

std::string Player::query_builder(const char* columns = "*") {
  std::string query("SELECT ");

  query += std::string(columns);

  query += std::string(" FROM Player");

  if (this->has_id || this->has_username) {
    query += " WHERE ";

    std::string where;

    where = this->has_username ? std::string("username = \'") + this->username +
                                     std::string("\'")
                               : std::string("");
    query += where;
  }

  query += std::string(";");

  return query;
}

std::string Player::update() {
  std::string query("UPDATE Player SET ");

  bool has_prev_attr = false;

  std::string set;
  std::string where;

  if (this->has_username) {
    set = std::string("username = \'") + this->username + std::string("\'");

    where = this->has_id ? std::string(" WHERE id = ") +
                               std::to_string(this->id) + std::string(" AND ")
                         : std::string(" WHERE ");

    where += set;

    has_prev_attr = true;
  }

  if (where.size() == 0) {
    std::cerr << "Operação necessita de uma parâmetro WHERE" << std::endl;
    return std::string("");
  }

  query += set;

  if (this->has_password && this->password.size() > 0) {
    set = has_prev_attr ? std::string(", ") : std::string("");

    set += std::string("password = \'") + this->password + std::string("\'");
    has_prev_attr = true;
  }
  query += set;

  if (this->has_wins) {
    set = has_prev_attr ? std::string(", ") : std::string("");

    set += std::string("wins = ") + std::to_string(this->wins);
    has_prev_attr = true;
  }
  query += set;

  if (this->has_tieds) {
    set = has_prev_attr ? std::string(", ") : std::string("");

    set += std::string("tieds = ") + std::to_string(this->tieds);
    has_prev_attr = true;
  }
  query += set;

  if (this->has_lost) {
    set = has_prev_attr ? std::string(", ") : std::string("");

    set += std::string("lost = ") + std::to_string(this->lost);
  }
  query += set;

  return query + where + std::string(";");
}

std::string Player::delete_from() {
  std::string query("DELETE FROM Player WHERE ");

  std::string where;

  if (!this->has_username) {
    std::cerr << "Operação necessita de uma parâmetro WHERE" << std::endl;
    return std::string("Operação necessita de uma parâmetro WHERE");
  }

  if (this->has_username) {
    where += std::string("username = \'") + this->username + std::string("\'");
  }
  query += where;

  return query + std::string(";");
}

bool Player::check_password(std::string pass) {
  if (!this->has_password) return false;

  return this->password == pass;
}

Player* Player::from_db(db_data_t data) {
  Player* player = new Player();

  return player->set_id(std::atoi(data.at(0).c_str()))
      ->set_username(data.at(1))
      ->set_password(data.at(2))
      ->set_wins(std::atoi(data.at(3).c_str()))
      ->set_tieds(std::atoi(data.at(4).c_str()))
      ->set_lost(std::atoi(data.at(5).c_str()));
}
