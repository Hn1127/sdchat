#pragma once

#include "MysqlConPool.h"
#include "ConfigMgr.h"


class MysqlDao
{
public:
	MysqlDao();

	~MysqlDao();

	// ע���û�
	int RegUser(const std::string& name, const std::string& email, const std::string& pwd);

	// ��������Ƿ���� 
	bool CheckEmail(const std::string& name, const std::string& email);

	// ��������
	bool UpdatePwd(const std::string& name, const std::string& pwd);

	// ��������Ƿ���ȷ
	bool CheckPwd(const std::string& name, const std::string& pwd, int& userID);
private:
	std::unique_ptr<MysqlConPool> _pool;
};

