#include "RedisConPool.h"

RedisConPool::RedisConPool(size_t poolSize, const char* host, int port, const char* pwd)
	:_poolSize(poolSize),_host(host),_port(port),b_stop(false)
{
	for (size_t i = 0; i < poolSize; ++i) {
		auto* context = redisConnect(host, port);
		// ������
		if (context == nullptr || context->err != 0) {
			if (context != nullptr) {
				redisFree(context);
			}
			continue;
		}

		// ��¼��֤
		auto reply = (redisReply*)redisCommand(context, "auth %s", pwd);
		if (reply->type == REDIS_REPLY_ERROR) {
			std::cout << "Redis��֤ʧ��" << std::endl;
			// �ͷ�redisCommandִ�к󷵻ص�redisReply��ռ�õ��ڴ�
			freeReplyObject(reply);
			continue;
		}

		// �ͷ�reply
		freeReplyObject(reply);
		std::cout << "��֤�ɹ�" << std::endl;
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
	// �Ӷ����л��һ������
	std::unique_lock<std::mutex> lock(_mutex);
	// �ȴ�conns�п�������/�رճ�
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
	// ����������һ������
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
