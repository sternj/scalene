#pragma once

#include <cmath>
#include <iostream>
#include <thread>
#include <unistd.h>

#include <chrono>
#include <random>

//#include "mwc.h"

#define SAMPLER_DETERMINISTIC 0
#define SAMPLER_LOWDISCREPANCY 1

#include <pthread.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#if SAMPLER_LOWDISCREPANCY
#include "lowdiscrepancy.hpp"
#endif

template <uint64_t SAMPLE_RATE>
class Sampler {
private:
  uint64_t _next;
#if !SAMPLER_DETERMINISTIC
#if !SAMPLER_LOWDISCREPANCY
  std::mt19937_64 rng { 1234567890UL + (uint64_t) getpid() + (uint64_t) pthread_self() };
#else
  LowDiscrepancy rng { 1234567890UL + (uint64_t) getpid() + (uint64_t) pthread_self() };
#endif
  std::geometric_distribution<uint64_t> geom { SAMPLE_PROBABILITY };
#endif
  
public:
  Sampler()
  {
#if !SAMPLER_DETERMINISTIC
    _next = geom(rng); // SAMPLE_RATE;
#else
    _next = SAMPLE_RATE;
#endif
  }
  
  inline ATTRIBUTE_ALWAYS_INLINE uint64_t sample(uint64_t sz) {
    if (unlikely(_next <= sz)) {
      return updateSample(sz - _next);
    }
    _next -= sz;
    return 0;
  }
  
private:

  uint64_t updateSample(uint64_t sz) {
#if SAMPLER_DETERMINISTIC
    _next = SAMPLE_RATE;
#else
    while (true) {
      _next = geom(rng);
      if (_next != 0) {
	break;
      }
    }
#endif
    return sz * SAMPLE_PROBABILITY + 1;
  }
  
  static constexpr double SAMPLE_PROBABILITY = (double) 1.0 / (double) SAMPLE_RATE;
};
