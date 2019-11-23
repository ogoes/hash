
#include <cstring>
#include <iostream>
#include <memory>
#include <string>
#include <thread>
#include <vector>

// protocol buffers
#include "communication/server_database.pb.h"

// models
#include "database/models.h"

namespace PlayerHandler {

server_db::PlayerResponse Create(const server_db::Player);
server_db::PlayerResponse Retrieve(const server_db::Player);
server_db::PlayerResponse Update(const server_db::Player);
server_db::PlayerResponse Delete(const server_db::Player);
std::string Handle(server_db::Request);

};  // namespace PlayerHandler

namespace GameHandler {

server_db::GameResponse Create(server_db::Game);
server_db::GameResponse Retrieve(server_db::Game);
server_db::GameResponse Update(server_db::Game);
server_db::GameResponse Delete(server_db::Game);
std::string Handle(server_db::Request);

};  // namespace GameHandler
