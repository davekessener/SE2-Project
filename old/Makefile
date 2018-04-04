.PHONY: test clean

test:
	$(MAKE) -C test all
	cp test/test.exe build
	cd build && ./test.exe

clean:
	$(MAKE) -C src clean
	$(MAKE) -C test clean
	rm -f build/*.exe

