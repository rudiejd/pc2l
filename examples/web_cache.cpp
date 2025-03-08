#include "pc2l.h"
#include <boost/asio.hpp>
#include <boost/beast.hpp>
#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/thread.hpp>
#include <iostream>
#include <unordered_map>

boost::asio::io_service &io_service() {
  static boost::asio::io_service svc;
  return svc;
}

char local_data[1024] = {0};
char remote_data[1024] = {0};

void handle_read(boost::asio::ip::tcp::socket &read_from,
                 boost::asio::ip::tcp::socket &write_to, char *read_buffer,
                 size_t bytes, const boost::system::error_code &e,
                 pc2l::Vector<char *> &cachedPages,
                 std::unordered_map<std::string, size_t> &urlIndexMap,
                 std::unordered_map<std::string, bool> completeMap,
                 bool isLocal) {
  // this function is called whenever data is received

  // for debugging purposes, show the data in the console window
  // or write to file, or whatever...
  std::string data(read_buffer, read_buffer + bytes);

  // Parse HTTP request so we can store URL
  boost::beast::error_code ec;
  boost::beast::http::request_parser<boost::beast::http::string_body> parser;
  parser.put(boost::asio::buffer(data), ec);
  boost::beast::http::request<boost::beast::http::string_body> rq =
      parser.get();
  std::string identifier = rq[boost::beast::http::field::host].to_string() +
                           rq[boost::beast::http::field::uri].to_string();
  // Check unordered map to see if this page is cached
  if (!isLocal && urlIndexMap.find(identifier) != urlIndexMap.end()) {
    char *cached = cachedPages.at(urlIndexMap[identifier]);
    if (completeMap[identifier]) {
      std::cout << "Sending data: " << cached << std::endl;
      auto pageSize = strlen(cached);
      // Send cached page
      write_to.send(boost::asio::buffer(cached, pageSize));
      return;
    } else {
      char *retrieved = new char[data.size() + 1];
      strcpy(retrieved, data.c_str());
      strcat(cached, retrieved);
      cachedPages.replace(urlIndexMap[identifier], cached);
    }
  }

  // forward the received data on to "the other side"
  write_to.send(boost::asio::buffer(read_buffer, bytes));

  if (!isLocal && read_from.available() == 0) {
    completeMap[identifier] = true;
  } else {
    completeMap[identifier] = false;
  }
  if (!isLocal && urlIndexMap.find(identifier) == urlIndexMap.end()) {
    char *retrieved = new char[data.size() + 1];
    strcpy(retrieved, data.c_str());
    cachedPages.push_back(retrieved);
    urlIndexMap[identifier] = cachedPages.size() - 1;
  }
  // read more data from "this side"
  read_from.async_read_some(
      boost::asio::buffer(read_buffer, 1024),
      boost::bind(handle_read, boost::ref(read_from), boost::ref(write_to),
                  read_buffer, boost::asio::placeholders::bytes_transferred,
                  boost::asio::placeholders::error, cachedPages, urlIndexMap,
                  completeMap, isLocal));
  // cache retrieved page
}

int main(int argc, char **argv) {
  if (argc == 3) {

    auto &pc2l = pc2l::System::get();
    pc2l.initialize(argc, argv);
    pc2l.start();

    pc2l::Vector<char *> cachedPages;
    std::unordered_map<std::string, size_t> urlIndexMap;
    std::unordered_map<std::string, bool> completeMap;
    while (true) {
      std::string remoteAddr, remotePort;
      std::cout << "Enter remote address: " << std::endl;
      std::getline(std::cin, remoteAddr);
      std::cout << "Enter remote port: " << std::endl;
      std::getline(std::cin, remotePort);
      boost::asio::io_service::work w(io_service());
      boost::thread t(
          boost::bind(&boost::asio::io_service::run, (&io_service())));

      // extract the connection information from the command line
      boost::asio::ip::address local_address =
          boost::asio::ip::address::from_string(argv[1]);
      uint16_t local_port = boost::lexical_cast<uint16_t>(argv[2]);
      boost::asio::ip::address remote_address =
          boost::asio::ip::address::from_string(remoteAddr);
      uint16_t remote_port = boost::lexical_cast<uint16_t>(remotePort);

      boost::asio::ip::tcp::endpoint local_ep(local_address, local_port);
      boost::asio::ip::tcp::endpoint remote_ep(remote_address, remote_port);

      // start listening on the "local" socket -- note this does not
      // have to be local, you could in theory forward through a remote
      // device it's called "local" in the logical sense
      boost::asio::ip::tcp::acceptor listen(io_service(), local_ep);
      boost::asio::ip::tcp::socket local_socket(io_service());
      listen.accept(local_socket);

      // open the remote connection
      boost::asio::ip::tcp::socket remote_socket(io_service());
      remote_socket.open(remote_ep.protocol());
      remote_socket.connect(remote_ep);

      // start listening for data on the "local" connection
      local_socket.async_receive(
          boost::asio::buffer(local_data, 1024),
          boost::bind(handle_read, boost::ref(local_socket),
                      boost::ref(remote_socket), local_data,
                      boost::asio::placeholders::bytes_transferred,
                      boost::asio::placeholders::error, cachedPages,
                      urlIndexMap, completeMap, true));

      // also listen for data on the "remote" connection
      remote_socket.async_receive(
          boost::asio::buffer(remote_data, 1024),
          boost::bind(handle_read, boost::ref(remote_socket),
                      boost::ref(local_socket), remote_data,
                      boost::asio::placeholders::bytes_transferred,
                      boost::asio::placeholders::error, cachedPages,
                      urlIndexMap, completeMap, false));

      t.join();
    }
  } else {
    std::cout << "proxy <local ip> <port> <remote ip> <port>\n";
  }

  return 0;
}