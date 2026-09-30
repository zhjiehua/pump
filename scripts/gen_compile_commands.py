#!/usr/bin/env python3
"""Emit compile_commands.json from qmake Makefiles without rebuilding."""
from __future__ import print_function

import argparse
import json
import os
import re
import sys

SRC_EXTS = {".c", ".cc", ".cpp", ".cxx", ".C"}
VAR_RE = re.compile(r"^([A-Za-z_][A-Za-z0-9_]*)\s*[:+]?=\s*(.*)$")
REF_RE = re.compile(r"\$\(([^)]+)\)|\$\{([^}]+)\}")


def parse_makefile_vars(path):
    text = open(path, "r").read()
    lines = text.splitlines()
    vars_ = {}
    i = 0
    while i < len(lines):
        raw = lines[i]
        if not raw or raw.startswith("#") or raw.startswith("\t"):
            i += 1
            continue
        continued = [raw]
        while continued[-1].rstrip().endswith("\\"):
            i += 1
            if i >= len(lines):
                break
            continued.append(lines[i])
        joined = "".join(part.rstrip()[:-1] + " " if part.rstrip().endswith("\\") else part
                         for part in continued)
        m = VAR_RE.match(joined.strip())
        if m:
            vars_[m.group(1)] = m.group(2).strip()
        i += 1
    return vars_


def expand(value, vars_, depth=0):
    if depth > 20 or not value:
        return value

    def repl(match):
        name = match.group(1) or match.group(2)
        return expand(vars_.get(name, ""), vars_, depth + 1)

    return REF_RE.sub(repl, value)


def tokenize(value):
    return [tok for tok in re.split(r"\s+", value.strip()) if tok]


def compiler_for(src, vars_):
    ext = os.path.splitext(src)[1]
    if ext == ".c":
        return expand(vars_.get("CC", "gcc"), vars_), expand(vars_.get("CFLAGS", ""), vars_)
    return expand(vars_.get("CXX", "g++"), vars_), expand(vars_.get("CXXFLAGS", ""), vars_)


def entries_from_makefile(makefile):
    makefile = os.path.abspath(makefile)
    directory = os.path.dirname(makefile)
    vars_ = parse_makefile_vars(makefile)
    incpath = expand(vars_.get("INCPATH", ""), vars_)
    sources = tokenize(expand(vars_.get("SOURCES", ""), vars_))
    entries = []
    seen = set()
    for src in sources:
        ext = os.path.splitext(src)[1]
        if ext not in SRC_EXTS:
            continue
        abs_src = os.path.normpath(os.path.join(directory, src))
        if abs_src in seen:
            continue
        seen.add(abs_src)
        cc, flags = compiler_for(src, vars_)
        command = " ".join(x for x in (cc, "-c", flags, incpath, src) if x)
        entries.append({
            "directory": directory,
            "command": command,
            "file": abs_src,
        })
    return entries


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("makefiles", nargs="+")
    parser.add_argument("-o", "--output", required=True)
    args = parser.parse_args()

    entries = []
    seen = set()
    for makefile in args.makefiles:
        if not os.path.isfile(makefile):
            print("skip missing Makefile: {}".format(makefile), file=sys.stderr)
            continue
        for entry in entries_from_makefile(makefile):
            key = entry["file"]
            if key in seen:
                continue
            seen.add(key)
            entries.append(entry)

    out_dir = os.path.dirname(os.path.abspath(args.output))
    if out_dir and not os.path.isdir(out_dir):
        os.makedirs(out_dir)
    with open(args.output, "w") as fh:
        json.dump(entries, fh, indent=2)
        fh.write("\n")
    print("wrote {} ({} files)".format(args.output, len(entries)))
    return 0 if entries else 1


if __name__ == "__main__":
    sys.exit(main())
