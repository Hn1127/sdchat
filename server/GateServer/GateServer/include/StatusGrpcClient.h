#pragma once

#include <grpcpp/grpcpp.h>
#include <mutex>
#include <queue>
#include "message.grpc.pb.h"
#include "Singleton.h"
#include "const.h"
#include "ConfigMgr.h"

using grpc::Channel;
using grpc::Status;
using grpc::ClientContext;

using message::GetChatServerReq;
using message::GetChatServerRsp;
using message::StatusService;

class StatusConPool {
public:
	StatusConPool(size_t poolSize, std::string host, std::string port)
		: _poolSize(poolSize), _host(host), _port(port), b_stop(false) {
		for (size_t i = 0; i < _poolSize; ++i) {

			std::shared_ptr<Channel> channel = grpc::CreateChannel(host + ":" + port,
				grpc::InsecureChannelCredentials());

			_conns.push(StatusService::NewStub(channel));
		}
	}

	~StatusConPool() {
		std::lock_guard<std::mutex> lock(_mutex);
		Close();
		while (!_conns.empty()) {
			_conns.pop();
		}
	}

	std::unique_ptr<StatusService::Stub> getConnection() {
		std::unique_lock<std::mutex> lock(_mutex);
		_cond.wait(lock, [this] {
			if (b_stop) {
				return true;
			}
			return !_conns.empty();
			});
		//如果停止则直接返回空指针
		if (b_stop) {
			return  nullptr;
		}
		std::unique_ptr<StatusService::Stub> context = std::move(_conns.front());
		_conns.pop();
		return context;
	}

	void returnConnection(std::unique_ptr<StatusService::Stub> context) {
		std::lock_guard<std::mutex> lock(_mutex);
		if (b_stop) {
			return;
		}
		_conns.push(std::move(context));
		_cond.notify_one();
	}

	void Close() {
		b_stop = true;
		_cond.notify_all();
	}

private:
	std::atomic<bool> b_stop;
	size_t _poolSize;
	std::string _host;
	std::string _port;
	std::queue<std::unique_ptr<StatusService::Stub>> _conns;
	std::mutex _mutex;
	std::condition_variable _cond;
};

class StatusGrpcClient :public Singleton<StatusGrpcClient>
{
	friend class Singleton<StatusGrpcClient>;
public:
	~StatusGrpcClient() {

	}

	GetChatServerRsp GetChatServer(int uid) {
		ClientContext context;
		GetChatServerRsp reply;
		GetChatServerReq request;
		request.set_uid(uid);
		auto stub = _pool->getConnection();
		Status status = stub->GetChatServer(&context, request, &reply);
		if (status.ok()) {
			_pool->returnConnection(std::move(stub));
			return reply;
		}
		else {
			reply.set_error(ErrorCodes::RPCFailed);
			_pool->returnConnection(std::move(stub));
			return reply;
		}
	}

private:
	StatusGrpcClient() {
		auto gCfgMgr = ConfigMgr::GetInstance();
		std::string host = gCfgMgr->operator[]("StatusServer")["Host"];
		std::string port = gCfgMgr->operator[]("StatusServer")["Port"];
		_pool.reset(new StatusConPool(5, host, port));
	}
	std::unique_ptr<StatusConPool> _pool;

};

