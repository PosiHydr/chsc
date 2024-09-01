bin/chsc: bin src/chsc.c
		gcc src/chsc.c -Wall -ldl -lpthread -o bin/chsc

bin:
		mkdir bin

src/chsc.c: include/chsc.h arch/scname.c src/chsc_detach.c src/chsc_thread.c

include/chsc.h: arch/arch.h

arch/arch.h: src/gen_arch arch
		bash src/gen_arch arch

arch/scname.c: src/gen_arch arch
		bash src/gen_arch arch

src/chsc_detach.c: include/chsc.h

src/chsc_thread.c: include/chsc.h

.PHONY: test
test: test/hw test/hw2 test/hw4 test/hwx test/libcheuid.so test/libtrace_write.so

test/hw: test/hw.c
		gcc test/hw.c -Wall -o test/hw

test/hw2: test/hw2.c
		gcc test/hw2.c -Wall -o test/hw2

test/hw4: test/hw4.c
		gcc test/hw4.c -Wall -o test/hw4

test/hwx: test/hwx.c
		gcc test/hwx.c -Wall -lpthread -o test/hwx

test/libcheuid.so: test/libcheuid.c
		gcc test/libcheuid.c -Wall -fPIC -shared -o test/libcheuid.so

test/libcheuid.c: test/chsc_dev.h

test/libtrace_write.so: test/libtrace_write.c
		gcc test/libtrace_write.c -Wall -fPIC -shared -o test/libtrace_write.so

test/libtrace_write.c: test/chsc_dev.h

test/chsc_dev.h:

.PHONY: all
all: bin/chsc test

.PHONY: clean
clean:
		rm -rf arch/*.tmp arch/arch.h arch/scname.c bin test/hw test/hw2 test/hw4 test/hwx test/libcheuid.so test/libtrace_write.so
