#include <cstring>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <algorithm>
#include <memory>

#include "communication/client_server.pb.h"

#include "server/game.h"

namespace Handler {

static std::vector<Player> players;

void Handle(std::string, SockAttr);

std::string ranking();
std::string play(Player&);
std::string online();
std::string view(Player&);
std::string change(Player&);

std::string logIn(std::string, std::string);
std::string signUp(std::string, std::string);

}  // namespace Handler
