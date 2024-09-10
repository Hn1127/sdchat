#pragma once

#include <memory>
#include <hiredis.h>
#include "ConfigMgr.h"
#include "Singleton.h"
#include "RedisConPool.h"

class RedisMgr:public Singleton<RedisMgr>,std::enable_shared_from_this<RedisMgr> {
	friend class Singleton<RedisMgr>;
public:
	~RedisMgr();
    // 连接操作
    bool Connect(const std::string& host, int port);
    // 获取key的值到value
    bool Get(const std::string& key, std::string& value);
    // 设置key-value
    bool Set(const std::string& key, const std::string& value);
    // 密码认证
    bool Auth(const std::string& password);
    // 左侧push
    bool LPush(const std::string& key, const std::string& value);
    // 左侧pop
    bool LPop(const std::string& key, std::string& value);
    // 右侧push
    bool RPush(const std::string& key, const std::string& value);
    // 右侧pop
    bool RPop(const std::string& key, std::string& value);
    // 哈希set
    bool HSet(const std::string& key, const std::string& hkey, const std::string& value);
    // 哈希get
    std::string HGet(const std::string& key, const std::string& hkey);
    // 哈希Del
    bool HDel(const std::string& key, const std::string& field);
    // 删除key对应的值
    bool Del(const std::string& key);
    // 判断是否存在某key
    bool ExistsKey(const std::string& key);
    // 关闭连接
    void Close();
private:
    RedisMgr();
    std::unique_ptr<RedisConPool> _pool;
};