#ifndef ASYNC_WORKER_H
#define ASYNC_WORKER_H

#include <mutex>

using namespace Napi;
using namespace std;

class ConnectToCallbackWorker : public AsyncWorker
{
public:
    ConnectToCallbackWorker(Function &callback, mutex &Mutex, S7Object Client, string RemAddress, int Rack, int Slot)
        : AsyncWorker(callback),
          Mutex(Mutex),
          Client(Client),
          RemAddress(RemAddress),
          Rack(Rack),
          Slot(Slot) {}

    ~ConnectToCallbackWorker() {}

    void Execute()
    {
        lock_guard<mutex> guard(Mutex);
        const char *pRemAddress = RemAddress.c_str();
        res = Cli_ConnectTo(Client, pRemAddress, Rack, Slot);
        if (res != 0)
        {
            AsyncWorker::SetError(CliErrorText(res));
        }
    }

    void OnError(Error const &error)
    {
        Callback().Call({Number::New(Env(), res)});
    }

    void OnOK()
    {
        Callback().Call({Env().Null(), Boolean::New(Env(), !res)});
    }

private:
    mutex &Mutex;
    S7Object Client;
    string RemAddress;
    int Rack;
    int Slot;
    int res;
};
//---------------------------------------------------------------------------
class ConnectToPromiseWorker : public AsyncWorker
{
public:
    ConnectToPromiseWorker(Napi::Env &env, mutex &Mutex, S7Object Client, string RemAddress, int Rack, int Slot)
        : AsyncWorker(env),
          Mutex(Mutex),
          Client(Client),
          RemAddress(RemAddress),
          Rack(Rack),
          Slot(Slot),
          deferred(Promise::Deferred::New(env)) {}

    ~ConnectToPromiseWorker() {}

    void Execute()
    {
        lock_guard<mutex> guard(Mutex);
        const char *pRemAddress = RemAddress.c_str();
        res = Cli_ConnectTo(Client, pRemAddress, Rack, Slot);
        if (res != 0)
        {
            AsyncWorker::SetError(CliErrorText(res));
        }
    }

    void OnError(Error const &error)
    {
        deferred.Reject(Number::New(Env(), res));
    }

    void OnOK()
    {
        deferred.Resolve(Boolean::New(Env(), !res));
    }

    Promise GetPromise() { return deferred.Promise(); }

private:
    mutex &Mutex;
    S7Object Client;
    string RemAddress;
    int Rack;
    int Slot;
    int res;
    int Time;
    Promise::Deferred deferred;
};
//---------------------------------------------------------------------------
// class DisconnectCallbackWorker : public AsyncWorker
// {
// public:
//     DisconnectCallbackWorker(Function &callback, mutex &Mutex, S7Object Client)
//         : AsyncWorker(callback),
//           Mutex(Mutex),
//           Client(Client) {}

//     ~DisconnectCallbackWorker() {}

//     void Execute()
//     {
//         lock_guard<mutex> guard(Mutex);
//         res = Cli_Disconnect(Client);
//         if (res != 0)
//         {
//             AsyncWorker::SetError(CliErrorText(res));
//         }
//     }

//     void OnError(Error const &error)
//     {
//         Callback().Call({Number::New(Env(), res)});
//     }

//     void OnOK()
//     {
//         Callback().Call({Env().Null(), Boolean::New(Env(), !res)});
//     }

// private:
//     mutex &Mutex;
//     S7Object Client;
//     int res;
// };
//---------------------------------------------------------------------------
// class DisconnectPromiseWorker : public AsyncWorker
// {
// public:
//     DisconnectPromiseWorker(Napi::Env &env, mutex &Mutex, S7Object Client)
//         : AsyncWorker(env),
//           Mutex(Mutex),
//           Client(Client),
//           deferred(Promise::Deferred::New(env)) {}

//     ~DisconnectPromiseWorker() {}

//     void Execute()
//     {
//         lock_guard<mutex> guard(Mutex);
//         res = Cli_Disconnect(Client);
//         if (res != 0)
//         {
//             AsyncWorker::SetError(CliErrorText(res));
//         }
//     }

//     void OnError(Error const &error)
//     {
//         deferred.Reject(Number::New(Env(), res));
//     }

//     void OnOK()
//     {
//         deferred.Resolve(Boolean::New(Env(), !res));
//     }

//     Promise GetPromise() { return deferred.Promise(); }

// private:
//     mutex &Mutex;
//     S7Object Client;
//     int res;
//     Promise::Deferred deferred;
// };
//---------------------------------------------------------------------------
class ReadCallbackWorker : public AsyncWorker
{
public:
    ReadCallbackWorker(Function &callback, mutex &Mutex, S7Object Client, int Area, int DBNumber, int Start, int Amount, int WordLen, int WordSize)
        : AsyncWorker(callback),
          Mutex(Mutex),
          Client(Client),
          Area(Area),
          DBNumber(DBNumber),
          Start(Start),
          Amount(Amount),
          WordLen(WordLen),
          WordSize(WordSize) {}

    ~ReadCallbackWorker() {}

    void Execute()
    {
        lock_guard<mutex> guard(Mutex);
        dataLength = Amount * WordSize;
        dataPtr = new uint8_t[dataLength];
        res = Cli_ReadArea(Client, Area, DBNumber, Start, Amount, WordLen, dataPtr);
        if (res != 0)
        {
            AsyncWorker::SetError(CliErrorText(res));
        }
    }

    void OnError(Error const &error)
    {
        Callback().Call({Number::New(Env(), res)});
    }

    void OnOK()
    {
        Callback().Call({Env().Null(), Buffer<uint8_t>::New(Env(), dataPtr, dataLength)});
    }

private:
    mutex &Mutex;
    S7Object Client;
    int Area;
    int DBNumber;
    int Start;
    int Amount;
    int WordLen;
    int WordSize;
    int res;
    int Time;
    uint8_t *dataPtr;
    size_t dataLength;
};
//---------------------------------------------------------------------------
class ReadPromiseWorker : public AsyncWorker
{
public:
    ReadPromiseWorker(Napi::Env &env, mutex &Mutex, S7Object Client, int Area, int DBNumber, int Start, int Amount, int WordLen, int WordSize)
        : AsyncWorker(env),
          Mutex(Mutex),
          Client(Client),
          Area(Area),
          DBNumber(DBNumber),
          Start(Start),
          Amount(Amount),
          WordLen(WordLen),
          WordSize(WordSize),
          deferred(Promise::Deferred::New(env)) {}

    ~ReadPromiseWorker() {}

    void Execute()
    {
        lock_guard<mutex> guard(Mutex);
        dataLength = Amount * WordSize;
        dataPtr = new uint8_t[dataLength];
        res = Cli_ReadArea(Client, Area, DBNumber, Start, Amount, WordLen, dataPtr);
        if (res != 0)
        {
            AsyncWorker::SetError(CliErrorText(res));
        }
    }

    void OnOK()
    {
        deferred.Resolve(Buffer<uint8_t>::New(Env(), dataPtr, dataLength));
    }

    void OnError(Error const &error)
    {
        deferred.Reject(Number::New(Env(), res));
    }

    Promise GetPromise() { return deferred.Promise(); }

private:
    mutex &Mutex;
    S7Object Client;
    int Area;
    int DBNumber;
    int Start;
    int Amount;
    int WordLen;
    int WordSize;
    int res;
    uint8_t *dataPtr;
    size_t dataLength;
    Promise::Deferred deferred;
};
//---------------------------------------------------------------------------
class WriteCallbackWorker : public AsyncWorker
{
public:
    WriteCallbackWorker(Function &callback, mutex &Mutex, S7Object Client, int Area, int DBNumber, int Start, int Amount, int WordLen, uint8_t *Buffer)
        : AsyncWorker(callback),
          Mutex(Mutex),
          Client(Client),
          Area(Area),
          DBNumber(DBNumber),
          Start(Start),
          Amount(Amount),
          WordLen(WordLen),
          Buffer(Buffer) {}

    ~WriteCallbackWorker() {}

    void Execute()
    {
        lock_guard<mutex> guard(Mutex);
        res = Cli_WriteArea(Client, Area, DBNumber, Start, Amount, WordLen, Buffer);
         if (res != 0)
        {
            AsyncWorker::SetError(CliErrorText(res));
        }
    }

    void OnError(Error const &error)
    {
        Callback().Call({Number::New(Env(), res)});
    }

    void OnOK()
    {
        Callback().Call({Boolean::New(Env(), !res)});
    }

private:
    mutex &Mutex;
    S7Object Client;
    int Area;
    int DBNumber;
    int Start;
    int Amount;
    int WordLen;
    int res;
    uint8_t *Buffer;
};
//---------------------------------------------------------------------------
class WritePromiseWorker : public AsyncWorker
{
public:
    WritePromiseWorker(Napi::Env &env, mutex &Mutex, S7Object Client, int Area, int DBNumber, int Start, int Amount, int WordLen, uint8_t *Buffer)
        : AsyncWorker(env),
          Mutex(Mutex),
          Client(Client),
          Area(Area),
          DBNumber(DBNumber),
          Start(Start),
          Amount(Amount),
          WordLen(WordLen),
          Buffer(Buffer),
          deferred(Promise::Deferred::New(env)) {}

    ~WritePromiseWorker() {}

    void Execute() override
    {
        lock_guard<mutex> guard(Mutex);
        res = Cli_WriteArea(Client, Area, DBNumber, Start, Amount, WordLen, Buffer);
        if (res != 0)
        {
            AsyncWorker::SetError(CliErrorText(res));
        }
    }

    void OnError(Error const &error)
    {
        deferred.Reject(Number::New(Env(), res));
    }

    void OnOK() override
    {
        deferred.Resolve(Boolean::New(Env(), !res));
    }

    Promise GetPromise() { return deferred.Promise(); }

private:
    mutex &Mutex;
    S7Object Client;
    int Area;
    int DBNumber;
    int Start;
    int Amount;
    int WordLen;
    int WordSize;
    int res;
    uint8_t *Buffer;
    Promise::Deferred deferred;
};
//---------------------------------------------------------------------------
#endif