#include <stdio.h>
#include <stdbool.h>
#include "duktape.h"

void martin_parse_json(duk_context *ctx, char *str, int len) {
    int i, strPtr, writePtr;
    bool inString;
    int stackDepth;

    stackDepth = 0;
	for(i=0; i<len; i++){
	    printf("\nstr %d/%d %c %s\n", i, len, str[i], &str[i]);
	    printf("stackDepth %d\n", stackDepth);
        switch(str[i]){
        case 0x09:
        case 0x0A:
        case 0x0D:
        case 0x20:
            // whitespace
            break;
        case '[':
            // array begin
            break;
        case ']':
            // array finish - stack may have pending value on it
            break;
        case '{':
            // object begin
            duk_push_object(ctx);
            stackDepth++;
            break;
        case ':':
            // object - stack has key on it
            break;
        case '}':
            // object finish - stack may have pending key and value on it
            if(stackDepth > 1){
                duk_put_prop(ctx, -3);
                stackDepth -= 2;
            }
            break;
        case ',':
            // object - stack has key and value on it
            // or array - stack has value on it
            duk_put_prop(ctx, -3);
            break;
        case '"':
            // string
            inString = true;
            writePtr = 0;
            for(strPtr=1; inString; strPtr++){
                switch(str[i+strPtr]){
                case '"':
                    duk_push_lstring(ctx, str, writePtr);
                    stackDepth++;
                    inString = false;
                    break;
                case '\\':
                    // escaped char
                    strPtr++;
                    switch(str[i+strPtr]){
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
                    // default case not checked
                    } // end escaped char switch
                    break;
                default:
                    str[writePtr++]=str[i+strPtr];
                    break;
                }
            } // end inString for
            i += strPtr-1;
            break; // end parse string
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
            // number
            break;
        case 't':
            // true
            duk_push_true(ctx);
            i += 4;
            break;
        case 'f':
            // false
            duk_push_false(ctx);
            i += 5;
            break;
        case 'n':
            // null
            duk_push_null(ctx);
            i += 4;
            break;
        }
    }
}
