// Copyright 2012 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/// @defgroup glx_config glx_config
/// @ingroup glx
/// @{

/// @file

#pragma once

#include <stdbool.h>
#include <stdint.h>

struct wcore_config_attrs;
union native_config;
union native_display;

union native_config*
glx_config_choose(
        union native_display *dpy,
        const struct wcore_config_attrs *attrs);

bool
glx_config_destroy(union native_config *self);

/// @}