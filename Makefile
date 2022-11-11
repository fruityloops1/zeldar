.PHONY: all clean

SERVERIP ?= 0.0.0.0 # put debug logger server IP here

all:
	cmake --toolchain=cmake/toolchain.cmake -DFTP_IP=10.0.0.225 -S . -B build && $(MAKE) -C build subsdk9_meta 

clean:
	rm -r build || true

log: all
	python3.8 scripts/tcpServer.py $(SERVERIP)