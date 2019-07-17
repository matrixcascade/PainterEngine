PREFIX:=/usr/local

all: libPainterEngine.so

clean:
	make -C Core clean
	make -C Kernel clean
	make -C Architecture clean
	-rm -f libPainterEngine.so

install:
	mkdir -p $(PREFIX)/include/PainterEngine/Core
	mkdir -p $(PREFIX)/include/PainterEngine/Kernel
	mkdir -p $(PREFIX)/include/PainterEngine/Architecture
	mkdir -p $(PREFIX)/lib
	make -C Core install PREFIX=$(PREFIX)/include/PainterEngine/Core
	make -C Kernel install PREFIX=$(PREFIX)/include/PainterEngine/Kernel
	make -C Architecture install PREFIX=$(PREFIX)/include/PainterEngine/Architecture
	cp libPainterEngine.so $(PREFIX)/lib/libPainterEngine.so

uninstall:
	-rm -rf $(PREFIX)/include/PainterEngine $(PREFIX)/lib/libPainterEngine.so

libPainterEngine.so:
	make -C Core all 
	make -C Kernel all
	make -C Architecture all
	g++ -shared -o $@ Core/lib.a Kernel/lib.a Architecture/lib.a $(LDFLAG)