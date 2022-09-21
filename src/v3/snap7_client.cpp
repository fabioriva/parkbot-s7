#include "snap7_client.h"

Napi::Object Snap7Client::Init(Napi::Env env, Napi::Object exports)
{
    Napi::Function func =
        DefineClass(env,
                    "Snap7Client",
                    {InstanceMethod("Connect", &Snap7Client::Connect),
                     InstanceMethod("ConnectTo", &Snap7Client::ConnectTo),
                     InstanceMethod("Disconnect", &Snap7Client::Disconnect),
                     // InstanceMethod("SetConnectionParams", &Snap7Client::SetConnectionParams),
                     // InstanceMethod("SetConnectionType", &Snap7Client::SetConnectionType),
                     InstanceMethod("ReadArea", &Snap7Client::ReadArea),
                     InstanceMethod("WriteArea", &Snap7Client::WriteArea),
                     // InstanceMethod("GetConnected", &Snap7Client::GetConnected),
                     // InstanceMethod("GetLastError", &Snap7Client::GetLastError),
                     InstanceMethod("ErrorText", &Snap7Client::ErrorText),
                     InstanceMethod("ExecTime", &Snap7Client::ExecTime)});

    Napi::FunctionReference *constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(func);

    exports.Set("Snap7Client", func);
    return exports;
}
//---------------------------------------------------------------------------
Snap7Client::Snap7Client(const Napi::CallbackInfo &info)
    : Napi::ObjectWrap<Snap7Client>(info)
{
    Client = Cli_Create();
}
//---------------------------------------------------------------------------
Snap7Client::~Snap7Client()
{
    Cli_Disconnect(Client);
    Cli_Destroy(&Client);
}
//---------------------------------------------------------------------------
Napi::Value Snap7Client::Connect(const Napi::CallbackInfo &info)
{
    int res = Cli_Connect(Client);
    return Napi::Boolean::New(info.Env(), !res);
}
//---------------------------------------------------------------------------
Napi::Value Snap7Client::ConnectTo(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    if (info.Length() != 3 && info.Length() != 4)
    {
        Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[0].IsString() || !info[1].IsNumber() || !info[2].IsNumber())
    {
        Napi::TypeError::New(env, "Wrong arguments").ThrowAsJavaScriptException();
        return env.Null();
    }

    std::string RemAddress = info[0].As<Napi::String>().Utf8Value();
    int Rack = info[1].As<Napi::Number>().Int32Value();
    int Slot = info[2].As<Napi::Number>().Int32Value();
    if (info[3].IsFunction())
    {
        Napi::Function cb = info[3].As<Function>();
        ConnectToCallbackWorker *wk = new ConnectToCallbackWorker(cb, mutex, Client, RemAddress, Rack, Slot);
        wk->Queue();
        return env.Undefined();
    }
    else
    {
        ConnectToPromiseWorker *wk = new ConnectToPromiseWorker(env, mutex, Client, RemAddress, Rack, Slot);
        auto promise = wk->GetPromise();
        wk->Queue();
        return promise;
    }
}
//---------------------------------------------------------------------------
Napi::Value Snap7Client::Disconnect(const Napi::CallbackInfo &info)
{
    int res = Cli_Disconnect(Client);
    return Napi::Boolean::New(info.Env(), !!res);

    // Napi::Env env = info.Env();
    // if (info[0].IsFunction())
    // {
    //     Napi::Function cb = info[0].As<Function>();
    //     DisconnectCallbackWorker *wk = new DisconnectCallbackWorker(cb, mutex, Client);
    //     wk->Queue();
    //     return env.Undefined();
    // }
    // else
    // {
    //     DisconnectPromiseWorker *wk = new DisconnectPromiseWorker(env, mutex, Client);
    //     auto promise = wk->GetPromise();
    //     wk->Queue();
    //     return promise;
    // }
}
//---------------------------------------------------------------------------
Napi::Value Snap7Client::ErrorText(const Napi::CallbackInfo &info)
{
    double arg0 = info[0].As<Napi::Number>().DoubleValue();
    TextString err = CliErrorText(arg0);
    return Napi::String::New(info.Env(), err);
}
//---------------------------------------------------------------------------
Napi::Value Snap7Client::ExecTime(const Napi::CallbackInfo &info)
{
    int Time;
    int Result = Cli_GetExecTime(Client, &Time);
    if (Result == 0)
        return Napi::Number::New(info.Env(), Time);
    else
        return Napi::Number::New(info.Env(), Result);
}
//---------------------------------------------------------------------------
Napi::Value Snap7Client::ReadArea(const Napi::CallbackInfo &info)
{
    ParametersCheck(info, 5);

    Napi::Env env = info.Env();

    int Area = info[0].As<Napi::Number>().Int32Value();
    int DBNumber = info[1].As<Napi::Number>().Int32Value();
    int Start = info[2].As<Napi::Number>().Int32Value();
    int Amount = info[3].As<Napi::Number>().Int32Value();
    int WordLen = info[4].As<Napi::Number>().Int32Value();
    int WordSize = GetWordSize(WordLen);
    if (info[5].IsFunction())
    {
        Napi::Function cb = info[5].As<Function>();
        ReadCallbackWorker *wk = new ReadCallbackWorker(cb, mutex, Client, Area, DBNumber, Start, Amount, WordLen, WordSize);
        wk->Queue();
        return env.Undefined();
    }
    else
    {
        ReadPromiseWorker *wk = new ReadPromiseWorker(env, mutex, Client, Area, DBNumber, Start, Amount, WordLen, WordSize);
        auto promise = wk->GetPromise();
        wk->Queue();
        return promise;
    }
}
//---------------------------------------------------------------------------
Napi::Value Snap7Client::WriteArea(const Napi::CallbackInfo &info)
{
    ParametersCheck(info, 5);

    Napi::Env env = info.Env();

    int Area = info[0].As<Napi::Number>().Int32Value();
    int DBNumber = info[1].As<Napi::Number>().Int32Value();
    int Start = info[2].As<Napi::Number>().Int32Value();
    int Amount = info[3].As<Napi::Number>().Int32Value();
    int WordLen = info[4].As<Napi::Number>().Int32Value();
    Napi::Buffer<uint8_t> Buffer = info[5].As<Napi::Buffer<uint8_t>>();
    uint8_t *pBuffer = reinterpret_cast<uint8_t *>(Buffer.Data());

    if (info[6].IsFunction())
    {
        Napi::Function cb = info[6].As<Function>();
        WriteCallbackWorker *wk = new WriteCallbackWorker(cb, mutex, Client, Area, DBNumber, Start, Amount, WordLen, pBuffer);
        wk->Queue();
        return env.Undefined();
    }
    else
    {
        WritePromiseWorker *wk = new WritePromiseWorker(env, mutex, Client, Area, DBNumber, Start, Amount, WordLen, pBuffer);
        auto promise = wk->GetPromise();
        wk->Queue();
        return promise;
    }
}
//---------------------------------------------------------------------------
int Snap7Client::GetWordSize(int WordLen)
{
    switch (WordLen)
    {
    case S7WLBit:
    case S7WLByte:
        return 1;
    case S7WLWord:
    case S7WLCounter:
    case S7WLTimer:
        return 2;
    case S7WLDWord:
    case S7WLReal:
        return 4;
    default:
        return 0;
    }
}
//---------------------------------------------------------------------------
void Snap7Client::ParametersCheck(const Napi::CallbackInfo &info, size_t length)
{
    Napi::Env env = info.Env();

    if (info.Length() < length)
    {
        Napi::TypeError::New(env, "Usage: Area, DBNumber, Start, Amount, WordLen").ThrowAsJavaScriptException();
        return;
    }
    if (!info[0].IsNumber()) // Area
    {
        Napi::TypeError::New(env, "Area must be of type integer").ThrowAsJavaScriptException();
        return;
    }
    if (!info[1].IsNumber()) // DBNumber
    {
        Napi::TypeError::New(env, "DBNumber must be of type integer").ThrowAsJavaScriptException();
        return;
    }
    if (!info[2].IsNumber()) // Start
    {
        Napi::TypeError::New(env, "Start must be of type integer").ThrowAsJavaScriptException();
        return;
    }
    if (!info[3].IsNumber()) // Amount
    {
        Napi::TypeError::New(env, "Amount must be of type integer").ThrowAsJavaScriptException();
        return;
    }
    if (!info[4].IsNumber()) // WordLen
    {
        Napi::TypeError::New(env, "WordLen must be of type integer").ThrowAsJavaScriptException();
        return;
    }
}
//---------------------------------------------------------------------------