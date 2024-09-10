#include "AsioIOServicePool.h"

AsioIOServicePool::AsioIOServicePool(std::size_t size)
	:_poolSize(size),_ioServices(size),_works(size),_nextIOService(0)
{
	// ʹ��ÿ��io_context������
	for (int i = 0; i < size; ++i) {
		_works[i] = WorkPtr(new Work(_ioServices[i]));
	}

	// ��������̣߳�ÿ���߳��ڲ�����һ��io_service
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
	// ioService����
	if (_nextIOService == _ioServices.size()) {
		_nextIOService = 0;
	}
	return service;
}

void AsioIOServicePool::Stop()
{
	// �ر����з�����߳�
	for (auto& work : _works) {
		work->get_io_context().stop();
		work.reset();
	}
	for (auto& th : _threads) {
		th.join();
	}
}

