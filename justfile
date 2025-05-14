list:
    @just --list

build:
    make

run:
    make && ./lang build

watch:
    watchexec -c clear -e .c,.lang,.h 'make && ./lang build | jq'
