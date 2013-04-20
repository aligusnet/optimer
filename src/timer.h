// Copyright (c) 2013 Alexander Ignatyev. All rights reserved.

#ifndef SRC_TIMER_H_
#define SRC_TIMER_H_

#include <chrono>

class Timer {
 public:
    Timer() {
        reset();
    }

    void reset() {
        start_ = std::chrono::steady_clock::now();
    }

    double elapsed_seconds() const {
        using std::chrono::duration_cast;
        using std::chrono::duration;
        using std::chrono::steady_clock;

        return duration_cast<duration<double>>(
            steady_clock::now() - start_).count();
    }

 private:
    std::chrono::steady_clock::time_point start_;
};

#endif  // SRC_TIMER_H_
