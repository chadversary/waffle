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

/// @addtogroup waffle_error
/// @{

/// @file

#include <string.h>
#include <waffle/core/wcore_error.h>

int32_t
waffle_error_get_code(void)
{
    return wcore_error_get_code();
}

const struct waffle_error_info*
waffle_error_get_info(void)
{
    return wcore_error_get_info();
}

const char*
waffle_error_to_string(int32_t e)
{
    switch (e) {
#define CASE(x) case x: return #x
        CASE(WAFFLE_NO_ERROR);
        CASE(WAFFLE_FATAL_ERROR);
        CASE(WAFFLE_UNKNOWN_ERROR);
        CASE(WAFFLE_INTERNAL_ERROR);
        CASE(WAFFLE_OUT_OF_MEMORY);
        CASE(WAFFLE_NOT_INITIALIZED);
        CASE(WAFFLE_ALREADY_INITIALIZED);
        CASE(WAFFLE_BAD_ATTRIBUTE);
        CASE(WAFFLE_BAD_PARAMETER);
        CASE(WAFFLE_UNSUPPORTED_ON_PLATFORM);
        default: return 0;
#undef CASE
    }
}

/// @}
