#include <cpp11.hpp>

#include <ctime>
#include <csignal>

using namespace cpp11;

class LocalSignalHandler {
private:
  bool signal_received = false;
  // oldhandler stores the old signal handler
  // so that it can be restored when the object is destroyed
  sig_t oldhandler;

  static LocalSignalHandler* instance;

public:
  LocalSignalHandler() {
    if (instance != nullptr) {
      throw("Only one instance of LocalSignalHandler is allowed");
    }
    instance = this;
    oldhandler = signal(SIGINT, signal_handler);
  }

  ~LocalSignalHandler() {
    signal(SIGINT, oldhandler);
    instance = nullptr;
  }

  bool GetSignalReceived() const {
    return signal_received;
  }

private:
  static void signal_handler(int signum) {
    if (signum == SIGINT) {
      instance->signal_received = true;
    }
  }
};

LocalSignalHandler* LocalSignalHandler::instance = nullptr;

[[cpp11::register]]
void fun() {
  LocalSignalHandler handler;

  for (int i = 0; i < 10000; i++) {
    timespec ts = {1, 0};
    nanosleep(&ts, NULL);
    cpp11::message("Iteration %d", i);
    if (handler.GetSignalReceived()) {
      cpp11::stop("Interrupted!");
    }
  }
}
