#include "main.h"

ring_buffer_t tx_ring_buffer = {0};

int main(void)
{
  system_init();

  while (1)
  {
    flags_check();
  }
}
