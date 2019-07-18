PREFIX:=/usr/local

all: libPainterEngine.a

clean:
	make -C Core clean
	make -C Kernel clean
	make -C Architecture clean
	-rm -f libPainterEngine.a

install:
	mkdir -p $(PREFIX)/include/PainterEngine/Core
	mkdir -p $(PREFIX)/include/PainterEngine/Kernel
	mkdir -p $(PREFIX)/include/PainterEngine/Architecture
	mkdir -p $(PREFIX)/lib
	make -C Core install PREFIX=$(PREFIX)/include/PainterEngine/Core
	make -C Kernel install PREFIX=$(PREFIX)/include/PainterEngine/Kernel
	make -C Architecture install PREFIX=$(PREFIX)/include/PainterEngine/Architecture
	cp libPainterEngine.a $(PREFIX)/lib/libPainterEngine.a

uninstall:
	-rm -rf $(PREFIX)/include/PainterEngine $(PREFIX)/lib/libPainterEngine.a

libPainterEngine.a:
	make -C Core all 
	make -C Kernel all
	make -C Architecture all
	ar r $@ $(wildcard Core/.obj/*.o) $(wildcard Kernel/.obj/*.o) $(wildcard Architecture/.obj/*.o) $(LDFLAG)