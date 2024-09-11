#pragma once

#include "Singleton.h"
#include "MysqlDao.h"

class MysqlMgr: public Singleton<MysqlMgr>
{
	friend class Singleton<MysqlMgr>;
public:
	~MysqlMgr();

	// 注册用户
	int RegUser(const std::string& name, const std::string& email, const std::string& pwd);

	// 检查邮箱是否存在 
	bool CheckEmail(const std::string& name, const std::string& email);

	// 更新密码
	bool UpdatePwd(const std::string& name, const std::string& pwd);

private:
	MysqlMgr();
	MysqlDao _dao;
};

