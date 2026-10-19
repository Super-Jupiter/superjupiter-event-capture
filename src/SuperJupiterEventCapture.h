// SPDX-License-Identifier: MIT
// Copyright (c) 2026 SuperJupiter
#ifndef SUPERJUPITER_EVENT_CAPTURE_H
#define SUPERJUPITER_EVENT_CAPTURE_H
#if defined(__FAST_MATH__) || (defined(__FINITE_MATH_ONLY__) && __FINITE_MATH_ONLY__ > 0)
#error "SuperJupiter requires IEEE non-finite checks; disable fast-math"
#endif
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>
#define SUPERJUPITER_EVENT_CAPTURE_VERSION "0.1.0"
namespace superjupiter {
namespace event_capture {
enum class Status { Accepted, Capturing, Complete, InvalidTime, Busy };
struct Sample {
  double raw;
  uint32_t atMs;
};
template <size_t Pre, size_t Post> class EventCapture {
  static_assert(Pre > 0 && Post > 0 && Pre + Post <= 65535,
                "EventCapture requires bounded positive capacities");
  Sample history_[Pre], capture_[Pre + Post];
  size_t head_, historyCount_, captured_, remaining_;
  bool triggered_, complete_, timed_;
  uint32_t last_;

public:
  EventCapture()
      : head_(0), historyCount_(0), captured_(0), remaining_(0), triggered_(false),
        complete_(false), timed_(false), last_(0) {}
  Status push(double raw, uint32_t atMs) {
    if (complete_)
      return Status::Complete;
    const uint32_t dt = atMs - last_;
    if (timed_ && (!dt || dt >= 0x80000000UL))
      return Status::InvalidTime;
    last_ = atMs;
    timed_ = true;
    const Sample sample = {raw, atMs};
    if (triggered_) {
      capture_[captured_++] = sample;
      --remaining_;
      if (!remaining_) {
        complete_ = true;
        return Status::Complete;
      }
      return Status::Capturing;
    }
    if (historyCount_ < Pre) {
      history_[(head_ + historyCount_) % Pre] = sample;
      ++historyCount_;
    } else {
      history_[head_] = sample;
      head_ = (head_ + 1) % Pre;
    }
    return Status::Accepted;
  }
  Status trigger() {
    if (triggered_)
      return Status::Busy;
    for (size_t i = 0; i < historyCount_; ++i)
      capture_[i] = history_[(head_ + i) % Pre];
    captured_ = historyCount_;
    remaining_ = Post;
    triggered_ = true;
    return Status::Capturing;
  }
  bool complete() const { return complete_; }
  size_t size() const { return captured_; }
  bool get(size_t i, Sample &out) const {
    if (i >= captured_)
      return false;
    out = capture_[i];
    return true;
  }
  void reset() {
    head_ = historyCount_ = captured_ = remaining_ = 0;
    triggered_ = complete_ = timed_ = false;
  }
};
} // namespace event_capture
} // namespace superjupiter
#endif
