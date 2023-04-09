#include "Logger.hpp"
#include "socket.h"
#include "nifm.h"
#include "util.h"
#include "lib.hpp"

Logger& Logger::instance() { 
    static Logger instance;
    return instance;
}

// namespace originally from https://github.com/randovania/exlaunch/blob/561d2b901af2a61c9169aca829b1cdd421f2d85e/source/program/remote_api.cpp
namespace {
    constexpr inline auto DefaultTcpAutoBufferSizeMax      = 192 * 1024; /* 192kb */
    constexpr inline auto MinTransferMemorySize            = (2 * DefaultTcpAutoBufferSizeMax + (128 * 1024));
    constexpr inline auto MinSocketAllocatorSize           = 128 * 1024;

    constexpr inline auto SocketAllocatorSize = MinSocketAllocatorSize * 1;
    constexpr inline auto TransferMemorySize = MinTransferMemorySize * 1;

    constexpr inline auto SocketPoolSize = SocketAllocatorSize + TransferMemorySize;
};

char socketPool[SocketPoolSize] __attribute__((aligned(0x4000)));

nn::Result Logger::init(const char *ip, u16 port) {
    in_addr hostAddress = { 0 };
    sockaddr serverAddress = { 0 };

    if (mState != LoggerState::UNINITIALIZED)
        return -1;

    nn::nifm::Initialize();

    nn::nifm::SubmitNetworkRequest();

    while (nn::nifm::IsNetworkRequestOnHold()) { }

    if (!nn::nifm::IsNetworkAvailable()) {
        mState = LoggerState::UNAVAILABLE;
        return -1;
    }

    nn::socket::Config config;

    config.pool = socketPool;
    config.allocPoolSize = SocketAllocatorSize;
    config.poolSize = SocketPoolSize;
    config.concurLimit = 0xE;

    nn::socket::Initialize(config);

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

#if !ISEMU
    if(instance().mState != LoggerState::CONNECTED)
        return;
#endif

    va_list args;
    va_start(args, fmt);

    char buffer[0x500] = {};

    if(nn::util::VSNPrintf(buffer, sizeof(buffer), fmt, args) > 0) {
#if !ISEMU
        nn::socket::Send(instance().mSocketFd, buffer, strlen(buffer), 0);
#else
        svcOutputDebugString(buffer, strlen(buffer));
#endif
    }

    va_end(args);
}

void Logger::log(const char *fmt, va_list args) {
#if !ISEMU
    if(instance().mState != LoggerState::CONNECTED)
        return;
#endif

    char buffer[0x500] = {};

    if(nn::util::VSNPrintf(buffer, sizeof(buffer), fmt, args) > 0) {
#if !ISEMU
        nn::socket::Send(instance().mSocketFd, buffer, strlen(buffer), 0);
#else
        svcOutputDebugString(buffer, strlen(buffer));
#endif
    }

}
