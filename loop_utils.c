#include "./dukluv/src/duv.h"
#include "./dukluv/src/misc.h"

#include "duk_module_duktape.h"
#include "duk_console.h"
#include "loop_utils.h"

// todo: remove global loop
static uv_loop_t loop;

static duk_ret_t duv_main(duk_context *ctx)
{
    const char *src = duk_get_string(ctx, -1);
    duk_push_global_object(ctx);
    duk_dup(ctx, -1);
    duk_put_prop_string(ctx, -2, "global");

    duk_push_boolean(ctx, 1);
    duk_put_prop_string(ctx, -2, "dukluv");

    // Load duv module into global uv
    duk_push_c_function(ctx, dukopen_uv, 0);
    duk_call(ctx, 0);
    duk_put_prop_string(ctx, -2, "uv");

    if (duk_peval_string(ctx, src))
    {
        uv_loop_close(&loop);
        return -1;
    }
    uv_run(&loop, UV_RUN_DEFAULT);

    return 0;
}

loop_init_rtn
loop_init()
{
    duk_context *ctx = NULL;
    uv_loop_init(&loop);

    // Tie loop and context together
    ctx = duk_create_heap(NULL, NULL, NULL, &loop, NULL);
    if (!ctx)
    {
        fprintf(stderr, "Problem initiailizing duktape heap\n");
        loop_init_rtn temp = {
            .ctx = ctx,
            .loop = &loop,
        };
        // todo: return an error?
        return temp;
    }
    duk_module_duktape_init(ctx);
    duk_console_init(ctx, 0);
    loop.data = ctx;

    loop_init_rtn temp = {
        .ctx = ctx,
        .loop = &loop,
    };
    return temp;
}

int loop_run(duk_context *ctx, uv_loop_t *theLoop, char *src)
{
    duk_push_c_function(ctx, duv_main, 1);
    duk_push_string(ctx, src);
    if (duk_pcall(ctx, 1))
    {
        uv_loop_close(theLoop);
        return 1;
    }
    return 0;
}

void loop_close(uv_loop_t *loop)
{
    uv_loop_close(loop);
}
