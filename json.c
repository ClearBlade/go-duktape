#include <stdio.h>
#include <stdbool.h>
#include "duktape.h"
#include "json.h"

void martin_parse_json_eat_whitespace(char *str, int *i) {
    for(;;){
        switch(str[*i]){
        case 0x09:
        case 0x0A:
        case 0x0D:
        case 0x20:
            (*i)++;
            break;
        default:
            return;
        }
    }
}

void martin_parse_json_string(duk_context *ctx, char *str, int *i) {
    int writePtr;

    writePtr = 0;
    (*i)++; // initial double-quote
    for(;;){
        switch(str[*i]){
        case '"':
            duk_push_lstring(ctx, str, writePtr);
            (*i)++; // final double-quote
            return;
        case '\\':
            // escaped char
            (*i)++;
            switch(str[*i]){
            case '"':
                str[writePtr++] = '"';
                break;
            case '\\':
                str[writePtr++] = '\\';
                break;
            case '/':
                str[writePtr++] = '/';
                break;
            case 'b':
                str[writePtr++] = '\b';
                break;
            case 'f':
                str[writePtr++] = '\f';
                break;
            case 'n':
                str[writePtr++] = '\n';
                break;
            case 'r':
                str[writePtr++] = '\r';
                break;
            // TODO unicode case
            // default case not checked
            }
            (*i)++;
            break;
        default:
            str[writePtr++]=str[*i];
            (*i)++;
            break;
        }
    }
}

void martin_parse_json_number(duk_context *ctx, char *str, int *i) {
    char *end;
    double num;

    num = strtod(&str[*i], &end);
    duk_push_number(ctx, num);
    while(&str[*i]!=end)(*i)++;
}

void martin_parse_json_array(duk_context *ctx, char *str, int *i) {
    int arrIdx, index;

    index = 0;
    arrIdx = duk_push_array(ctx);
    (*i)++; // open bracket
    for(;;){
        martin_parse_json_eat_whitespace(str, i);
        switch(str[*i]){
        case ']':
            (*i)++; // close bracket
            return;
        case ',':
            (*i)++;
            break;
        default:
            martin_parse_json_value(ctx, str, i);
            duk_put_prop_index(ctx, arrIdx, index);
            index++;
            break;
        }
    }
}

void martin_parse_json_object(duk_context *ctx, char *str, int *i) {
    int objIdx;

    objIdx = duk_push_object(ctx);
    (*i)++; // open curly
    for(;;){
        martin_parse_json_eat_whitespace(str, i);
        switch(str[*i]){
        case '}':
            (*i)++; // close curly
            return;
        case ',':
            (*i)++;
            break;
        case '"':
            martin_parse_json_string(ctx, str, i);
            (*i)++; // colon
            martin_parse_json_value(ctx, str, i);
            duk_put_prop(ctx, objIdx);
            break;
        }
    }
}

void martin_parse_json_value(duk_context *ctx, char *str, int *i) {
    martin_parse_json_eat_whitespace(str, i);
    switch(str[*i]){
    case '-':
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
        martin_parse_json_number(ctx, str, i);
        break;
    case '"':
        martin_parse_json_string(ctx, str, i);
        break;
    case '{':
        martin_parse_json_object(ctx, str, i);
        break;
    case '[':
        martin_parse_json_array(ctx, str, i);
        break;
    case 't':
        // true
        duk_push_true(ctx);
        (*i) += 4;
        break;
    case 'f':
        // false
        duk_push_false(ctx);
        (*i) += 5;
        break;
    case 'n':
        // null
        duk_push_null(ctx);
        (*i) += 4;
        break;
    }
    martin_parse_json_eat_whitespace(str, i);
}

void martin_parse_json(duk_context *ctx, char *str, int len) {
    int i = 0;
    martin_parse_json_value(ctx, str, &i);
}
