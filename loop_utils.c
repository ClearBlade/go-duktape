#include "./dukluv/src/duv.h"

#include "duk_module_duktape.h"
#include "duk_console.h"
#include "loop_utils.h"

// todo: remove global loop
static uv_loop_t loop;

static void duv_dump_error(duk_context *ctx, duk_idx_t idx)
{
    fprintf(stderr, "\nUncaught Exception:\n");
    if (duk_is_object(ctx, idx))
    {
        duk_get_prop_string(ctx, -1, "stack");
        fprintf(stderr, "\n%s\n\n", duk_get_string(ctx, -1));
        duk_pop(ctx);
    }
    else
    {
        fprintf(stderr, "\nThrown Value: %s\n\n", duk_json_encode(ctx, idx));
    }
}

loop_init_rtn loop_init()
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
        return temp;
    }
    duk_module_duktape_init(ctx);
    duk_console_init(ctx, 0);
    loop.data = ctx;

    // begin inline duv_main
    duk_push_global_object(ctx);
    duk_dup(ctx, -1);
    duk_put_prop_string(ctx, -2, "global");

    duk_push_boolean(ctx, 1);
    duk_put_prop_string(ctx, -2, "dukluv");

    // Load duv module into global uv
    duk_push_c_function(ctx, dukopen_uv, 0);
    duk_call(ctx, 0);
    duk_put_prop_string(ctx, -2, "uv");
    // end inline duv_main

    loop_init_rtn temp = {
        .ctx = ctx,
        .loop = &loop,
    };
    return temp;
}

void loop_run(uv_loop_t *loop)
{
    uv_run(loop, UV_RUN_DEFAULT);
}

void loop_close(uv_loop_t *loop)
{
    uv_loop_close(loop);
}
