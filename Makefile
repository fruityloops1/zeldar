.PHONY: all clean

FTP_USERNAME ?= crafty # set these fields to what your ftp server uses on switch (if it has one)
FTP_PASSWORD ?= boss

LOGGER_IP ?= "10.0.0.224" # set this to the IP of the machine that is hosting the tcpServer.py script found in scripts/ (or any other ftp logging script)
FTP_IP ?= 10.0.0.225 # set this to your switches IP address in order to send built mod over to it on compile

all:
	cmake --toolchain=cmake/toolchain.cmake -DFTP_IP=$(FTP_IP) -DFTP_USERNAME=$(FTP_USERNAME) -DFTP_PASSWORD=$(FTP_PASSWORD) -DLOGGER_IP=$(LOGGER_IP) -S . -B build && $(MAKE) -C build subsdk9_meta 

clean:
	rm -r build || true

log: all
	python3.8 scripts/tcpServer.py 0.0.0.0