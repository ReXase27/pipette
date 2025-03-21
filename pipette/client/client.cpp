#include <cassert>
#include <expected>
#include <print>

#include <pipette/client/client.hpp>

#include <spdlog/spdlog.h>

static auto read_stdin() noexcept -> std::string {
  std::print("[client] >>\t");
  std::string input;
  std::getline(std::cin, input);

  return input;
}

using namespace pipette::client;

PipetteClient::PipetteClient(const std::string &host,
                             const int16_t port) noexcept(false) {
  m_conn.connect(sockpp::inet_address(host, port));
  assert(m_conn.is_open());
}

auto PipetteClient::start() noexcept(false) -> void {
  spdlog::info("Connected to server");

  handle_connection();

  spdlog::info("Client stopped");
}

auto PipetteClient::query(const std::string &input) noexcept
    -> std::expected<std::string, std::string> {

  if (m_conn.write(input.data(), input.size()) == -1) {
    spdlog::warn("Error writing to socket: {}", m_conn.last_error_str());
    return std::unexpected<std::string>(m_conn.last_error_str());
  }

  const auto bytes_received = m_conn.read(m_buffer.data(), m_buffer.size());

  if (bytes_received == -1) {
    spdlog::warn("Error reading from socket: {}", m_conn.last_error_str());
    return std::unexpected<std::string>(m_conn.last_error_str());
  }

  return std::string(m_buffer.data(), bytes_received);
}

auto PipetteClient::handle_connection() noexcept -> void {
  while (m_conn.is_open()) {
    const auto input = read_stdin();

    if (input.empty())
      continue;

    if (input == "exit") {
      terminate_connection();
      return;
    }

    const auto response = query(input);

    if (!response) {
      std::print(std::cerr, "[error] >>\t{}\n", response.error());
      continue;
    }

    std::print("[server] >>\t{}\n", response.value());
  }
}

auto PipetteClient::terminate_connection() noexcept -> void {
  if (m_conn.write("exit", 4) == -1) {
    spdlog::warn("Error writing to socket: {}", m_conn.last_error_str());
    return;
  }

  m_conn.close();
  spdlog::info("Connection closed");
}