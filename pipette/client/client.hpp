#pragma once

#include <cstdint>
#include <expected>

#include <sockpp/tcp_connector.h>

namespace pipette::client {

class PipetteClient final {
public:
  PipetteClient(const std::string &host = "localhost",
                int16_t port = 6379) noexcept(false);
  ~PipetteClient() = default;

  auto start() noexcept(false) -> void;

  auto query(const std::string &input) noexcept
      -> std::expected<std::string, std::string>;

private:
  PipetteClient(const PipetteClient &) = delete;
  PipetteClient &operator=(const PipetteClient &) = delete;
  PipetteClient(PipetteClient &&) = delete;
  PipetteClient &operator=(PipetteClient &&) = delete;

  auto handle_connection() noexcept -> void;
  auto terminate_connection() noexcept -> void;

  sockpp::tcp_connector m_conn;
  std::array<char, 1024> m_buffer = {};
};

} // namespace pipette::client