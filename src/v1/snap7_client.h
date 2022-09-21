#ifndef SNAP7_CLIENT_H
#define SNAP7_CLIENT_H

#include <napi.h>
#include "snap7.h"
#include "async_worker.h"
#include "echo_worker.h"

class Snap7Client : public Napi::ObjectWrap<Snap7Client>
{
public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    Snap7Client(const Napi::CallbackInfo &info);
    ~Snap7Client();
    Napi::Value Connect(const Napi::CallbackInfo &info);
    Napi::Value ConnectTo(const Napi::CallbackInfo &info);
    Napi::Value Disconnect(const Napi::CallbackInfo &info);
    Napi::Value Echo(const Napi::CallbackInfo &info);
    Napi::Value ErrorText(const Napi::CallbackInfo &info);
    Napi::Value ExecTime(const Napi::CallbackInfo &info);
    Napi::Value ReadArea(const Napi::CallbackInfo &info);
    Napi::Value WriteArea(const Napi::CallbackInfo &info);

private:
    std::mutex mutex;
    S7Object Client;
    // TS7Client *Client;
    int GetWordSize(int WordLen);
    void ParametersCheck(const Napi::CallbackInfo &info, int length);
};

#endif