#include <algorithm>
#include <array>
#include <cctype>
#include <cstdint>
#include <format>
#include <print>
#include <sockpp/tcp_socket.h>

#include <spdlog/spdlog.h>

#include <sockpp/tcp_acceptor.h>

static const std::array<std::string, 6> commands = {"echo", "get", "set",
                                                    "del",  "key", "flush"};

auto echo(sockpp::tcp_socket &conn, const std::string_view args) noexcept
    -> void {
  auto bytes_sent = conn.write_n(args.data(), args.size());

  if (bytes_sent == -1) {
    spdlog::warn("Error writing to socket: {}", conn.last_error_str());
    return;
  }
}

auto parse_command(const std::string_view input) noexcept
    -> std::pair<std::string, std::string> {
  auto command = std::string();
  auto args = std::string();

  auto it = std::find_if(input.begin(), input.end(),
                         [](const char c) { return std::isspace(c); });

  if (it != input.end()) {
    command = std::string(input.begin(), it);
    args = std::string(it + 1, input.end());
  } else {
    command = std::string(input);
  }

  return {command, args};
}

auto handle_connection(sockpp::tcp_socket &socket) noexcept -> void {
  std::array<char, 1024> buffer = {0};

  if (socket.read(buffer.data(), buffer.size()) == -1) {
    spdlog::warn("Error reading from socket: {}", socket.last_error_str());
    return;
  }

  auto [command, args] = parse_command(std::string_view(buffer.data()));

  if (!std::ranges::contains(commands, command)) {
    socket.write("Invalid command\n", 15);
    return;
  }

  auto idx = std::distance(
      commands.begin(), std::find(commands.begin(), commands.end(), command));

  switch (idx) {
  case 0:
    spdlog::info("Echo command");
    echo(socket, args);
    break;
  default:
    socket.write("Command not implemented\n", 23);
    break;
  }
}

auto start(const char host[] = "localhost",
           const int16_t port = 6379) noexcept(false) -> void {
  auto acceptor = sockpp::tcp_acceptor(port);
  if (!acceptor) {
    spdlog::critical("Error creating acceptor: {}", acceptor.last_error_str());

    throw std::runtime_error(
        std::format("Error creating acceptor: {}", acceptor.last_error_str()));
  }

  spdlog::info("Server started");
  spdlog::info("Listening on port {}", port);

  while (true) {
    auto client = acceptor.accept();

    if (!client) {
      spdlog::warn("Error accepting incoming connection: {}",
                   client.last_error_str());

      continue;
    }

    spdlog::info("Accepted connection from: {}",
                 client.peer_address().to_string());

    while (true) {
      handle_connection(client);
    }
  }
}

auto main(const int argc, const char *argv[]) noexcept -> int {
  start();

  spdlog::info("Server stopped.");
}