all: .obj lib.a

clean:
	-rm -r .obj lib.a

install: all
	cp $(wildcard *.h) $(PREFIX)

.obj:
	mkdir -p .obj

lib.a: $(patsubst %.c, .obj/%.o, $(wildcard *.c))
	ar r $@ $^

.obj/%.o: %.c
	gcc -c -o $@ $<