#include <algorithm>
#include <cstring>
#include <iostream>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include "communication/in_game.pb.h"
#include "server/player.h"

namespace inGame {

class Game {
 private:
  void start();

  std::pair<bool, in_game::Position::ValueType> finished();
  in_game::State hit(Player *);
  in_game::Result::Tieds winner();

 public:
  Game(Player &, Player &);
  ~Game();

  in_game::Result result();

 private:
  Player *player1;
  Player *player2;

  in_game::Result resultPB;
  in_game::State state;
};

std::string play(Player &, Player &);
std::string challenge(Player &, Player &);

}  // namespace inGame
