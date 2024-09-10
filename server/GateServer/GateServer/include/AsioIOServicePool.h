#pragma once

#include <vector>
#include <boost/asio.hpp>
#include "Singleton.h"
class AsioIOServicePool :public Singleton<AsioIOServicePool>
{
    friend Singleton<AsioIOServicePool>;
public:
    ~AsioIOServicePool();
    AsioIOServicePool(const AsioIOServicePool&) = delete;
    AsioIOServicePool& operator=(const AsioIOServicePool&) = delete;

    // 使用 round-robin 的方式返回一个 io_service
    boost::asio::io_context& GetIOService();
    void Stop();
public:
    using IOService = boost::asio::io_context;
    using Work = boost::asio::io_context::work;
    using WorkPtr = std::shared_ptr<Work>;
private:
    AsioIOServicePool(std::size_t size = 2);
    size_t _poolSize;
    std::vector<IOService> _ioServices;
    std::vector<WorkPtr> _works;
    std::vector<std::thread> _threads;
    std::size_t _nextIOService;
};