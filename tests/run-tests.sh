#!/bin/sh

emulator=${1:-}
test_dir=${2:-}

if [ -z "$emulator" ] || [ -z "$test_dir" ]; then
    printf "Usage: %s <emulator> <test-directory>\n" "$0" >&2
    exit 2
fi

failures=0
test_count=0

for test_elf in "$test_dir"/*.elf; do
    test_count=$((test_count + 1))
    test_name=$(basename "$test_elf" .elf)

    if [ "$test_name" = syscall_read ]; then
        "$emulator" "$test_elf" < tests/fixtures/read-input.txt
    else
        "$emulator" "$test_elf"
    fi
    result=$?

    if [ "$result" -eq 0 ]; then
        printf "PASS  %s\n" "$test_name"
    else
        printf "FAIL  %s (assertion/exit code: %d)\n" "$test_name" "$result"
        failures=$((failures + 1))
    fi
done

printf "\n%d tests, %d failures\n" "$test_count" "$failures"
[ "$failures" -eq 0 ]
