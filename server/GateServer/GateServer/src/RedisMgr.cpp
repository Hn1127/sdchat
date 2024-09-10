#include "RedisMgr.h"

RedisMgr::~RedisMgr()
{
}

bool RedisMgr::Connect(const std::string& host, int port)
{
	auto _connect = _pool->getConnection();
	if (_connect != nullptr && _connect->err) {
		std::cout << "Redis connect error " << _connect->errstr << std::endl;
		return false;
	}
	return true;
}

bool RedisMgr::Get(const std::string& key, std::string& value)
{
	// 获取连接
	auto connect = _pool->getConnection();
	if (connect == nullptr) {
		// 空连接
		return false;
	}
	// 执行命令
	auto reply = (redisReply*)redisCommand(connect, "GET %s", key.c_str());
	// 命令失败
	if (reply == nullptr) {
		std::cout << "Execut command [ GET  " << key << " ] failed" << std::endl;
		freeReplyObject(reply);
		_pool->returnConnection(connect);
		return false;
	}
	// 命令失败
	if (reply->type != REDIS_REPLY_STRING) {
		std::cout << "Execut command [ GET  " << key << " ] failed" << std::endl;
		freeReplyObject(reply);
		_pool->returnConnection(connect);
		return false;
	}

	value = reply->str;
	// 释放连接
	freeReplyObject(reply);
	_pool->returnConnection(connect);

	std::cout << "Succeed to execute command [ GET " << key << "  ]" << std::endl;
	return true;
}

bool RedisMgr::Set(const std::string& key, const std::string& value)
{
	// 获取连接
	auto connect = _pool->getConnection();
	if (connect == nullptr) {
		// 空连接
		return false;
	}
	// 执行命令
	auto reply = (redisReply*)redisCommand(connect, "SET %s %s", key.c_str(),value.c_str());
	// 命令失败
	if (reply == nullptr) {
		std::cout << "[ SET  " << key << " " << value << " ] failed" << std::endl;
		freeReplyObject(reply);
		_pool->returnConnection(connect);
		return false;
	}
	// 命令失败
	if (!(reply->type == REDIS_REPLY_STATUS && (strcmp(reply->str, "OK") == 0 || strcmp(reply->str, "ok") == 0))) {
		std::cout << "[ SET  " << key << " " << value << " ] failed" << std::endl;
		freeReplyObject(reply);
		_pool->returnConnection(connect);
		return false;
	}

	// 释放连接
	freeReplyObject(reply);
	_pool->returnConnection(connect);

	std::cout << "Succeed to execute command [ SET " << key << " " << value << "  ]" << std::endl;
	return true;
}

bool RedisMgr::Auth(const std::string& password)
{
	return false;
}

bool RedisMgr::LPush(const std::string& key, const std::string& value)
{
	auto connect = _pool->getConnection();
	if (connect == nullptr) {
		return false;
	}
	auto reply = (redisReply*)redisCommand(connect, "LPUSH %s %s", key.c_str(), value.c_str());
	if (NULL == reply)
	{
		std::cout << "Execut command [ LPUSH " << key << "  " << value << " ] failure ! " << std::endl;
		freeReplyObject(reply);
		_pool->returnConnection(connect);
		return false;
	}

	if (reply->type != REDIS_REPLY_INTEGER || reply->integer <= 0) {
		std::cout << "Execut command [ LPUSH " << key << "  " << value << " ] failure ! " << std::endl;
		freeReplyObject(reply);
		_pool->returnConnection(connect);
		return false;
	}

	std::cout << "Execut command [ LPUSH " << key << "  " << value << " ] success ! " << std::endl;
	freeReplyObject(reply);
	_pool->returnConnection(connect);
	return true;
}

bool RedisMgr::LPop(const std::string& key, std::string& value) {
	auto connect = _pool->getConnection();
	if (connect == nullptr) {
		return false;
	}
	auto reply = (redisReply*)redisCommand(connect, "LPOP %s ", key.c_str());
	if (reply == nullptr) {
		std::cout << "Execut command [ LPOP " << key << " ] failure ! " << std::endl;
		_pool->returnConnection(connect);
		return false;
	}

	if (reply->type == REDIS_REPLY_NIL) {
		std::cout << "Execut command [ LPOP " << key << " ] failure ! " << std::endl;
		freeReplyObject(reply);
		_pool->returnConnection(connect);
		return false;
	}

	value = reply->str;
	std::cout << "Execut command [ LPOP " << key << " ] success ! " << std::endl;
	freeReplyObject(reply);
	_pool->returnConnection(connect);
	return true;
}

bool RedisMgr::RPush(const std::string& key, const std::string& value) {
	auto connect = _pool->getConnection();
	if (connect == nullptr) {
		return false;
	}
	auto reply = (redisReply*)redisCommand(connect, "RPUSH %s %s", key.c_str(), value.c_str());
	if (NULL == reply)
	{
		std::cout << "Execut command [ RPUSH " << key << "  " << value << " ] failure ! " << std::endl;
		freeReplyObject(reply);
		_pool->returnConnection(connect);
		return false;
	}

	if (reply->type != REDIS_REPLY_INTEGER || reply->integer <= 0) {
		std::cout << "Execut command [ RPUSH " << key << "  " << value << " ] failure ! " << std::endl;
		freeReplyObject(reply);
		_pool->returnConnection(connect);
		return false;
	}

	std::cout << "Execut command [ RPUSH " << key << "  " << value << " ] success ! " << std::endl;
	freeReplyObject(reply);
	_pool->returnConnection(connect);
	return true;
}
bool RedisMgr::RPop(const std::string& key, std::string& value) {
	auto connect = _pool->getConnection();
	if (connect == nullptr) {
		return false;
	}
	auto reply = (redisReply*)redisCommand(connect, "RPOP %s ", key.c_str());
	if (reply == nullptr) {
		std::cout << "Execut command [ RPOP " << key << " ] failure ! " << std::endl;
		_pool->returnConnection(connect);
		return false;
	}

	if (reply->type == REDIS_REPLY_NIL) {
		std::cout << "Execut command [ RPOP " << key << " ] failure ! " << std::endl;
		freeReplyObject(reply);
		_pool->returnConnection(connect);
		return false;
	}
	value = reply->str;
	std::cout << "Execut command [ RPOP " << key << " ] success ! " << std::endl;
	freeReplyObject(reply);
	_pool->returnConnection(connect);
	return true;
}

bool RedisMgr::HSet(const std::string& key, const std::string& hkey, const std::string& value) {
	auto connect = _pool->getConnection();
	if (connect == nullptr) {
		return false;
	}
	auto reply = (redisReply*)redisCommand(connect, "HSET %s %s %s", key.c_str(), hkey.c_str(), value.c_str());
	if (reply == nullptr) {
		std::cout << "Execut command [ HSet " << key << "  " << hkey << "  " << value << " ] failure ! " << std::endl;
		_pool->returnConnection(connect);
		return false;
	}

	if (reply->type != REDIS_REPLY_INTEGER) {
		std::cout << "Execut command [ HSet " << key << "  " << hkey << "  " << value << " ] failure ! " << std::endl;
		freeReplyObject(reply);
		_pool->returnConnection(connect);
		return false;
	}

	std::cout << "Execut command [ HSet " << key << "  " << hkey << "  " << value << " ] success ! " << std::endl;
	freeReplyObject(reply);
	_pool->returnConnection(connect);
	return true;
}

std::string RedisMgr::HGet(const std::string& key, const std::string& hkey)
{
	auto connect = _pool->getConnection();
	if (connect == nullptr) {
		return "";
	}
	const char* argv[3];
	size_t argvlen[3];
	argv[0] = "HGET";
	argvlen[0] = 4;
	argv[1] = key.c_str();
	argvlen[1] = key.length();
	argv[2] = hkey.c_str();
	argvlen[2] = hkey.length();

	auto reply = (redisReply*)redisCommandArgv(connect, 3, argv, argvlen);
	if (reply == nullptr) {
		std::cout << "Execut command [ HGet " << key << " " << hkey << "  ] failure ! " << std::endl;
		_pool->returnConnection(connect);
		return "";
	}

	if (reply->type == REDIS_REPLY_NIL) {
		freeReplyObject(reply);
		std::cout << "Execut command [ HGet " << key << " " << hkey << "  ] failure ! " << std::endl;
		_pool->returnConnection(connect);
		return "";
	}

	std::string value = reply->str;
	freeReplyObject(reply);
	_pool->returnConnection(connect);
	std::cout << "Execut command [ HGet " << key << " " << hkey << " ] success ! " << std::endl;
	return value;
}

bool RedisMgr::HDel(const std::string& key, const std::string& field)
{
	auto connect = _pool->getConnection();
	if (connect == nullptr) {
		return false;
	}

	redisReply* reply = (redisReply*)redisCommand(connect, "HDEL %s %s", key.c_str(), field.c_str());
	if (reply == nullptr) {
		std::cerr << "HDEL command failed" << std::endl;
		return false;
	}

	bool success = false;
	if (reply->type == REDIS_REPLY_INTEGER) {
		success = reply->integer > 0;
	}

	freeReplyObject(reply);
	return success;
}

bool RedisMgr::Del(const std::string& key)
{
	auto connect = _pool->getConnection();
	if (connect == nullptr) {
		return false;
	}
	auto reply = (redisReply*)redisCommand(connect, "DEL %s", key.c_str());
	if (reply == nullptr) {
		std::cout << "Execut command [ Del " << key << " ] failure ! " << std::endl;
		_pool->returnConnection(connect);
		return false;
	}

	if (reply->type != REDIS_REPLY_INTEGER) {
		std::cout << "Execut command [ Del " << key << " ] failure ! " << std::endl;
		freeReplyObject(reply);
		_pool->returnConnection(connect);
		return false;
	}

	std::cout << "Execut command [ Del " << key << " ] success ! " << std::endl;
	freeReplyObject(reply);
	_pool->returnConnection(connect);
	return true;
}

bool RedisMgr::ExistsKey(const std::string& key)
{
	auto connect = _pool->getConnection();
	if (connect == nullptr) {
		return false;
	}

	auto reply = (redisReply*)redisCommand(connect, "exists %s", key.c_str());
	if (reply == nullptr) {
		std::cout << "Not Found [ Key " << key << " ]  ! " << std::endl;
		_pool->returnConnection(connect);
		return false;
	}

	if (reply->type != REDIS_REPLY_INTEGER || reply->integer == 0) {
		std::cout << "Not Found [ Key " << key << " ]  ! " << std::endl;
		_pool->returnConnection(connect);
		freeReplyObject(reply);
		return false;
	}
	std::cout << " Found [ Key " << key << " ] exists ! " << std::endl;
	freeReplyObject(reply);
	_pool->returnConnection(connect);
	return true;
}


void RedisMgr::Close()
{
	_pool->Close();
}

RedisMgr::RedisMgr()
{
	auto gCfgMgr = ConfigMgr::GetInstance();
	auto host = gCfgMgr->operator[]("Redis")["Host"];
	auto port = gCfgMgr->operator[]("Redis")["Port"];
	auto passwd = gCfgMgr->operator[]("Redis")["Passwd"];
	_pool.reset(new RedisConPool(5, host.c_str(), atoi(port.c_str()), passwd.c_str()));
}
