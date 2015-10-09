#include <netdb.h>
#include <netinet/in.h>
#include <strings.h>
#include <fcntl.h>
#include <sys/socket.h>

#include "Socket.h"

namespace Network {

Socket* Socket::CreateClientSocket(
    const std::string hostname, const int port, bool block) {
  struct sockaddr_in server_addr;
  struct hostent *server;

  // Create socket
  int fd_ = socket(AF_INET, SOCK_STREAM, 0); 
  if (fd_ < 0) {
    fprintf(stderr, "ERROR: opening socket\n");
    return NULL;
  }

  // get server ip
  server = gethostbyname(hostname.c_str());
  if (server == NULL) {
    fprintf(stderr,"ERROR: hostname not found\n");
    return NULL;
  }

  // set server
  bzero((char *)&server_addr, sizeof(server_addr));
  server_addr.sin_family = AF_INET; // IPv4
  server_addr.sin_addr = *(struct in_addr *)*server->h_addr_list;
  server_addr.sin_port = htons(port);

  /* Now connect to the server */
  if (connect(fd_, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
    fprintf(stderr, "ERROR connecting\n");
    return NULL;
  }

  if (!block) {
    int x = fcntl(fd_, F_GETFL, 0);
    fcntl(fd_, F_SETFL, x | O_NONBLOCK);
  }

  Socket *new_socket = new Socket(hostname);
  new_socket->setFd(fd_);
  return new_socket;
}

Socket* Socket::CreateServerSocket(const int port, bool block) {
  struct sockaddr_in serv_addr;

  // Create socket
  int fd_ = socket(AF_INET, SOCK_STREAM, 0);
 
  if (fd_ < 0) {
    fprintf(stderr, "ERROR: opening socket\n");
    return NULL;
  }

  if (!block) {
    int x = fcntl(fd_, F_GETFL, 0);
    fcntl(fd_, F_SETFL, x | O_NONBLOCK);
  }

  // Set server addr
  bzero((char *) &serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(port);

  /* Now bind the host address using bind() call.*/
  if (bind(fd_, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
    fprintf(stderr,"ERROR: hostname binding server sockfd\n");
    return NULL;
  }

  // Start listening
  listen(fd_, 5);
  Socket *new_socket = new Socket();
  new_socket->setFd(fd_);
  return new_socket;
}

int Socket::Read(void* buffer, const int nbytes) const {
  if (!closed_ && fd_ > 0) {
    return read(fd_, buffer, nbytes);
  }
  return -1;
}

int Socket::Write(const void* buf, const int nbytes) const {
  if (!closed_ && fd_ > 0) {
    return write(fd_, buf, nbytes);
  }
  return -1;
}

int Socket::Send(void* buffer, const int nbytes) const {
  return Read(buffer, nbytes);
}

int Socket::Recv(const void* buffer, const int nbytes) const {
  return Write(buffer, nbytes);
}


}  // namespace Network