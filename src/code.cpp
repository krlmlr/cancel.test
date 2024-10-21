#include <cpp11.hpp>
#include "cpp11/protect.hpp"

#include <ctime>
#include <csignal>

#include <R_ext/GraphicsEngine.h>

extern "C" {
  void Rf_onintrNoResume();

  extern Rboolean R_interrupts_suspended;
}

class LocalSignalHandler {
private:
  bool signal_received = false;
  // oldhandler stores the old signal handler
  // so that it can be restored when the object is destroyed
  typedef void (*sig_t)(int);
  sig_t oldhandler;

  static LocalSignalHandler* instance;

public:
  LocalSignalHandler() {
    // cpp11::message("LocalSignalHandler constructor");
    if (instance != nullptr) {
      throw("Only one instance of LocalSignalHandler is allowed");
    }
    instance = this;
    oldhandler = signal(SIGINT, signal_handler);
  }

  ~LocalSignalHandler() {
    // Messages crash on Windows
    // cpp11::message("LocalSignalHandler destructor");
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

  cpp11::message("Interrupts are suspended: %d", R_interrupts_suspended);

  for (int i = 0; i < 30; i++) {
    cpp11::message("Iteration %d", i);
    timespec ts = {0, 100000000};
    nanosleep(&ts, NULL);
    if (handler.GetSignalReceived()) {
      // Why does this message not come through in RStudio IDE on Windows?
      cpp11::message("Interrupts are suspended: %d", R_interrupts_suspended);
      cpp11::safe[Rf_onintr]();
      // FIXME: Is the following better? cpp11::safe[Rf_onintrNoResume]();
      break;
    }
  }

  cpp11::message("Done");
}
