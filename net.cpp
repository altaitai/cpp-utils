namespace net {

#ifdef _WIN32
void initSocket() {
  WSADATA wsaData;

  // Initialize Winsock"
  int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
  if (iResult != 0) {
    // TODO
    // printf("WSAStartup failed: %d\n", iResult);
    // return 1;
  }
}
#endif

std::string getLocalHostname() {
  char host[128] = "";
  gethostname(host, sizeof(host));
  return std::string(host);
}

/**
 * InetAddr
 */

InetAddr::InetAddr(const struct sockaddr_in sin) {
  char buffer[16];
  inet_ntop(AF_INET, &(sin.sin_addr), buffer, 16);
  mHost = buffer;
  mPort = ntohs(sin.sin_port);
}

std::string InetAddr::Print() const {
  std::ostringstream ss;
  ss << mHost << ":" << mPort;
  return ss.str();
}

struct sockaddr_in InetAddr::GetSockaddr() const {
  struct sockaddr_in sin = { 0 };
  if (mHost.empty()) {
    sin.sin_addr.s_addr = INADDR_ANY;
  } else {
    struct hostent *he = gethostbyname(mHost.c_str());
    if (he == NULL) {
      throw std::runtime_error("Cannot resolve hostname " + mHost);
    } else {
      memcpy(&sin.sin_addr, he->h_addr_list[0], he->h_length);
    }
  }
  sin.sin_family = AF_INET;
  sin.sin_port   = htons((short)mPort);
  return sin;
}

/**
 * Socket
 */

std::pair<Socket, InetAddr> Socket::Accept() {
  struct sockaddr_in sin;
  socklen_t size = sizeof(sin);
  int stat       = accept(mFd, (struct sockaddr *)&sin, &size);
  if (stat < 0) {
    return std::make_pair(0, InetAddr());
  }

  Socket newsock(stat);
  InetAddr addr(sin);
  return std::make_pair(newsock, addr);
}

void Socket::Bind(const InetAddr &addr) {
  struct sockaddr_in sin = addr.GetSockaddr();
  int stat               = bind(mFd, (struct sockaddr *)&sin, sizeof(sin));
  if (stat < 0) {
    throw std::runtime_error("Cannot bind socket to " + addr.Print() + " (" + strerror(errno) + ")");
  }
}

void Socket::Connect(const InetAddr &addr) {
  struct sockaddr_in sin = addr.GetSockaddr();
  int stat               = connect(mFd, (struct sockaddr *)&sin, sizeof(sin));
  if (stat < 0) {
    throw std::runtime_error("Cannot connect socket to " + addr.Print() + " (" + strerror(errno) + ")");
  }
}

void Socket::Open(const int socket_type) {
  switch (socket_type) {
    case SocketTCP: mFd = (int)socket(AF_INET, SOCK_STREAM, 0); break;
    default: mFd = (int)socket(AF_INET, SOCK_DGRAM, 0); break;
  }

  if (mFd < 0) {
    throw std::runtime_error("Cannot open socket (" + std::string(strerror(errno)) + ")");
  }
}

void Socket::Listen(const int backlog) {
  int stat = listen(mFd, backlog);
  if (stat < 0) {
    throw std::runtime_error("Cannot listen to socket (" + std::string(strerror(errno)) + ")");
  }
}

std::string Socket::Recv(const int bufsize) {
  std::vector<char> buffer(bufsize);
  std::string response;
  int nbytes = recv(mFd, &buffer[0], buffer.size(), 0);
  response.append(buffer.cbegin(), buffer.cend());

  if (nbytes > 0) {
    return response;
  } else
    return std::string();
}

std::pair<std::string, InetAddr> Socket::RecvFrom(const int bufsize) {
  struct sockaddr addr;
#ifdef _WIN32
  int laddr = sizeof(sockaddr);
#else
  unsigned int laddr = sizeof(sockaddr);
#endif
  std::vector<char> buffer(bufsize);
  std::string response;
  int numbytes = recvfrom(mFd, &buffer[0], bufsize, 0, &addr, &laddr);
  response.append(buffer.cbegin(), buffer.cend());

  // convert addr to sockaddr_in
  struct sockaddr_in *addr_in = (struct sockaddr_in *)&addr;
  InetAddr inet(*addr_in);

  if (numbytes <= 0) {
    return std::pair<std::string, InetAddr>();
  } else {
    return std::make_pair(response, inet);
  }
}

int Socket::Send(const std::string &data) {
  return send(mFd, data.c_str(), data.length(), 0);
}

int Socket::SendTo(const std::string &data, const InetAddr &addr) {
  struct sockaddr_in sin = addr.GetSockaddr();
  int stat = sendto(mFd, data.c_str(), data.length(), 0, (sockaddr *)&sin,
                    sizeof(sin));
  return stat;
}

void Socket::Reuse(const bool reuse) {
  int enable = (int)reuse;
  setsockopt(mFd, SOL_SOCKET, SO_REUSEADDR, (char *)&enable, sizeof(enable));
}

void Socket::Timeout(const int timeout) {
#ifdef _WIN32
  DWORD t = timeout;
#else
  struct timeval t;
  t.tv_sec  = timeout / 1000;
  t.tv_usec = timeout % 1000;
#endif
  setsockopt(mFd, SOL_SOCKET, SO_RCVTIMEO, (char *)&t, sizeof(t));
  setsockopt(mFd, SOL_SOCKET, SO_SNDTIMEO, (char *)&t, sizeof(t));
}

void Socket::Close() {
  close(mFd);
#ifdef _WIN32
  WSACleanup();
#endif
}

}
