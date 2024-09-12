#pragma once

#include "MysqlConPool.h"
#include "ConfigMgr.h"


class MysqlDao
{
public:
	MysqlDao();

	~MysqlDao();

	// 注册用户
	int RegUser(const std::string& name, const std::string& email, const std::string& pwd);

	// 检查邮箱是否存在 
	bool CheckEmail(const std::string& name, const std::string& email);

	// 更新密码
	bool UpdatePwd(const std::string& name, const std::string& pwd);

	// 检查密码是否正确
	bool CheckPwd(const std::string& name, const std::string& pwd, int& userID);
private:
	std::unique_ptr<MysqlConPool> _pool;
};

