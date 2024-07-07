bin/chsc: bin src/chsc.c
		gcc src/chsc.c -Wall -ldl -lpthread -o bin/chsc

bin:
		mkdir bin

src/chsc.c: include/chsc.h src/scname.c src/chsc_detach.c src/chsc_thread.c

include/chsc.h: include/arch.h include/merr.h

src/scname.c: src/sc_tbl2arr src/syscall.tbl
		bash src/sc_tbl2arr src/syscall.tbl src/scname.c

src/chsc_detach.c: include/chsc.h

src/chsc_thread.c: include/chsc.h

.PHONY: test
test: bin/chsc test/hw test/hw2 test/hw4 test/hwx test/libcheuid.so test/libchsc_test.so

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

test/libchsc_test.so: test/libchsc_test.c
		gcc test/libchsc_test.c -Wall -fPIC -shared -o test/libchsc_test.so

test/libcheuid.c: test/chsc_dev.h

test/libchsc_test.c: test/chsc_dev.h

test/chsc_dev.h: include/merr.h

.PHONY: all
all: bin/chsc test

.PHONY: clean
clean:
		rm -rf bin src/scname.c test/hw test/hw2 test/hw4 test/hwx test/libcheuid.so test/libchsc_test.so
