#include <stdint.h>
#include <time.h>

int execution_timeout_callback(void *udata) {
  if(udata == NULL) return 0;
  int64_t *t = (int64_t *) udata;
  int64_t timeout = *t;
  if(timeout == 0) {
    return 0;
  }
  time_t timeNow = time(NULL);
  if(timeNow >= timeout) {
    return 1;
  }
  return 0;
}
