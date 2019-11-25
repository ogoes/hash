
// socket includes
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>


#define SockAddress struct sockaddr
#define SockIn struct sockaddr_in

#define RCV_BUFFER_SIZE (1024 * 8)

typedef struct {
  int sockFd;
  SockIn in;
  int inLen;
} SockAttr;



#if LOG
#define logging(x) std::cout << x << std::endl;
#else
#define logging(x)
#endif
