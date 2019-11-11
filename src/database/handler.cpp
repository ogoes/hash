#include "database/handler.h"

server_db::PlayerResponse PlayerHandler::Create (
    const server_db::Player player ) {

  server_db::PlayerResponse response;
  server_db::Response*      status = response.mutable_status ();

  std::unique_ptr< models::Player > player_model =
      std::make_unique< models::Player > ();

  std::string username = player.username ();

  if ( username.size () > 0 ) {
    player_model->set_username ( username );
  } else {
    status->set_status_code ( 400 );
    status->set_message ( "[username] não especificado." );
    // bad request
    //
    return response;
  }

  std::vector< models::db_data_t > query =
      Database::query ( player_model->query_builder ( "*" ).c_str () );

  if ( !query.empty () ) {

    status->set_status_code ( 409 );
    status->set_message ( "Player já registrado." );

    return response;
  }

  player_model->set_password ( player.password () );

  if ( !Database::exec ( player_model->insert_into ().c_str () ) ) {
    status->set_status_code ( 500 );
    status->set_message ( "Erro ao criar o [player] no banco de dados." );

    return response;
  }

  status->set_status_code ( 201 );
  status->set_message ( "Player registrado com sucesso." );

  return response;
}

server_db::PlayerResponse PlayerHandler::Retrieve (
    const server_db::Player player ) {

  server_db::PlayerResponse response;
  server_db::Response*      status = response.mutable_status ();

  std::unique_ptr< models::Player > player_model =
      std::make_unique< models::Player > ();

  std::string username = player.username ();

  if ( username.size () > 0 ) {
    player_model->set_username ( username );
  } else {
    status->set_status_code ( 400 );
    status->set_message ( "[username] não especificado." );
    // bad request
    //
    return response;
  }

  std::vector< models::db_data_t > query =
      Database::query ( player_model->query_builder ( "*" ).c_str () );

  if ( query.empty () ) {

    status->set_status_code ( 404 );
    status->set_message ( "Player não encontrado." );

    return response;
  }

  for ( auto it = query.begin (); it != query.end (); ++it ) {
    std::unique_ptr< models::Player > from_db (
        models::Player::from_db ( *it ) );

    server_db::Player* new_player = response.add_player ();
    new_player->set_id ( from_db->get_id () );
    new_player->set_username ( from_db->get_username () );
    new_player->set_wins ( from_db->get_wins () );
    new_player->set_tieds ( from_db->get_tieds () );
    new_player->set_lost ( from_db->get_lost () );
  }

  status->set_status_code ( 200 );
  status->set_message ( "Player registrado com sucesso." );

  return response;
}
server_db::PlayerResponse PlayerHandler::Update (
    const server_db::Player player ) {

  server_db::PlayerResponse response;
  return response;
}
server_db::PlayerResponse PlayerHandler::Delete (
    const server_db::Player player ) {

  server_db::PlayerResponse response;
  return response;
}

std::string PlayerHandler::Handle ( server_db::Request request ) {

  server_db::PlayerResponse response;

  switch ( request.operation () ) {

    case server_db::Request::CREATE:
      response = PlayerHandler::Create ( request.player () );
      break;

    case server_db::Request::RETRIEVE:
      response = PlayerHandler::Retrieve ( request.player () );
      break;

    case server_db::Request::UPDATE:
      response = PlayerHandler::Update ( request.player () );
      break;

    case server_db::Request::DELETE:
      response = PlayerHandler::Delete ( request.player () );
      break;

    default:
      break;
  }

  std::string response_as_string;
  response.SerializeToString ( &response_as_string );

  return response_as_string;
}

server_db::GameResponse GameHandler::Create ( const server_db::Game game ) {}
server_db::GameResponse GameHandler::Retrieve ( const server_db::Game game ) {}
server_db::GameResponse GameHandler::Update ( const server_db::Game game ) {}
server_db::GameResponse GameHandler::Delete ( const server_db::Game game ) {}

std::string GameHandler::Handle ( server_db::Request request ) {

  server_db::GameResponse response;

  switch ( request.operation () ) {

    case server_db::Request::CREATE:
      response = GameHandler::Create ( request.game () );
      break;

    case server_db::Request::RETRIEVE:
      response = GameHandler::Retrieve ( request.game () );
      break;

    case server_db::Request::UPDATE:
      response = GameHandler::Update ( request.game () );
      break;

    case server_db::Request::DELETE:
      response = GameHandler::Delete ( request.game () );
      break;

    default:
      break;
  }

  std::string response_as_string;
  response.SerializeToString ( &response_as_string );

  return response_as_string;
}
