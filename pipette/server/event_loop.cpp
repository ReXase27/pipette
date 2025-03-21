#include <pipette/server/event_loop.hpp>

using namespace pipette::server;

auto EventLoop::run() noexcept -> void {
  while (!m_tasks.empty()) {
    auto task = std::move(m_tasks.front());
    m_tasks.pop();
    task();
  }
}