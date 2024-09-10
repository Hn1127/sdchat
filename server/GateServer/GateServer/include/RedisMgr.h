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
    // ���Ӳ���
    bool Connect(const std::string& host, int port);
    // ��ȡkey��ֵ��value
    bool Get(const std::string& key, std::string& value);
    // ����key-value
    bool Set(const std::string& key, const std::string& value);
    // ������֤
    bool Auth(const std::string& password);
    // ���push
    bool LPush(const std::string& key, const std::string& value);
    // ���pop
    bool LPop(const std::string& key, std::string& value);
    // �Ҳ�push
    bool RPush(const std::string& key, const std::string& value);
    // �Ҳ�pop
    bool RPop(const std::string& key, std::string& value);
    // ��ϣset
    bool HSet(const std::string& key, const std::string& hkey, const std::string& value);
    // ��ϣget
    std::string HGet(const std::string& key, const std::string& hkey);
    // ��ϣDel
    bool HDel(const std::string& key, const std::string& field);
    // ɾ��key��Ӧ��ֵ
    bool Del(const std::string& key);
    // �ж��Ƿ����ĳkey
    bool ExistsKey(const std::string& key);
    // �ر�����
    void Close();
private:
    RedisMgr();
    std::unique_ptr<RedisConPool> _pool;
};