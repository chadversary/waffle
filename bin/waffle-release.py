#!/usr/bin/env python3

# Copyright 2014 Intel Corporation
#
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# - Redistributions of source code must retain the above copyright notice, this
#   list of conditions and the following disclaimer.
#
# - Redistributions in binary form must reproduce the above copyright notice,
#   this list of conditions and the following disclaimer in the documentation
#   and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

import os
import os.path
import shlex
import subprocess
import sys

PROG_NAME = os.path.basename(sys.argv[0])

def _log_tag(tag, message, file=sys.stdout):
    print("{0}: {1}: {2}".format(PROG_NAME, tag, message), file=file)

def usage_error(message):
    _log_tag("usage error", message, file=sys.stderr)
    sys.exit(1)

def loge(message):
    _log_tag("error", message, file=sys.stderr)

def die(message):
    loge(message)
    sys.exit(1)

def check_call(args):
    if isinstance(args, str):
        subprocess.check_call(shlex.split(args))
    else:
        subprocess.check_call(args)

def call(args):
    if isinstance(args, str):
        return subprocess.call(shlex.split(args))
    else:
        return subprocess.call(args)

def cmd_tag():
    branch_name = get_current_branch_name()
    tag_name = get_next_release_tag_name(branch_name)
    check_call(['git', 'tag', '--sign',
        '--message', 'Waffle ' + version_name, tag_name])

def check_tree_is_clean():
    if (call('git diff --no-ext-diff --quiet --exit-code') != 0 or
        call('git diff-index --cached --quiet HEAD --') != 0):
        die('tree contains uncommitted changes')

    if call('git ls-files --others --exclude-standard --error-unmatch-- *') != 0:
        die('tree contains untracked files')

def get_current_branch_name():
    symbolic_ref_target = check_output('git symbolic-ref HEAD')

    if ! symbolic_head_target.startswith('refs/heads/'):
        die('HEAD does not refer to any branch in refs/heads')

    return symbolic_ref_target.lstrip('refs/heads/')

def get_prev_release_tag(branch_name):
    if branch_name == 'master':
        raise NotImplementedError('TODO: implement major releases')
    elif branch_name = 'maint':
            # Get highest release tag among all releases.
            git tag --list |
            grep -E '^v[0-9]+\.[0-9]+\.[0-9]+$' |
            sort |
            tail -1
    elif re.match(r'^maint-[0-9]+\.[0-9]+$', branch_name):
            if ! [[ "$branch_name" =~ ^maint-([0-9]+)\.([0-9]+)$ ]]
            then
                wfl_die "current branch '$branch_name' is ill-formed"
            fi

            major="$match[1]"
            minor="$match[2]"

            # Get the highest release tag in this series of
            # MAJOR.MINOR maintenance releases.
            git tag --list |
            grep -E "^v${major}\.${minor}\.[0-9]+$" |
            sort |
            tail -1
            ;;
        *)
            wfl_die "current branch '$branch_name' is neither 'master' " \
                "nor a maintenance branch"
            ;;
    esac
}

function wfl_fmt_next_version_tuple {
    local branch_name
    local prev_tag
    local prev_version_tuple

    branch_name=$(wfl_print_current_branch)
    prev_tag=$(wfl_print_prev_release_tag)
    prev_version_tuple=("$(wfl_fmt_version_tuple_from_tag "$prev_tag")")

    # Assert version is a 3-tuple.
    [[ "${prev_tag[@]}" = 3 ]]

    local major="$prev_tag[1]"
    local minor="$prev_tag[2]"
    local micro="$prev_tag[3]"

    case "$branch_name" in
        master)
            wfl_die "TODO: implement major releases"
            ;;
        maint|maint-*)
            (( ++micro ))
            ;;
        *)
            false # oops. internal error.
            ;;
    esac

    echo "$major $minor $micro"
}

function wfl_fmt_next_release_version {
    local ver

    ver=("$(wfl_fmt_version_tuple_from_tag)")

    # Assert version is a 3-tuple.
    [[ ${#ver[@]} -eq 3 ]]

    echo "${ver[1]}.${ver[2]}.${ver[3]}"
}

function wfl_fmt_next_release_tag {
    echo "v$(wfl_fmt_next_release_versoin)"
}

function wfl_fmt_version_tuple_from_tag {
    local tag="$1"

    if ! [[ "$tag" =~ ^v([0-9]\+)\.([0-9]\+)\.([0-9]\+)$ ]]
    then
        wfl_die "tag '$tag' is ill-formed"
    fi

    echo "${match[@]}"
}

function main {
    local wfl_git_dir=
    local merge_status=0

    wfl_parse_args "$@"

    wfl_git_dir=$(git rev-parse --git-dir)
    wfl_check_tree_is_clean
    cmd_tag
}

main "$@"
