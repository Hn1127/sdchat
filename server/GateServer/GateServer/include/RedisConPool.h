#pragma once

#include <iostream>
#include <hiredis.h>
#include <atomic>
#include <mutex>
#include <queue>

class RedisConPool
{
public:
	// 初始化连接
	RedisConPool(size_t poolSize, const char* host, int port, const char* pwd);

	// 关闭连接池
	~RedisConPool();

	// 从池中获得一个连接
	redisContext* getConnection();

	// 归还连接
	void returnConnection(redisContext* context);

	// 关闭连接池
	void Close();
private:
	// 停止标志
	std::atomic<bool> b_stop;
	size_t _poolSize;
	// Host:Port 
	const char* _host;
	int _port;
	// 连接队列
	std::queue<redisContext*> _conns;
	// 线程控制
	std::mutex _mutex;
	std::condition_variable _cond;
};

