#pragma once

#include <iostream>
#include <hiredis.h>
#include <atomic>
#include <mutex>
#include <queue>

class RedisConPool
{
public:
	// ��ʼ������
	RedisConPool(size_t poolSize, const char* host, int port, const char* pwd);

	// �ر����ӳ�
	~RedisConPool();

	// �ӳ��л��һ������
	redisContext* getConnection();

	// �黹����
	void returnConnection(redisContext* context);

	// �ر����ӳ�
	void Close();
private:
	// ֹͣ��־
	std::atomic<bool> b_stop;
	size_t _poolSize;
	// Host:Port 
	const char* _host;
	int _port;
	// ���Ӷ���
	std::queue<redisContext*> _conns;
	// �߳̿���
	std::mutex _mutex;
	std::condition_variable _cond;
};

