#include <pipette/server/server.hpp>

auto main(const int argc, const char *argv[]) noexcept -> int {
  auto server = pipette::server::PipetteServer();
  server.start();
}