# count lines of code

find . -name '*.c' -o -name '*.asm' -o -name '*.h' | xargs wc -l | sort -nr
