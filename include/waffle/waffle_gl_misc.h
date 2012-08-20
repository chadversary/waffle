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

/// @defgroup waffle_gl_misc waffle_gl_misc
/// @ingroup waffle_api
/// @{

/// @file

#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "waffle_portability.h"

#ifdef __cplusplus
extern "C" {
#endif

struct waffle_context;
struct waffle_display;
struct waffle_window;

/// @brief Check if an extension is contained in a GL-style extension string.
///
/// The @a extension_string must be in the format returned by
/// `glGetString(GL_EXTENSIONS)`. Note that this function is not restricted to
/// the GL extension string; it can also be used on the GLX and EGL extension
/// strings.
///
/// This function may be called before waffle_init().
WAFFLE_API bool
waffle_is_extension_in_string(
        const char *restrict extension_string,
        const char *restrict extension_name);

/// @brief Analogous to eglMakeCurrent.
WAFFLE_API bool
waffle_make_current(
        struct waffle_display *dpy,
        struct waffle_window *window,
        struct waffle_context *ctx);

/// @brief Analogous to eglGetProcAddress.
WAFFLE_API void*
waffle_get_proc_address(const char *name);

#ifdef __cplusplus
} // end extern "C"
#endif

/// @}
