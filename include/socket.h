#pragma once

#include <iostream>
#include <memory>

#ifdef _WIN32
#include <winsock2.h>
#endif
#ifndef _WIN32
#include <sys/socket.h>
#endif
#ifndef _WIN32
#include <netinet/in.h>
#endif
#ifndef _WIN32
#include <sys/select.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifndef _WIN32
#include <arpa/inet.h>
#endif
#ifndef _WIN32
#include <sys/time.h>
#endif
#include <sys/types.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <ctype.h>

namespace api {
  class socketImpl {

  public:
    socketImpl(const char* _hostname, int _port) : hostname(_hostname), port(_port) {
      makeSocket();
    }
    ~socketImpl() {
#ifdef _WIN32
      closesocket(*sock);
#else
      close(sock);
#endif
    }
    int connectImpl() {
      if (connect(*sock, (struct sockaddr*)(&sin),
        sizeof(struct sockaddr_in)) != 0) {
        fprintf(stderr, "failed to connect!\n");
        return -1;
      }
      return 0;
    }
    std::shared_ptr<int> getSocket() {
      return sock;
    }
  private:
    const char* hostname;
    int port;
    unsigned long hostaddr;
    std::shared_ptr<int> sock;
    struct sockaddr_in sin;
    
    int makeSocket() {
#ifdef WIN32
      WSADATA wsadata;
      int err;

      err = WSAStartup(MAKEWORD(2, 0), &wsadata);
      if (err != 0) {
        fprintf(stderr, "WSAStartup failed with error: %d\n", err);
        return 1;
      }
#endif
      hostaddr = inet_addr(hostname);
      /* Ultra basic "connect to port 22 on localhost"
      * Your code is responsible for creating the socket establishing the
      * connection
      */
      int sockImpl = socket(AF_INET, SOCK_STREAM, 0);

      sock = std::make_shared<int>();
      *sock = sockImpl;

      sin.sin_family = AF_INET;
      sin.sin_port = htons(port);
      sin.sin_addr.s_addr = hostaddr;
    }

  };
}