// Copyright 2012 Intel Corporation
//
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// - Redistributions of source code must retain the above copyright notice, this
//   list of conditions and the following disclaimer.
//
// - Redistributions in binary form must reproduce the above copyright notice,
//   this list of conditions and the following disclaimer in the documentation
//   and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#include "waffle_enum.h"

#include "wcore_error.h"

#include "wegl_context.h"
#include "wegl_display.h"
#include "wegl_util.h"
#include "wegl_window.h"

void
wegl_emit_error(const char *egl_func_call)
{
    EGLint egl_error_code = eglGetError();
    const char *egl_error_name;

    switch (egl_error_code) {
#define CASE(x) case x: egl_error_name = #x; break
        CASE(EGL_FALSE);
        CASE(EGL_TRUE);
        CASE(EGL_DONT_CARE);
        CASE(EGL_SUCCESS);
        CASE(EGL_NOT_INITIALIZED);
        CASE(EGL_BAD_ACCESS);
        CASE(EGL_BAD_ALLOC);
        CASE(EGL_BAD_ATTRIBUTE);
        CASE(EGL_BAD_CONFIG);
        CASE(EGL_BAD_CONTEXT);
        CASE(EGL_BAD_CURRENT_SURFACE);
        CASE(EGL_BAD_DISPLAY);
        CASE(EGL_BAD_MATCH);
        CASE(EGL_BAD_NATIVE_PIXMAP);
        CASE(EGL_BAD_NATIVE_WINDOW);
        CASE(EGL_BAD_PARAMETER);
        CASE(EGL_BAD_SURFACE);
        CASE(EGL_CONTEXT_LOST);
        default: egl_error_name = ""; break;
#undef CASE
    }

    wcore_errorf(WAFFLE_ERROR_UNKNOWN,
                 "%s failed with error %s(0x%x)",
                 egl_func_call,
                 egl_error_name,
                 egl_error_code);
}

bool
wegl_bind_api(int32_t waffle_context_api)
{
    bool ok = true;

    switch (waffle_context_api) {
        case WAFFLE_CONTEXT_OPENGL:
            ok &= eglBindAPI(EGL_OPENGL_API);
            break;
        case WAFFLE_CONTEXT_OPENGL_ES1:
        case WAFFLE_CONTEXT_OPENGL_ES2:
        case WAFFLE_CONTEXT_OPENGL_ES3:
            ok &= eglBindAPI(EGL_OPENGL_ES_API);
            break;
        default:
            wcore_error_internal("waffle_context_api has bad value #x%x",
                                 waffle_context_api);
            return false;
    }

    if (!ok)
        wegl_emit_error("eglBindAPI");

    return ok;
}

bool
wegl_make_current(struct wcore_platform *wc_plat,
                  struct wcore_display *wc_dpy,
                  struct wcore_window *wc_window,
                  struct wcore_context *wc_ctx)
{
    bool ok;
    struct wegl_context *ctx = wegl_context(wc_ctx);
    EGLSurface surface = wc_window ? wegl_window(wc_window)->egl : NULL;

    if (ctx) {
        ok = wegl_bind_api(ctx->waffle_context_api);
        if (!ok)
            return false;

    }

    ok = eglMakeCurrent(wegl_display(wc_dpy)->egl,
                        surface,
                        surface,
                        ctx ? ctx->egl : NULL);
    if (!ok)
        wegl_emit_error("eglMakeCurrent");

    return ok;
}

void*
wegl_get_proc_address(struct wcore_platform *wc_self, const char *name)
{
    return eglGetProcAddress(name);
}
