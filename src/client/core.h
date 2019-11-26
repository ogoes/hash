#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "client/player.h"
#include "communication/in_game.pb.h"

class Core {
 public:
  Core();
  ~Core();

  void createPlayer(std::string, std::string);
  bool connectToServer(std::string, long int);

  void play();

  std::vector<server_client::Player> ranking();
  std::vector<server_client::Player> online();
  server_client::Player view();

  in_game::State hit();
  in_game::Result result();

 private:
  std::shared_ptr<Player> player = nullptr;
};
