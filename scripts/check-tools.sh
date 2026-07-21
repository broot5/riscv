#!/bin/sh

missing=0

for tool in "$@"; do
    if ! command -v "$tool" >/dev/null 2>&1; then
        printf "Error: required tool '%s' was not found in PATH.\n" "$tool" >&2
        missing=1
    fi
done

exit "$missing"
