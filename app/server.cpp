#include <pipette/server/event_loop.hpp>
#include <pipette/server/server.hpp>

#include <spdlog/spdlog.h>

auto main(const int argc, const char *argv[]) noexcept -> int {
  auto event_loop = pipette::server::EventLoop();
  auto server = pipette::server::PipetteServer();

  server.start(event_loop);
  event_loop.run();

  spdlog::info("Server stopped");
}