#ifndef STPRINTF_H
#define STPRINTF_H

#pragma once

// Written by Emery Berger

#include <string.h>
#include <cmath>
#include <cstdint>

using namespace std;

namespace stprintf {
  
  template <typename T>
    inline int writeval(char * buf, T v);

  template <typename T>
  inline int itoa(char * buf, T v) {
    long n = (long) v;
    auto startbuf = buf;
    if (n < 0) {
      *buf++ = '-';
      n = -n;
    }
    if (n == 0) {
      *buf++ = '0';
      return (int) (buf - startbuf);
    }
    long tens = 1L;
    while (n / (10 * tens)) {
      tens *= 10;
    }
    while (tens) {
      *buf++ = '0' + n / tens;
      n = n - (n / tens) * tens;
      tens /= 10;
    }
    return (int) (buf - startbuf);
  }

  //TODO: perhaps make this a bit more tolerant of invalid numbers?
  inline int atoi(char* buf) {
    bool negative = false;
    int res = 0;
    if(*buf == '-') {
      negative = true;
      buf++;
    }
    while(*buf != '\0') {
      res *= 10;
      res += *buf++ - '0';
    }
    return negative ? -res : res;
    // if (i < 0) {
    //   *buf++ = '-';
    //   i = -i;
    // }
    // do {
    //   *buf++ = (i / 10) + '0';
    //   i %= 10;
    // } while(i >= 0);
    // *buf = '\0';
  }

  inline int ftoa(char * buf, double n, int decimalPlaces = 8) {
    // Extract integer part
    auto ipart = (long) n;
  
    // Extract floating part
    auto fpart = n - (double) ipart;
    if (fpart < 0.0) {
      fpart = -fpart;
    }
  
    // convert integer part to string
    int i = itoa(buf, ipart);
  
    if (decimalPlaces > 0) {
      buf[i] = '.';
      auto multiple = pow(10, decimalPlaces);
      fpart = fpart * multiple;
      multiple /= 10;
      while ((fpart < multiple) && (decimalPlaces > 0)) {
	buf[++i] = '0';
	multiple /= 10;
	decimalPlaces--;
      }
      if (fpart > 0) {
	i = i + itoa(buf + i + 1, (long) fpart) + 1;
      }
    }
    return i;
  }

  inline int writeval(char * buf, double n) {
    int len = ftoa(buf, n);
    return len;
  }

  inline int writeval(char * buf, float n) {
    int len = ftoa(buf, n);
    return len;
  }

  inline int writeval(char * buf, const char * str) {
    auto len = strlen(str);
    //    cout << "len = " << len << ", str = " << str << endl;
    for (auto i = 0; i < len + 1; i++) {
      buf[i] = str[i];
    }
    return len;
  }

  inline int writeval(char * buf, const char c) {
    buf[0] = c;
    return 1;
  }

  inline int writeval(char * buf, uint64_t n) {
    int len = itoa(buf, n);
    return len;
  }

  template <class T>
    inline int writeval(char * buf, T n) {
    int len = itoa(buf, n);
    return len;
  }

  inline void stprintf(char * buf, const char* format) // base function
  {
    writeval(buf, format);
  }

  template<typename T, typename... Targs>
    inline void stprintf(char * buf, const char * format, T value, Targs... Fargs)
  {
    for ( ; *format != '\0'; format++ ) {
      if ( *format == '@' ) {
	if ( *(format + 1) == '\\') {
	  auto len = writeval(buf, "@");
	  buf += len;
	  format = format+2;
	} else {
	  auto len = writeval(buf, value);
	  buf += len;
	  stprintf(buf, format+1, Fargs...);
	  return;
	}
      }
      buf += writeval(buf, *format);
    }
  }

}


#endif
