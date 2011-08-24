#include <phoenix/phoenix.hpp>
#include <nall/png.hpp>

//NOTE: This is a workaround for the nall::png library, which complains that there are multiple definitions
//      due to this array. That shouldn't actually matter (because the array is declared as static const) but
//      for some reason gcc complains.

const unsigned nall::png::interlace[7][4] = {
  //x-distance, y-distance, x-origin, y-origin
  { 8, 8, 0, 0 },
  { 8, 8, 4, 0 },
  { 4, 8, 0, 4 },
  { 4, 4, 2, 0 },
  { 2, 4, 0, 2 },
  { 2, 2, 1, 0 },
  { 1, 2, 0, 1 },
};


