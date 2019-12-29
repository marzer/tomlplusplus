all:
	mkdir -p build; \
	cd build; \
	cmake -DCMAKE_INSTALL_PREFIX=/ -DCMAKE_STAGING_PREFIX=/ ..; \
	$(MAKE)

clean:
	rm -rf build

