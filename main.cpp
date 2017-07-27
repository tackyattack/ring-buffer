
#include <iostream>
#include <thread>
#include <chrono>

using namespace std;

#define BUFFER_OK          0
#define BUFFER_ERROR_FULL  1
#define BUFFER_ERROR_EMPTY 2


#define RING_BUFFER_SIZE 20000
#define BYTE_PER_SAMPLE 2 // 2 bytes per uint16_t
uint16_t ring_buffer[RING_BUFFER_SIZE] = {0};
volatile uint16_t ring_buffer_head = 0;
volatile uint16_t ring_buffer_tail = 0;
volatile uint16_t ring_buffer_count = 0;

uint16_t ring_buffer_get_free()
{
  return (RING_BUFFER_SIZE - ring_buffer_count);
}

uint16_t ring_buffer_get_count()
{
  return ring_buffer_count;
}

uint16_t ring_buffer_dist_to_wrap(uint16_t pos)
{
  return (RING_BUFFER_SIZE) - pos;
}

uint16_t ring_buffer_push(uint16_t *data, uint16_t size)
{
  if( size > ring_buffer_get_free())
  {
    return BUFFER_ERROR_FULL;
  }

  if(size > ring_buffer_dist_to_wrap(ring_buffer_head))
  { // wrap copy
    uint16_t w1 = ring_buffer_dist_to_wrap(ring_buffer_head);
    uint16_t w2 = size - w1;
    memcpy(&ring_buffer[ring_buffer_head], data, w1*BYTE_PER_SAMPLE); // memcpy needs to know how many BYTES
    memcpy(&ring_buffer[0], data+1, w2*BYTE_PER_SAMPLE);
  }
  else
  { // straight copy
    memcpy(&ring_buffer[ring_buffer_head], data, size*BYTE_PER_SAMPLE);
  }

  ring_buffer_head = (ring_buffer_head + size) % RING_BUFFER_SIZE;
  ring_buffer_count = ring_buffer_count + size;

  return BUFFER_OK;

}

uint16_t ring_buffer_pop(uint16_t *sample)
{
  if( ring_buffer_get_count() < 1 )
  {
    return BUFFER_ERROR_EMPTY;
  }

  *sample = ring_buffer[ring_buffer_tail];

  ring_buffer_tail = (ring_buffer_tail + 1) % RING_BUFFER_SIZE;

  ring_buffer_count--;

  return BUFFER_OK;

}

void ring_buffer_clear()
{
  ring_buffer_head = 0;
  ring_buffer_tail = 0;
  ring_buffer_count = 0;
}

//-----end of C only------

void ring_buffer_print()
{
  for(uint16_t i = 0; i < RING_BUFFER_SIZE; i++)
  {
    cout << "[" << ring_buffer[i] << "]";
  }
  cout << endl;
  string ht;
  for(uint16_t i = 0; i < RING_BUFFER_SIZE; i++)
  {
    if(i == ring_buffer_head)
    {
      ht = "H";
    }
    else
    {
      ht = "0";
    }
    cout << "[" << ht << "]";
  }
  cout << endl;
  for(uint16_t i = 0; i < RING_BUFFER_SIZE; i++)
  {
    if(i == ring_buffer_tail)
    {
      ht = "T";
    }
    else
    {
      ht = "0";
    }
    cout << "[" << ht << "]";
  }
  cout << endl;
  cout << "item count: " << ring_buffer_get_count() << endl;
}

void timeout_ms(uint16_t ms)
{
  this_thread::sleep_for (chrono::milliseconds(ms));
}

void task_1()
{
  uint16_t data[512] = {0};
  while(1)
  {
    while(ring_buffer_push(data, 512) != BUFFER_OK)
    timeout_ms(10);
  }
}

void task_2()
{
  uint16_t sample = 0;
  while(1)
  {
    ring_buffer_pop(&sample);
    timeout_ms(100);
  }
}

void task_3()
{
  while(1)
  {
    cout << "ring buffer objects: " << ring_buffer_get_count() << endl;
    timeout_ms(1000);
  }
}

int main()
{
   cout << "starting threads..." << endl;
  thread first (task_1);
  thread second (task_2);
  thread third (task_3);

  //  uint16_t data[] = {1,2,3,4};
  // cout << "push with return: " << ring_buffer_push(data, 4) << endl;
  // cout << "push with return: " << ring_buffer_push(data, 4) << endl;
  // cout << "push with return: " << ring_buffer_push(data, 2) << endl;
  // ring_buffer_print();
  //
  // uint16_t sample = 0;
  // cout << "pop with return: " << ring_buffer_pop(&sample) << " and sample: " << sample << endl;
  // cout << "pop with return: " << ring_buffer_pop(&sample) << " and sample: " << sample << endl;
  // cout << "push with return: " << ring_buffer_push(data, 1) << endl;
  //
  // ring_buffer_print();
  //
  // cout << "pop with return: " << ring_buffer_pop(&sample) << " and sample: " << sample << endl;
  // cout << "pop with return: " << ring_buffer_pop(&sample) << " and sample: " << sample << endl;
  // cout << "pop with return: " << ring_buffer_pop(&sample) << " and sample: " << sample << endl;
  // cout << "pop with return: " << ring_buffer_pop(&sample) << " and sample: " << sample << endl;
  // cout << "pop with return: " << ring_buffer_pop(&sample) << " and sample: " << sample << endl;
  // cout << "pop with return: " << ring_buffer_pop(&sample) << " and sample: " << sample << endl;
  // cout << "pop with return: " << ring_buffer_pop(&sample) << " and sample: " << sample << endl;
  // cout << "pop with return: " << ring_buffer_pop(&sample) << " and sample: " << sample << endl;
  // cout << "pop with return: " << ring_buffer_pop(&sample) << " and sample: " << sample << endl;
  // cout << "pop with return: " << ring_buffer_pop(&sample) << " and sample: " << sample << endl;
  // cout << "pop with return: " << ring_buffer_pop(&sample) << " and sample: " << sample << endl;
  // cout << "pop with return: " << ring_buffer_pop(&sample) << " and sample: " << sample << endl;
  // cout << "pop with return: " << ring_buffer_pop(&sample) << " and sample: " << sample << endl;
  // cout << "pop with return: " << ring_buffer_pop(&sample) << " and sample: " << sample << endl;
  // ring_buffer_print();


  while(1)
  {

  }


  return 0;
}
