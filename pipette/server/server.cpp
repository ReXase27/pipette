#include <algorithm>
#include <pipette/server/server.hpp>

#include <pipette/server/event_loop.hpp>

#include <cassert>

#include <spdlog/spdlog.h>

static auto parse_query(const std::string_view input) noexcept
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

using namespace pipette::server;

PipetteServer::PipetteServer(const std::string_view address,
                             const std::uint16_t port) noexcept(false)
    : m_acceptor(sockpp::inet_address(address.data(), port)) {
  assert(m_acceptor.is_open());
}

auto PipetteServer::start(EventLoop &event_loop) noexcept -> void {
  spdlog::info("Server started");
  spdlog::info("Listening on port {}", m_acceptor.address().port());

  event_loop.add_task([this, &event_loop]() -> EventLoop::Task {
    while (true) {
      auto client = m_acceptor.accept();

      if (!client) {
        spdlog::warn("Error accepting incoming connection: {}",
                     client.last_error_str());

        continue;
      }

      spdlog::info("Accepted connection from: {}",
                   client.peer_address().to_string());

      event_loop.add_task(
          [this, client = std::move(client)]() mutable -> EventLoop::Task {
            co_await handle_connection(std::move(client));
          });
    }
  });
}

auto PipetteServer::handle_connection(sockpp::tcp_socket socket) noexcept
    -> EventLoop::Task {
  while (socket.is_open()) {
    std::array<char, 1024> buffer = {0};

    if (socket.read(buffer.data(), buffer.size()) == -1) {
      spdlog::warn("Error reading from socket: {}", socket.last_error_str());
      co_return;
    }

    auto [command, args] = parse_query(std::string_view(buffer.data()));

    if (command == "exit") {
      spdlog::info("Client requested to exit");
      socket.close();
      co_return;
    }

    if (!std::ranges::contains(m_commands, command)) {
      socket.write("Invalid command\n", 15);
      co_return;
    }

    auto idx =
        std::distance(m_commands.begin(),
                      std::find(m_commands.begin(), m_commands.end(), command));

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

  spdlog::info("Connection closed");
}

auto PipetteServer::echo(sockpp::tcp_socket &conn,
                         const std::string_view args) noexcept -> void {
  if (conn.write_n(args.data(), args.size()) == -1) {
    spdlog::warn("Error writing to socket: {}", conn.last_error_str());
    return;
  }
}