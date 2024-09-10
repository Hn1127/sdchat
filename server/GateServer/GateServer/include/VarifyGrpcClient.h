#pragma once

#include <grpcpp/grpcpp.h>
#include <atomic>
#include <queue>
#include "message.grpc.pb.h"
#include "const.h"
#include "Singleton.h"
#include "ConfigMgr.h"
using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

using message::GetVarifyReq;
using message::GetVarifyRsp;
using message::VarifyService;

class RPConPool {
public:
    RPConPool(size_t poolSize, std::string host, std::string port)
        : poolSize_(poolSize), host_(host), port_(port), b_stop_(false) {
        for (size_t i = 0; i < poolSize_; ++i) {

            std::shared_ptr<Channel> channel = grpc::CreateChannel(host + ":" + port,
                grpc::InsecureChannelCredentials());

            connections_.push(VarifyService::NewStub(channel));
        }
    }

    ~RPConPool() {
        std::lock_guard<std::mutex> lock(mutex_);
        Close();
        while (!connections_.empty()) {
            connections_.pop();
        }
    }

    std::unique_ptr<VarifyService::Stub> getConnection() {
        std::unique_lock<std::mutex> lock(mutex_);
        cond_.wait(lock, [this] {
            if (b_stop_) {
                return true;
            }
            return !connections_.empty();
            });
        //如果停止则直接返回空指针
        if (b_stop_) {
            return  nullptr;
        }
        auto context = std::move(connections_.front());
        connections_.pop();
        return context;
    }

    void returnConnection(std::unique_ptr<VarifyService::Stub> context) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (b_stop_) {
            return;
        }
        connections_.push(std::move(context));
        cond_.notify_one();
    }

    void Close() {
        b_stop_ = true;
        cond_.notify_all();
    }

private:
    std::atomic<bool> b_stop_;
    size_t poolSize_;
    std::string host_;
    std::string port_;
    std::queue<std::unique_ptr<VarifyService::Stub>> connections_;
    std::mutex mutex_;
    std::condition_variable cond_;
};

class VarifyGrpcClient : public Singleton<VarifyGrpcClient>
{
    friend class Singleton<VarifyGrpcClient>;

public:
    GetVarifyRsp GetVarifyCode(std::string email)
    {
        ClientContext context;
        GetVarifyRsp reply;
        GetVarifyReq request;
        request.set_email(email);
        auto stub = _pool->getConnection();
        Status status = stub->GetVarifyCode(&context, request, &reply);

        if (!status.ok())
        {
            reply.set_error(ErrorCodes::RPCFailed);
        }
        _pool->returnConnection(std::move(stub));
        return reply;
    }

private:
    VarifyGrpcClient()
    {
        auto gCfgMgr = ConfigMgr::GetInstance();
        std::string host = gCfgMgr->operator[]("VarifyServer")["Host"];
        std::string port = gCfgMgr->operator[]("VarifyServer")["Port"];
        _pool.reset(new RPConPool(5, host, port));
    }
    std::unique_ptr<RPConPool> _pool;
};