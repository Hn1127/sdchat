#include "RedisConPool.h"

RedisConPool::RedisConPool(size_t poolSize, const char* host, int port, const char* pwd)
	:_poolSize(poolSize),_host(host),_port(port),b_stop(false)
{
	for (size_t i = 0; i < poolSize; ++i) {
		auto* context = redisConnect(host, port);
		// 错误检查
		if (context == nullptr || context->err != 0) {
			if (context != nullptr) {
				redisFree(context);
			}
			continue;
		}

		// 登录认证
		auto reply = (redisReply*)redisCommand(context, "auth %s", pwd);
		if (reply->type == REDIS_REPLY_ERROR) {
			std::cout << "Redis认证失败" << std::endl;
			// 释放redisCommand执行后返回的redisReply所占用的内存
			freeReplyObject(reply);
			continue;
		}

		// 释放reply
		freeReplyObject(reply);
		std::cout << "认证成功" << std::endl;
		_conns.push(context);
	}
}

RedisConPool::~RedisConPool()
{
	std::lock_guard<std::mutex> lock(_mutex);
	while (!_conns.empty()) {
		_conns.pop();
	}
}

redisContext* RedisConPool::getConnection()
{
	// 从队列中获得一个连接
	std::unique_lock<std::mutex> lock(_mutex);
	// 等待conns有可用连接/关闭池
	_cond.wait(lock,
		[this]
		{
			if (b_stop)
				return true;
			return !_conns.empty();
		});
	if (b_stop)
		return nullptr;
	auto* context = _conns.front();
	_conns.pop();
	return context;
}

void RedisConPool::returnConnection(redisContext* context)
{
	// 向队列中添加一个连接
	std::lock_guard<std::mutex> lock(_mutex);
	if (b_stop)
		return;
	_conns.push(context);
	_cond.notify_one();
}

void RedisConPool::Close()
{
	b_stop = true;
	_cond.notify_all();
}
