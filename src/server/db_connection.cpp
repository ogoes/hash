#include "server/db_connection.h"

using namespace DatabaseCommunication;

using server_db::GameResponse;
using server_db::PlayerResponse;
using server_db::Request;
using server_db::Response;

PlayerResponse DatabaseCommunication::receiveFromPlayerRequest(int socketfd) {
  PlayerResponse response;

  long int readed;

  char buffer[RCV_BUFFER_SIZE];
  if (read(socketfd, buffer, RCV_BUFFER_SIZE)) {
    response.ParseFromString(std::string(buffer));
  }

  return response;
}

GameResponse DatabaseCommunication::receiveFromGameRequest(int socketfd) {
  GameResponse response;

  long int readed;

  char buffer[RCV_BUFFER_SIZE];
  if (read(socketfd, buffer, RCV_BUFFER_SIZE)) {
    response.ParseFromString(std::string(buffer));
  }

  return response;
}

int DatabaseCommunication::sendRequest(Request request) {
  if (!DATABASE_ADDR) {
    logging("Endereço padrão do banco de dados: 127.0.0.1");
    DATABASE_ADDR = (char *)"127.0.0.1";
  }

  if (!DATABASE_PORT) {
    logging("Porta padrão do banco de dados: 9134");
    DATABASE_PORT = 9134;
  }

  int DATABASE_SOCKET_FD = 0;
  if ((DATABASE_SOCKET_FD = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    logging("Erro ao criar o socket");
    return 0;
  }

  SocketInfo dbAddr;

  dbAddr.sin_family = AF_INET;
  dbAddr.sin_port = htons(DATABASE_PORT);

  if (inet_pton(AF_INET, DATABASE_ADDR, &dbAddr.sin_addr) <= 0) {
    logging("Endereço inválido");
    return 0;
  }

  if (connect(DATABASE_SOCKET_FD, (struct sockaddr *)&dbAddr, sizeof(dbAddr)) <
      0) {
    logging("Falha ao criar conexão");
    return 0;
  }
  logging("Conexão estabelecida");

  std::string requestAsString = request.SerializeAsString();
  logging("Serialização executada");

  const char *buffer = requestAsString.c_str();
  long int size = strlen(buffer);

  send(DATABASE_SOCKET_FD, buffer, size, 0);
  logging("Requisição enviada");

  return DATABASE_SOCKET_FD;
}
