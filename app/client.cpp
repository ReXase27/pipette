#include <pipette/client/client.hpp>

auto main(const int argc, const char *argv[]) noexcept -> int {
  auto client = pipette::client::PipetteClient("localhost", 6379);
  client.start();
}