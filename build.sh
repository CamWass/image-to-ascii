mkdir -p out
clang -Wall -Wextra -std=c11 \
    src/main.c \
    -o ./out/img-to-ascii