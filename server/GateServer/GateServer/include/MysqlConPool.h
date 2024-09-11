#pragma once

#include <string>
#include <mutex>
#include <queue>
#include <mysql/jdbc.h>

class MysqlConPool
{
public:
	MysqlConPool(const std::string& url, const std::string& user, const std::string& pass, const std::string& schema, size_t poolSize)
		:_url(url), _user(user), _pass(pass), _schema(schema),_poolSize(poolSize)
	{
		try {
			for (int i = 0; i < poolSize; ++i) {
				sql::mysql::MySQL_Driver* driver = sql::mysql::get_mysql_driver_instance();
                std::unique_ptr<sql::Connection> con(driver->connect(url, user, pass));
                con->setSchema(schema);
                _pool.emplace(std::move(con));
			}
		}
		catch (sql::SQLException& e) {
			// 处理异常
			std::cout << "MysqlConPool Init Failed!" << std::endl;
            std::cout << user<< "@" << url << std::endl;
            b_stop = true;
		}
	}

    std::unique_ptr<sql::Connection> getConnection() {
        std::unique_lock<std::mutex> lock(_mutex);
        _cond.wait(lock, [this] {
            if (b_stop) {
                return true;
            }
            return !_pool.empty(); });
        if (b_stop) {
            return nullptr;
        }
        std::unique_ptr<sql::Connection> con(std::move(_pool.front()));
        _pool.pop();
        return con;
    }

    void returnConnection(std::unique_ptr<sql::Connection> con) {
        std::unique_lock<std::mutex> lock(_mutex);
        if (b_stop) {
            return;
        }
        _pool.push(std::move(con));
        _cond.notify_one();
    }

    void Close() {
        b_stop = true;
        _cond.notify_all();
    }

    ~MysqlConPool() {
        std::unique_lock<std::mutex> lock(_mutex);
        while (!_pool.empty()) {
            _pool.pop();
        }
    }
private:
	std::atomic<bool> b_stop;
	// mysql设置
	std::string _url;
	std::string _user;
	std::string _pass;
	std::string _schema;
	// 连接池设置
	size_t _poolSize;
	std::queue<std::unique_ptr<sql::Connection>> _pool;
	// 并发访问控制
	std::mutex _mutex;
	std::condition_variable _cond;
};

