import os

#env = Environment(CC = 'clang')
env = Environment(CC = 'gcc')
env.Append(CFLAGS = '-std=c99 -g -Wall -W -pedantic')
env.Append(CFLAGS = '-Wextra -Wconversion -Wshadow -Wcast-qual -Werror')
env.Append(CFLAGS = '-I/usr/include/gc', LIBS = 'gc')

env.Program('bs', Glob('*.c'))

