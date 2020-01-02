#if !defined(LOOP_UTILS_H_INCLUDED)
#define LOOP_UTILS_H_INCLUDED

#include "./dukluv/src/duv.h"

#if defined(__cplusplus)
extern "C"
{
#endif

    typedef struct
    {
        duk_context *ctx;
        uv_loop_t *loop;
    } loop_init_rtn;

    typedef struct
    {
        uv_loop_t *loop;
        int64_t *timeout;
    } stuff;

    extern loop_init_rtn loop_init();
    extern int loop_run(duk_context *ctx, uv_loop_t *loop, char *src);
    extern void loop_close(uv_loop_t *loop);

#if defined(__cplusplus)
}
#endif /* end 'extern "C"' wrapper */

#endif /* LOOP_UTILS_H_INCLUDED */
