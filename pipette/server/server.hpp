#pragma once

#include "pipette/server/event_loop.hpp"
#include <array>
#include <cstdint>
#include <string_view>

#include <sockpp/tcp_acceptor.h>

namespace pipette::server {

class PipetteServer final {
public:
  PipetteServer(const std::string_view address = "localhost",
                const std::uint16_t port = 6379) noexcept(false);
  ~PipetteServer() noexcept = default;

  auto start(EventLoop &event_loop) noexcept -> void;

private:
  auto handle_connection(sockpp::tcp_socket socket) noexcept -> EventLoop::Task;
  auto echo(sockpp::tcp_socket &conn, const std::string_view args) noexcept
      -> void;

  sockpp::tcp_acceptor m_acceptor;

  const std::array<std::string, 6> m_commands = {"echo", "get", "set",
                                                 "del",  "key", "flush"};
};

} // namespace pipette::server