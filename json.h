#ifndef json_h_
#define json_h_

#include "duktape.h"

void martin_parse_json(duk_context *ctx, char *str, int len);

#endif
