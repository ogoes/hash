
#include <cstring>
#include <iostream>
#include <string>
#include <vector>

#include "server/net.h"

// protocol buffers
#include "communication/server_database.pb.h"

typedef struct sockaddr_in SocketInfo;

namespace DatabaseCommunication {

static char *DATABASE_ADDR;
static long int DATABASE_PORT = 0;

server_db::PlayerResponse receiveFromPlayerRequest(int);
server_db::GameResponse receiveFromGameRequest(int);

int sendRequest(server_db::Request);

}  // namespace DatabaseCommunication
