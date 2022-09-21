#include "snap7_client.h"
#include <iostream>

Napi::Object Snap7Client::Init(Napi::Env env, Napi::Object exports)
{
    Napi::Function func =
        DefineClass(env,
                    "Snap7Client",
                    {InstanceMethod("Connect", &Snap7Client::Connect),
                     InstanceMethod("ConnectTo", &Snap7Client::ConnectTo),
                     InstanceMethod("Echo", &Snap7Client::Echo),
                     InstanceMethod("Disconnect", &Snap7Client::Disconnect),
                     // InstanceMethod("SetConnectionParams", &Snap7Client::SetConnectionParams),
                     // InstanceMethod("SetConnectionType", &Snap7Client::SetConnectionType),
                     // InstanceMethod("ReadAreaSync", &Snap7Client::ReadAreaSync),
                     InstanceMethod("ReadArea", &Snap7Client::ReadArea),
                     // InstanceMethod("WriteAreaSync", &Snap7Client::WriteAreaSync),
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
    // Napi::Env env = info.Env();
    // Napi::HandleScope scope(env);
    // Client = new TS7Client();
    Client = Cli_Create();
}
//---------------------------------------------------------------------------
Snap7Client::~Snap7Client()
{
    // delete Client;
    Cli_Destroy(&Client);
}
//---------------------------------------------------------------------------
Napi::Value Snap7Client::Connect(const Napi::CallbackInfo &info)
{
    int res = Cli_Connect(Client); // Client->Connect();
    return Napi::Number::New(info.Env(), res);
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
    // const char *RemAddress = str.c_str();
    int Rack = info[1].As<Napi::Number>().Int32Value();
    int Slot = info[2].As<Napi::Number>().Int32Value();
    // int res = Cli_ConnectTo(Client, RemAddress, Rack, Slot); // Client->ConnectTo(RemAddress, Rack, Slot);
    // return Napi::Number::New(env, res);
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
    // int res = Cli_Disconnect(Client); // Client->Disconnect();
    // return Napi::Number::New(info.Env(), res);
    Napi::Env env = info.Env();
    if (info[0].IsFunction())
    {
        Napi::Function cb = info[0].As<Function>();
        DisconnectCallbackWorker *wk = new DisconnectCallbackWorker(cb, mutex, Client);
        wk->Queue();
        return env.Undefined();
    }
    else
    {
        DisconnectPromiseWorker *wk = new DisconnectPromiseWorker(env, mutex, Client);
        auto promise = wk->GetPromise();
        wk->Queue();
        return promise;
    }
}
//---------------------------------------------------------------------------
Napi::Value Snap7Client::Echo(const Napi::CallbackInfo &info)
{
    // You need to validate the arguments here.
    Napi::Function cb = info[1].As<Function>();
    std::string in = info[0].As<String>();
    EchoWorker *wk = new EchoWorker(cb, in);
    wk->Queue();
    return info.Env().Undefined();
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
    // int time = Client->ExecTime();
    // return Napi::Number::New(info.Env(), time);
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

    // if (!info[5].IsArrayBuffer())
    // {
    //     Napi::Error::New(info.Env(), "Expected an ArrayBuffer")
    //         .ThrowAsJavaScriptException();
    //     return info.Env().Undefined();
    // }

    // Napi::ArrayBuffer buf = info[5].As<Napi::ArrayBuffer>();
    // uint8_t *p = reinterpret_cast<uint8_t *>(buf.Data());

    // int WordSize = GetWordSize(WordLen);
    // size_t dataLength = Amount * WordSize;
    // std::cout << dataLength << std::endl;
    // uint8_t *pBuffer = new uint8_t[dataLength];
    // dataPtr = info[5].As<Napi::Uint8Array>();
    // int res = Cli_WriteArea(Client, Area, DBNumber, Start, Amount, WordLen, pBuffer);
    // return Napi::Number::New(info.Env(), res);

    // Napi::Uint8Array arr = info[5].As<Napi::Uint8Array>();
    // // size_t length = arr.ElementLength(); // == size
    // uint8_t *Buffer = reinterpret_cast<uint8_t *>(arr.ArrayBuffer().Data());

    Napi::Buffer<uint8_t> Buffer = info[5].As<Napi::Buffer<uint8_t>>();
    // std::cout << Buffer.Data() << std::endl;
    // std::cout << Buffer.Length() << std::endl;
    // uint8_t *pBuffer = (uint8_t *)Buffer.Data();
    uint8_t *pBuffer = reinterpret_cast<uint8_t *>(Buffer.Data());

    // for (int i = 0; i < Buffer.Length(); ++i)
    // {
    //     std::cout << "--> " << std::hex << unsigned(*(pBuffer + i)) << std::endl;
    // }

    int WordSize = GetWordSize(WordLen);

    if (info[6].IsFunction())
    {
        Napi::Function cb = info[6].As<Function>();
        WriteCallbackWorker *wk = new WriteCallbackWorker(cb, mutex, Client, Area, DBNumber, Start, Amount, WordLen, pBuffer);
        wk->Queue();
        return env.Undefined();
    }
    else
    {
        // WritePromiseWorker *wk = new WritePromiseWorker(env, mutex, Client, Area, DBNumber, Start, Amount, WordLen, Buffer);
        WritePromiseWorker *wk = new WritePromiseWorker(env, mutex, Client, Area, DBNumber, Start, Amount, WordLen, WordSize);
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
void Snap7Client::ParametersCheck(const Napi::CallbackInfo &info, int length)
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