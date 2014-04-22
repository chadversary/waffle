// Copyright 2014 Intel Corporation
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

#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <EGL/egl.h>
#include <GLES2/gl2.h>

#include <waffle.h>
#include <waffle_x11_egl.h>
#include <waffle_wayland.h>
#include <waffle_gbm.h>

static const char *usage_message =
    "Usage:\n"
    "    wfl-swapinterval <platform> <num_swaps> [swap_interval]\n"
    "\n"
    "Description:\n"
    "    `platform` must be one of 'gbm', 'wayland', or 'x11_egl'.\n"
    "    wfl-swapinterval will call eglSwapBuffers() `num_swaps` times.\n"
    "    If `swap_interval` is given, then wfl-swapinterval will request that\n"
    "    interval eglSwapInterval().\n"
    ;

static void
print_usage(void) {
    printf("%s", usage_message);
}

static void
error_vprintf(const char *fmt, va_list ap) {
    fprintf(stderr, "error: ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
}

static void
error_printf(const char *fmt, ...) {
    va_list ap;

    va_start(ap, fmt);
    error_vprintf(fmt, ap);
    va_end(ap);
}

static void __attribute__((noreturn))
die(const char *fmt, ...) {
    va_list ap;

    va_start(ap, fmt);
    error_vprintf(fmt, ap);
    va_end(ap);
    exit(EXIT_FAILURE);
}

static void
die_on_waffle_error(void)
{
    const struct waffle_error_info *info = waffle_error_get_info();
    int32_t err = info->code;
    const char *err_str = waffle_error_to_string(info->code);

    if (info->message_length > 0)
        die("%s(0x%x): %s", err_str, err, info->message);
    else
        die("%s(0x%x)", err_str, err);
}

static void __attribute__((noreturn))
usage_error(const char *fmt, ...)
{
    fflush(stdout);
    fprintf(stderr, "usage error");

    if (fmt) {
        va_list ap;
        va_start(ap, fmt);
        fprintf(stderr, ": ");
        vfprintf(stderr, fmt, ap);
        va_end(ap);
    }

    fprintf(stderr, "\n");
    fprintf(stderr, "\n");
    print_usage();
    exit(EXIT_FAILURE);
}

static bool
streq(const char *a, const char *b) {
    return strcmp(a, b) == 0;
}

static void
parse_args(int argc, char **argv,
           enum waffle_enum *platform,
           int *num_swaps,
           int *swap_interval) {

    int i = 0;

    ++i;
    if (argc <= i) {
        usage_error("missing arg 'platform'");
    }

    if (streq(argv[i], "x11_egl")) {
        *platform = WAFFLE_PLATFORM_X11_EGL;
    } else if (streq(argv[i], "wayland")) {
        *platform = WAFFLE_PLATFORM_WAYLAND;
    } else if (streq(argv[i], "gbm")) {
        *platform = WAFFLE_PLATFORM_GBM;
    } else {
        usage_error("bad platform '%s'\n", argv[i]);
    }

    ++i;
    if (argc <= i) {
        usage_error("missing arg 'num_swaps'");
    }

    sscanf(argv[i], "%d", num_swaps);
    if (*num_swaps < 0) {
        usage_error("bad num swaps '%d'", argv[i]);
    }

    ++i;
    if (argc <= i) {
        *swap_interval = -1;
        return;
    }

    sscanf(argv[i], "%d", swap_interval);
    if (*swap_interval < 0) {
        usage_error("bad swap interval '%s'", argv[i]);
    }

    ++i;
    if (argc > i) {
        usage_error("trailing argument '%s'", argv[i]);
    }
}

int
main(int argc, char **argv)
{
    bool ok;

    enum waffle_enum platform = 0;
    int swap_interval = -1;
    int num_swaps = 0;

    parse_args(argc, argv, &platform, &num_swaps, &swap_interval);

    const int window_width = 32;
    const int window_height = 32;

    EGLDisplay egl_dpy = 0;
    EGLConfig egl_config = 0;

    int min_swap_interval = -1;
    int max_swap_interval = -1;

    const enum waffle_enum init_attrs[] = {
        WAFFLE_PLATFORM,                platform,
        WAFFLE_NONE,
    };

    const enum waffle_enum config_attrs[] = {
        WAFFLE_CONTEXT_API,             WAFFLE_CONTEXT_OPENGL_ES2,
        WAFFLE_DOUBLE_BUFFERED,         true,
        WAFFLE_NONE,
    };

    struct waffle_display *dpy;
    struct waffle_config *config;
    struct waffle_context *ctx;
    struct waffle_window *window;

    ok = waffle_init(init_attrs);
    if (!ok)
        die_on_waffle_error();

    dpy = waffle_display_connect(NULL);
    if (!dpy)
        die_on_waffle_error();

    if (!waffle_display_supports_context_api(dpy, WAFFLE_CONTEXT_OPENGL_ES2)) {
        error_printf("Display does not support OpenGL ES 2.0");
    }

    config = waffle_config_choose(dpy, config_attrs);
    if (!config)
        die_on_waffle_error();

    ctx = waffle_context_create(config, NULL);
    if (!ctx)
        die_on_waffle_error();

    window = waffle_window_create(config, window_width, window_height);
    if (!window)
        die_on_waffle_error();

    ok = waffle_make_current(dpy, window, ctx);
    if (!ok)
        die_on_waffle_error();

    switch (platform) {
        case WAFFLE_PLATFORM_X11_EGL:
            egl_dpy = waffle_display_get_native(dpy)->x11_egl->egl_display;
            egl_config = waffle_config_get_native(config)->x11_egl->egl_config;
            break;
        case WAFFLE_PLATFORM_WAYLAND:
            egl_dpy = waffle_display_get_native(dpy)->wayland->egl_display;
            egl_config = waffle_config_get_native(config)->wayland->egl_config;
            break;
        case WAFFLE_PLATFORM_GBM:
            egl_dpy = waffle_display_get_native(dpy)->gbm->egl_display;
            egl_config = waffle_config_get_native(config)->gbm->egl_config;
            break;
        default:
            assert(false);
            break;
    }

    ok = eglGetConfigAttrib(egl_dpy, egl_config, EGL_MIN_SWAP_INTERVAL, &min_swap_interval);
    if (!ok)
        die("eglGetConfigAttrib(EGL_MIN_SWAP_INTERVAL) failed");

    ok = eglGetConfigAttrib(egl_dpy, egl_config, EGL_MAX_SWAP_INTERVAL, &max_swap_interval);
    if (!ok)
        die("eglGetConfigAttrib(EGL_MAX_SWAP_INTERVAL) failed");

    printf("info: EGL_MIN_SWAP_INTERVAL=%d\n", min_swap_interval);
    printf("info: EGL_MIN_SWAP_INTERVAL=%d\n", max_swap_interval);

    if (swap_interval < 0) {
        printf("info: using default swap interval 1\n");
    } else {
        printf("info: requesting swap interval %d\n", swap_interval);
        ok = eglSwapInterval(egl_dpy, swap_interval);
        if (!ok) {
            error_printf("eglSwapInterval(%d) failed", swap_interval);
        }
    }

    ok = waffle_window_show(window);
    if (!ok)
        die_on_waffle_error();

    for (int i = 0; i < num_swaps; ++i) {
        switch (i % 3) {
            case 0: glClearColor(1, 0, 0, 1); break;
            case 1: glClearColor(0, 1, 0, 1); break;
            case 2: glClearColor(0, 0, 1, 1); break;
            case 3: abort(); break;
        }

        glClear(GL_COLOR_BUFFER_BIT);

        ok = waffle_window_swap_buffers(window);
        if (!ok)
            die_on_waffle_error();
    }

    return 0;
}
