#include "database/handler.h"

server_db::PlayerResponse PlayerHandler::Create(
    const server_db::Player player) {
  logging("Recebendo requisição de criação de Player");

  server_db::PlayerResponse response;
  server_db::Response* status = response.mutable_status();

  models::Player* player_model = new models::Player();

  std::string username = player.username();
  logging("Criando Player: " << username);

  if (username.size() > 0) {
    player_model->set_username(username);
    logging("Tudo certo com o nome");
  } else {
    status->set_status_code(400);
    status->set_message("[username] não especificado.");
    // bad request
    //
    logging("Nome não foi enviado");
    return response;
  }

  logging("Excutando query em busca do player com este username");

  std::vector<models::db_data_t> query =
      Database::query(player_model->query_builder("*").c_str());

  if (!query.empty()) {
    logging("Player já existe");
    status->set_status_code(409);
    status->set_message("Player já registrado.");

    return response;
  }

  logging("Player não encontrado, continuando");

  player_model->set_password(player.password());

  logging("Executando comando de criação do PLayer");
  if (!Database::exec(player_model->insert_into().c_str())) {
    status->set_status_code(500);
    status->set_message("Erro ao criar o [player] no banco de dados.");
    logging("Erro ao criar o player");
    return response;
  }

  logging("Player criado com sucesso");

  delete player_model;

  status->set_status_code(201);
  status->set_message("Player registrado com sucesso.");

  logging("Retornando da criação");
  return response;
}

server_db::PlayerResponse PlayerHandler::Retrieve(
    const server_db::Player player) {
  logging("Recebendo requisição de recuperação de player");

  server_db::PlayerResponse response;
  server_db::Response* status = response.mutable_status();

  models::Player* player_model = new models::Player();

  std::string username = player.username();
  logging("username: " << username);

  if (username.size() > 0) {
    player_model->set_username(username);
  } else {
    status->set_status_code(400);
    status->set_message("[username] não especificado.");
    // bad request
    //
    logging("Não foi passado o username");
    return response;
  }

  uint id = player.id();
  if (id > 0) {
    player_model->set_id(id);
  }

  logging("Tudo certo com o username");

  logging("Executando query...");

  std::vector<models::db_data_t> query =
      Database::query(player_model->query_builder("*").c_str());

  if (query.empty()) {
    status->set_status_code(404);
    status->set_message("Player não encontrado.");
    logging("Player não encontrado");
    return response;
  }

  logging("Players encontrados: " << query.size());

  for (auto it = query.begin(); it != query.end(); ++it) {
    std::unique_ptr<models::Player> from_db(models::Player::from_db(*it));

    server_db::Player* new_player = response.add_player();
    new_player->set_id(from_db->get_id());
    new_player->set_username(from_db->get_username());
    new_player->set_wins(from_db->get_wins());
    new_player->set_tieds(from_db->get_tieds());
    new_player->set_lost(from_db->get_lost());
  }

  delete player_model;

  logging("Retornando");

  status->set_status_code(200);
  status->set_message("Player registrado com sucesso.");

  return response;
}

server_db::PlayerResponse PlayerHandler::Update(
    const server_db::Player player) {
  logging("Update player");
  server_db::PlayerResponse response;
  server_db::Response* status = response.mutable_status();

  models::Player* player_model = new models::Player();

  player_model->set_id(player.id())
      ->set_username(player.username())
      ->set_password(player.password())
      ->set_wins(player.wins())
      ->set_tieds(player.tieds())
      ->set_lost(player.lost());

  status->set_status_code(200);
  status->set_message("Player atualizado com sucesso.");

  logging("Executando no Banco de dados");
  if (!Database::exec(player_model->update().c_str())) {
    status->set_status_code(500);
    status->set_message("Não foi possível atualizar os dados do Player.");
    logging("Erro ao atualizar o player");
  }

  delete player_model;

  return response;
}

server_db::PlayerResponse PlayerHandler::Delete(
    const server_db::Player player) {
  logging("Recebendo requisição de exclusão");
  server_db::PlayerResponse response;
  server_db::Response* status = response.mutable_status();

  models::Player* player_model = new models::Player();

  player_model->set_id(player.id())->set_username(player.username());

  status->set_status_code(200);
  status->set_message("Player removido com sucesso.");

  logging("Executando requisição no Banco de Dados");

  if (!Database::exec(player_model->delete_from().c_str())) {
    status->set_status_code(500);
    status->set_message("Não foi possível remover o Player.");
    logging("Error ao excluir o player");
  }

  delete player_model;
  logging("Retornando");
  return response;
}

std::string PlayerHandler::Handle(server_db::Request request) {
  server_db::PlayerResponse response;

  switch (request.operation()) {
    case server_db::Request::CREATE:
      response = PlayerHandler::Create(request.player());
      break;

    case server_db::Request::RETRIEVE:
      response = PlayerHandler::Retrieve(request.player());
      break;

    case server_db::Request::UPDATE:
      response = PlayerHandler::Update(request.player());
      break;

    case server_db::Request::DELETE:
      response = PlayerHandler::Delete(request.player());
      break;

    default:
      break;
  }

  std::string response_as_string;
  response.SerializeToString(&response_as_string);

  return response_as_string;
}

server_db::GameResponse GameHandler::Create(const server_db::Game game) {}
server_db::GameResponse GameHandler::Retrieve(const server_db::Game game) {
  server_db::GameResponse response;
  server_db::Response* status = response.mutable_status();

  models::Game* game_model = new models::Game();

  if (game.id() > 0) game_model->set_id(game.id());

  std::vector<models::db_data_t> query =
      Database::query(game_model->query_builder("*").c_str());

  if (query.empty()) {
    status->set_status_code(404);
    status->set_message("Game não encontrado.");

    delete game_model;

    return response;
  }

  for (auto it = query.begin(); it != query.end(); ++it) {
    models::Game* from_db = models::Game::from_db(*it);

    server_db::Game* new_game = response.add_game();
    new_game->set_id(from_db->get_id());

    server_db::Player* player1 = new_game->mutable_player1();
    models::Player* model_player1 = from_db->get_first_player();

    player1->set_id(model_player1->get_id());
    player1->set_username(model_player1->get_username());
    player1->set_wins(model_player1->get_wins());
    player1->set_tieds(model_player1->get_tieds());
    player1->set_lost(model_player1->get_lost());

    server_db::Player* player2 = new_game->mutable_player2();
    models::Player* model_player2 = from_db->get_second_player();

    player2->set_id(model_player2->get_id());
    player2->set_username(model_player2->get_username());
    player2->set_wins(model_player2->get_wins());
    player2->set_tieds(model_player2->get_tieds());
    player2->set_lost(model_player2->get_lost());

    new_game->set_result((server_db::Game::Result)from_db->get_result());

    models::Player* winner = from_db->get_winner();

    if (winner) {
      if (winner->get_id() == model_player1->get_id())
        new_game->set_winner(server_db::Game::PLAYER1);
      else
        new_game->set_winner(server_db::Game::PLAYER2);
    }

    delete from_db;
  }

  delete game_model;

  status->set_status_code(200);
  status->set_message("Player registrado com sucesso.");

  return response;
}
server_db::GameResponse GameHandler::Update(const server_db::Game game) {
  server_db::GameResponse response;
  server_db::Response* status = response.mutable_status();

  models::Game* game_model = new models::Game();

  if (game.has_player1()) {
    models::Player* first_player_model = new models::Player();

    server_db::Player player = game.player1();

    first_player_model->set_id(player.id())
        ->set_username(player.username())
        ->set_password(player.password())
        ->set_wins(player.wins())
        ->set_tieds(player.tieds())
        ->set_lost(player.lost());

    game_model->set_first_player(first_player_model);
  }

  if (game.has_player2()) {
    models::Player* second_player_model = new models::Player();

    server_db::Player player = game.player2();

    second_player_model->set_id(player.id())
        ->set_username(player.username())
        ->set_password(player.password())
        ->set_wins(player.wins())
        ->set_tieds(player.tieds())
        ->set_lost(player.lost());

    game_model->set_second_player(second_player_model);
  }

  game_model->set_id(game.id())
      ->set_result((models::Result)((int)game.result() + 1))
      ->set_winner((models::Winner)((int)game.winner() + 1));

  status->set_status_code(200);
  status->set_message("Jogo atualizado com sucesso.");

  if (!Database::exec(game_model->update().c_str())) {
    status->set_status_code(500);
    status->set_message("Não foi possível atualizar os dados do Jogo.");
  }

  delete game_model;

  return response;
}
server_db::GameResponse GameHandler::Delete(const server_db::Game game) {
  server_db::GameResponse response;
  server_db::Response* status = response.mutable_status();

  models::Game* game_model = new models::Game();

  game_model->set_id(game.id());

  status->set_status_code(200);
  status->set_message("Jogo removido com sucesso.");

  if (!Database::exec(game_model->delete_from().c_str())) {
    status->set_status_code(500);
    status->set_message("Não foi possível remover o Jogo.");
  }

  delete game_model;

  return response;
}

std::string GameHandler::Handle(server_db::Request request) {
  server_db::GameResponse response;

  switch (request.operation()) {
    case server_db::Request::CREATE:
      response = GameHandler::Create(request.game());
      break;

    case server_db::Request::RETRIEVE:
      response = GameHandler::Retrieve(request.game());
      break;

    case server_db::Request::UPDATE:
      response = GameHandler::Update(request.game());
      break;

    case server_db::Request::DELETE:
      response = GameHandler::Delete(request.game());
      break;

    default:
      break;
  }

  std::string response_as_string;
  response.SerializeToString(&response_as_string);

  return response_as_string;
}
