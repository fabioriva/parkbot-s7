#ifndef SNAP7_CLIENT_H
#define SNAP7_CLIENT_H

#include <napi.h>
#include "snap7.h"
#include "async.h"

class Snap7Client : public Napi::ObjectWrap<Snap7Client>
{
public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    Snap7Client(const Napi::CallbackInfo &info);
    ~Snap7Client();
    Napi::Value WriteArea(const Napi::CallbackInfo &info);

private:
    std::mutex mutex;
    TS7Client *Client;
    // int GetWordSize(int WordLen);
    // void ParametersCheck(const Napi::CallbackInfo &info, int length);
};

#endif