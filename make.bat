nasm -f win64 utils.asm -o utils.o
gcc funcs.c -c -nostdlib -ffreestanding
python ./parser_x86.py
gcc main.c aes_home/aes.c aes_home/fast.o utils.o -o main.exe -w -Ofast -s
