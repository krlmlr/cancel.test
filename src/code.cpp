#include <cpp11.hpp>

#include <ctime>

using namespace cpp11;

extern "C" {

// flag indicating whether R interrupts are pending
#ifndef _WIN32
extern int R_interrupts_pending;
#else
LibExtern int UserBreak;
#endif

};


[[cpp11::register]]
void fun() {
  for (int i = 0; i < 10000; i++) {
    timespec ts = {1, 0};
    nanosleep(&ts, NULL);
    cpp11::message("Iteration %d", i);
    if (R_interrupts_pending) {
      cpp11::stop("Interrupted!");
    }
  }
}
