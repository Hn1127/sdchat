#include "AsioIOServicePool.h"

AsioIOServicePool::AsioIOServicePool(std::size_t size)
	:_poolSize(size),_ioServices(size),_works(size),_nextIOService(0)
{
	// 使得每个io_context非阻塞
	for (int i = 0; i < size; ++i) {
		_works[i] = WorkPtr(new Work(_ioServices[i]));
	}

	// 启动多个线程，每个线程内部分配一个io_service
	for (int i = 0; i < size; ++i) {
		_threads.emplace_back(
			[this, i] 
			{
			_ioServices[i].run();
			});
	}
}

AsioIOServicePool::~AsioIOServicePool()
{
	Stop();
}

boost::asio::io_context& AsioIOServicePool::GetIOService()
{
	auto& service = _ioServices[_nextIOService++];
	// ioService已满
	if (_nextIOService == _ioServices.size()) {
		_nextIOService = 0;
	}
	return service;
}

void AsioIOServicePool::Stop()
{
	// 关闭所有服务和线程
	for (auto& work : _works) {
		work->get_io_context().stop();
		work.reset();
	}
	for (auto& th : _threads) {
		th.join();
	}
}

