#include <cstring>
#include <iostream>
#include <string>
#include <thread>
#include <vector>

// protocol buffers
#include "communication/client_server.pb.h"
#include "communication/server_database.pb.h"
#include "server/handler.h"

/**
 * [SignIn  description] procedimento que irá redirecionar a
 * requisição recebida via socket
 * @param connection [description] Atributos do socket conectado
 */
void SignIn(SockAttr connection) {
  char receiverBuffer[RCV_BUFFER_SIZE] = {0};

  server_client::LogInSignUp request;
  logging("Esperando pacote do cliente");

  std::string response;
  if (read(connection.sockFd, receiverBuffer, RCV_BUFFER_SIZE)) {
    if (!request.ParseFromString(std::string(receiverBuffer))) {
      logging("Erro ao receber pacote PB");
      return;
    }

    logging("Resolvendo requisição");

    switch (request.request_type()) {
      case server_client::LogInSignUp::SIGNUP:
        logging("SignUp");
        response = Handler::signUp(request.username(), request.password());
        break;
      case server_client::LogInSignUp::LOGIN:
        logging("LogIn");
        response = Handler::logIn(request.username(), request.password());
        break;
      default:
        return;
    }
    const char* sender_buffer = response.c_str();
    long int sender_buffer_size = strlen(sender_buffer);

    logging("Enviando Responta");
    send(connection.sockFd, sender_buffer, sender_buffer_size, 0);
  }

  Handler::Handle(response, connection);
}

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cerr << "Modo de usar: ./server PORT" << std::endl;
    exit(1);
  }

  logging("Iniciando Servidor");

  int PORT = std::atoi(argv[1]);

  SockAttr controller;  // informações sobre o controller
  controller.inLen = sizeof(controller.in);

  SockAttr client;  // informações sobre o cliente

  logging("Criando Socket");
  controller.sockFd =
      socket(AF_INET, SOCK_STREAM, 0);  // cria socket do controller
  if (controller.sockFd == -1) {
    std::cerr << "Erro ao criar o socket do controlador" << std::endl;
    exit(1);
  }
  bzero(&(controller.in),
        controller.inLen);  // limpa o endereço do controlador

  controller.in.sin_family = AF_INET;  // define como familia do socket
  controller.in.sin_addr.s_addr = htonl(INADDR_ANY);  // define o endereço
  controller.in.sin_port = htons(PORT);               // define a porta

  logging("Executando o Bind");
  // executa o bind da conexao do socket
  if ((bind(controller.sockFd, (SockAddress*)&(controller.in),
            controller.inLen))) {
    close(controller.sockFd);
    std::cerr << "Erro ao executar o bind" << std::endl;
    exit(1);
  }

  logging("Definindo Backlog de conexões");
  if ((listen(controller.sockFd, 5))) {
    std::cerr << "Erro no listen" << std::endl;
    exit(1);
  }

  logging("Servidor Executando na Porta: " << PORT);

  client.inLen = sizeof(client.in);

  logging("Esperando Conexões");
  while (true) {
    client.sockFd = accept(controller.sockFd, (SockAddress*)&(client.in),
                           (unsigned int*)&client.inLen);

    char addressBuffer[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &client.in.sin_addr.s_addr, addressBuffer,
              INET_ADDRSTRLEN);

    logging("Conexão Aceita: " << addressBuffer << ":" << client.in.sin_port);
    if (client.sockFd < 0)
      std::cout << "Erro ao aceitar nova conexão" << std::endl;
    else {
      logging("Tratando conexão");
      std::thread(SignIn, client).detach();
    }
  }

  return 0;
}
