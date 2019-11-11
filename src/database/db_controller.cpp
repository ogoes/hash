#include <cstring>
#include <iostream>
#include <string>
#include <vector>

// socket includes
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

// protocol buffers
#include "communication/server_database.pb.h"

// handlers
#include "database/handler.h"

#define SockAddress struct sockaddr
#define SockIn struct sockaddr_in

#define RCV_BUFFER_SIZE ( 1024 * 64 )

typedef struct {
  int    sockFd;
  SockIn in;
  int    inLen;
} SockAttr;

/**
 * [conenctionHandler  description] procedimento que irá redirecionar a
 * requisição recebida via socket
 * @param connection [description] Atributos do socket conectado
 */
void conenctionHandler ( SockAttr connection ) {
  char receiverBuffer[ RCV_BUFFER_SIZE ] = {0};

  server_db::Request request;

  if ( read ( connection.sockFd, receiverBuffer, RCV_BUFFER_SIZE ) ) {
    if ( !request.ParseFromString ( std::string ( receiverBuffer ) ) ) {
      std::string message = "Erro ao receber pacote PB";

      std::cerr << message << std::endl;
      return;
    }

    std::string response;
    switch ( request.entity () ) {
      case server_db::Request::PLAYER:
        response = PlayerHandler::Handle ( request );
        break;
      case server_db::Request::GAME:
        response = GameHandler::Handle ( request );
        break;
      default:
        return;
    }

    const char* sender_buffer      = response.c_str ();
    long int    sender_buffer_size = strlen ( sender_buffer );

    send ( connection.sockFd, sender_buffer, sender_buffer_size, 0 );
  }

  close ( connection.sockFd );
}

int main ( int argc, char* argv[] ) {
  if ( argc != 2 ) {
    std::cerr << "Modo de usar: ./db_daemon PORT" << std::endl;
    exit ( 1 );
  }

  int PORT = std::atoi ( argv[ 1 ] );

  SockAttr controller;  // informações sobre o controller
  controller.inLen = sizeof ( controller.in );

  SockAttr client;  // informações sobre o cliente

  controller.sockFd =
      socket ( AF_INET, SOCK_STREAM, 0 );  // cria socket do controller
  if ( controller.sockFd == -1 ) {
    std::cerr << "Erro ao criar o socket do controlador" << std::endl;
    exit ( 1 );
  }
  bzero ( &( controller.in ),
          controller.inLen );  // limpa o endereço do controlador

  controller.in.sin_family      = AF_INET;  // define como familia do socket
  controller.in.sin_addr.s_addr = htonl ( INADDR_ANY );  // define o endereço
  controller.in.sin_port        = htons ( PORT );        // define a porta

  // executa o bind da conexao do socket
  if ( ( bind ( controller.sockFd, ( SockAddress* )&( controller.in ),
                controller.inLen ) ) ) {
    close ( controller.sockFd );
    std::cerr << "Erro ao executar o bind" << std::endl;
    exit ( 1 );
  }

  if ( ( listen ( controller.sockFd, 5 ) ) ) {
    std::cerr << "Erro no listen" << std::endl;
    exit ( 1 );
  }

  std::cout << "controller running at " << PORT << std::endl;

  client.inLen = sizeof ( client.in );

  Database::open ( "data.db" );
  Database::exec ( models::PLAYER_CREATION.c_str () );
  Database::exec ( models::GAME_CREATION.c_str () );

  while ( true ) {
    client.sockFd = accept ( controller.sockFd, ( SockAddress* )&( client.in ),
                             ( unsigned int* )&client.inLen );
    if ( client.sockFd < 0 )
      std::cout << "Erro ao aceitar nova conexão" << std::endl;

    std::thread ( conenctionHandler, client ).detach ();
  }

  return 0;
}
