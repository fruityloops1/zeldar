#include "Logger.hpp"
#include "socket.h"
#include "nifm.h"
#include "util.h"
#include "lib.hpp"

Logger& Logger::instance() { 
    static Logger instance;
    return instance;
}

namespace {

    constexpr inline auto DefaultTcpAutoBufferSizeMax      = 192 * 1024; /* 192kb */
    constexpr inline auto MinTransferMemorySize            = (2 * DefaultTcpAutoBufferSizeMax + (128 * 1024));
    constexpr inline auto MinSocketAllocatorSize           = 128 * 1024;

    constexpr inline auto SocketAllocatorSize = MinSocketAllocatorSize * 1;
    constexpr inline auto TransferMemorySize = MinTransferMemorySize * 1;

    constexpr inline auto SocketPoolSize = SocketAllocatorSize + TransferMemorySize;

};

nn::Result Logger::init(const char *ip, u16 port) {
    in_addr hostAddress = { 0 };
    sockaddr serverAddress = { 0 };

    if (mState != LoggerState::UNINITIALIZED)
        return -1;

    nn::nifm::Initialize();

    // void* socketPool = aligned_alloc(0x4000, SocketPoolSize);

    // nn::socket::Initialize(socketPool, SocketPoolSize, SocketAllocatorSize, 0xE);

    nn::nifm::SubmitNetworkRequest();

    while (nn::nifm::IsNetworkRequestOnHold()) { }

    if (!nn::nifm::IsNetworkAvailable()) {
        mState = LoggerState::UNAVAILABLE;
        return -1;
    }

    if ((mSocketFd = nn::socket::Socket(2, 1, 0)) < 0) {
        mState = LoggerState::UNAVAILABLE;
        return -1;
    }

    nn::socket::InetAton(ip, &hostAddress);

    serverAddress.address = hostAddress;
    serverAddress.port = nn::socket::InetHtons(port);
    serverAddress.family = 2;

    nn::Result result = nn::socket::Connect(mSocketFd, &serverAddress, sizeof(serverAddress));

    mState = result.isSuccess() ? LoggerState::CONNECTED : LoggerState::DISCONNECTED;

    if (mState == LoggerState::CONNECTED) {
        Logger::log("Connected!\n");
    }

    return result;
}

void Logger::log(const char *fmt, ...) {

    if(instance().mState != LoggerState::CONNECTED)
        return;

    va_list args;
    va_start(args, fmt);

    char buffer[0x500] = {};

    if(nn::util::VSNPrintf(buffer, sizeof(buffer), fmt, args) > 0) {
        nn::socket::Send(instance().mSocketFd, buffer, strlen(buffer), 0);
    }

    va_end(args);
}

void Logger::log(const char *fmt, va_list args) {

    if(instance().mState != LoggerState::CONNECTED)
        return;

    char buffer[0x500] = {};

    if(nn::util::VSNPrintf(buffer, sizeof(buffer), fmt, args) > 0) {
        nn::socket::Send(instance().mSocketFd, buffer, strlen(buffer), 0);
    }
}
