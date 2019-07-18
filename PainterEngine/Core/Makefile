all: .obj $(patsubst %.c, .obj/%.o, $(wildcard *.c))

clean:
	-rm -r .obj

install: all
	cp $(wildcard *.h) $(PREFIX)

.obj:
	mkdir -p .obj

.obj/%.o: %.c
	gcc -c -o $@ $<