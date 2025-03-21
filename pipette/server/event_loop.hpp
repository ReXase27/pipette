#pragma once

#include <coroutine>
#include <exception>
#include <queue>

namespace pipette::server {

class EventLoop final {
public:
  struct Task final {
    struct promise_type final {
      auto get_return_object() noexcept -> Task {
        return Task{std::coroutine_handle<promise_type>::from_promise(*this)};
      }
      auto initial_suspend() noexcept -> std::suspend_never { return {}; }
      auto final_suspend() noexcept -> std::suspend_never { return {}; }
      auto unhandled_exception() -> void { std::terminate(); }
      auto return_void() -> void {}

      std::coroutine_handle<> continuation;
    };

    Task(std::coroutine_handle<promise_type> handle) noexcept
        : handle(handle) {}
    ~Task() {
      if (handle)
        handle.destroy();
    }

    auto await_ready() noexcept -> bool { return false; }
    auto await_suspend(std::coroutine_handle<>) noexcept
        -> std::coroutine_handle<> {
      return handle;
    }
    auto await_resume() noexcept -> void {}

    std::coroutine_handle<promise_type> handle;
  };

  auto run() noexcept -> void;

  template <typename Func> auto add_task(Func &&func) noexcept -> void {
    static_assert(std::is_invocable_v<Func>, "Func must be invocable");
    auto task = func();
    m_tasks.push(task.handle);
  }

private:
  std::queue<std::coroutine_handle<>> m_tasks;
};

} // namespace pipette::server