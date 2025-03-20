#include <print>

#include <spdlog/spdlog.h>

#include <sockpp/socket.h>
#include <sockpp/tcp_connector.h>
#include <sockpp/tcp_socket.h>

static const std::array<std::string, 6> commands = {"echo", "get", "set",
                                                    "del",  "key", "flush"};

auto read_stdin() noexcept -> std::string {
  std::flush(std::cout);
  std::string input;
  std::getline(std::cin, input);

  return input;
}

auto handle_connection(sockpp::tcp_connector &conn) noexcept -> void {
  std::print("[client] >>\t");
  auto input = read_stdin();

  if (input.empty())
    return;

  auto bytes_sent = conn.write(input.data(), input.size());

  if (bytes_sent == -1) {
    spdlog::warn("Error writing to socket: {}", conn.last_error_str());
    return;
  }

  if (input == "exit") {
    conn.close();
    return;
  }

  auto buffer = std::array<char, 1024>();
  auto bytes_received = conn.read(buffer.data(), buffer.size());

  if (bytes_received == -1) {
    spdlog::warn("Error reading from socket: {}", conn.last_error_str());
    return;
  }

  std::print("[server] >>\t{}\n", std::string(buffer.data(), bytes_received));
}

auto start(const char host[] = "localhost",
           const int16_t port = 6379) noexcept(false) -> void {
  auto conn = sockpp::tcp_connector();
  if (!conn.connect(sockpp::inet_address(host, port))) {
    spdlog::critical("Error connecting to server: {}", conn.last_error_str());

    throw std::runtime_error(
        std::format("Error connecting to server: {}", conn.last_error_str()));
  }
  spdlog::info("Connected to server");

  while (conn.is_open()) {
    handle_connection(conn);
  }

  spdlog::info("Closing connection");

  conn.close();
}

auto main(const int argc, const char *argv[]) noexcept -> int {
  start();

  spdlog::info("Client stopped");
}