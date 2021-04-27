#ifndef json_h_
#define json_h_

#include "duktape.h"

void martin_parse_json_eat_whitespace(char *str, int *i);
void martin_parse_json_string(duk_context *ctx, char *str, int *i);
void martin_parse_json_object(duk_context *ctx, char *str, int *i);
void martin_parse_json_value(duk_context *ctx, char *str, int *i);
void martin_parse_json(duk_context *ctx, char *str, int len);

#endif
