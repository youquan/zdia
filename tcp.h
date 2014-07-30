#ifndef _TCP_H_
#define _TCP_H_

#include <netinet/in.h>

int tcp_create_bind(int *sock, struct sockaddr *sa, socklen_t len);
int tcp_listen(int sock);

#endif
