#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <array>
#include <string>
#include <cstdlib>
#include <cerrno>
#include <cstring>

using namespace std;

int inet_pton_ipv4(int sock_family, const char *strptr, void *addrptr);
const char* inet_ntop_ipv4(int family, const void *addrptr, char *strptr, size_t len);

int inet_pton_ipv4(int sock_family, const char *strptr, void *addrptr) {
  if (sock_family == AF_INET) {
    struct in_addr in_val;

    if (inet_aton(strptr, &in_val)) {
      cout << "sizeof(struct in_addr): " << sizeof(struct in_addr) << '\n';
      memcpy(addrptr, &in_val, sizeof(struct in_addr));
      return 1;
    }

    return 0;
  }

  errno = EAFNOSUPPORT;
  return -1;
}

const char* inet_ntop_ipv4(int family, const void *addrptr, char *strptr, size_t len) {
  const u_char *p = (const u_char *)addrptr;

  if (family == AF_INET) {
    char temp[16];

    snprintf(temp, sizeof(temp), "%d.%d.%d.%d", p[0], p[1], p[2], p[3]);
    if (strlen(temp) >= len) {
      cout << "No space" << '\n';
      errno = ENOSPC;
      return NULL;
    }
    strcpy(strptr, temp);
    return strptr;
  }
  cout << "No support" << '\n';
  errno = EAFNOSUPPORT;
  return NULL;
}

char *sock_ntop(const struct sockaddr *sa, socklen_t salen) {
  char portstr[7];
  static char str[128]; // not thread safe

  switch (sa->sa_family) {
    case AF_INET: {  // TODO: AF_INET6, AF_UNIX, AF_LINK
      struct sockaddr_in *sin = (struct sockaddr_in *) sa;

      if (inet_ntop_ipv4(AF_INET, &sin->sin_addr, str, sizeof(str)) == NULL) {
        return NULL;
      }
      if (ntohs(sin->sin_port) != 0) {
        snprintf(portstr, sizeof(portstr), ".%d", ntohs(sin->sin_port));
        strcat(str, portstr);
      }
    }
    return str;
  }
  errno = EAFNOSUPPORT;
  return NULL;
}

int inet_pton_loose(int family, const char* strptr, void *addrptr) {
  if (family == AF_INET || family == AF_INET6) {
    int pton_inet = inet_pton(family, strptr, addrptr);
    if (pton_inet == 0) {
      return inet_aton();
    } else {
      return pton_inet;
    }
  }

  return -1;
}

int main() {
  char p_addr[] = "127.0.0.1";
  char *n_addr = new char(4);
  cout << "sizeof(n_addr) before: " << sizeof(n_addr) << '\n';
  int result = inet_pton_ipv4(AF_INET, p_addr, n_addr);
  cout << "sizeof(n_addr) after: " << sizeof(n_addr) << '\n';
  if (result < 0) {
    cout << "inet_pton_ipv4() failed: " << strerror(errno) << '\n';
    exit(1);
  } else {
    for (int i = 0; i < sizeof(n_addr); ++i) {
      printf("%02x\n", ((uint8_t *)n_addr)[i]);
    }
  }

  cout << "sizeof(p_addr): " << sizeof(p_addr) << '\n';

  char converted_addr[sizeof(p_addr)];

  cout << "p_addr p_addr addr: " << &p_addr << '\n';
  cout << "p_addr p_addr end addr: " << (&p_addr + sizeof(p_addr)) << '\n';
  cout << "n_addr n_addr addr: " << &n_addr << '\n';
  cout << "converted_addr addr: " << &converted_addr << '\n';
  cout << "sizeof(converted_addr): " << sizeof(converted_addr) << '\n';

  struct sockaddr_in serv;

  // or memset(&serv, 0, sizeof(serv));
  serv.sin_family = AF_INET;
  serv.sin_port = htons(9999);
  serv.sin_addr.s_addr = htons();
  memset(&serv.sin_zero, 0, sizeof(serv.sin_zero));

  if (sock_ntop((struct sockaddr *)&serv, sizeof(serv)) != NULL) {
    cout << "Converted address: " << converted_addr << '\n';
  } else {
    cout << "Failed to convert address back" << '\n';
  }

  delete []n_addr;

  return 0;
}
