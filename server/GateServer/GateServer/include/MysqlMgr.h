#pragma once

#include "Singleton.h"
#include "MysqlDao.h"

class MysqlMgr: public Singleton<MysqlMgr>
{
	friend class Singleton<MysqlMgr>;
public:
	~MysqlMgr();

	// ע���û�
	int RegUser(const std::string& name, const std::string& email, const std::string& pwd);

	// ��������Ƿ���� 
	bool CheckEmail(const std::string& name, const std::string& email);

	// ��������
	bool UpdatePwd(const std::string& name, const std::string& pwd);

	// ��ѯ����
	bool CheckPwd(const std::string& name, const std::string& pwd, int& userID) {
		return _dao.CheckPwd(name, pwd, userID);
	}

private:
	MysqlMgr();
	MysqlDao _dao;
};

