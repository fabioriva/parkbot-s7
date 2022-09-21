#ifndef ASYNC_WORKER_H
#define ASYNC_WORKER_H

#include <iostream>
#include <mutex>
#include <thread>

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
        cout << "<<NAPI>> ConnectTo param: " << RemAddress << " " << Rack << " " << Slot << " " << endl;
        res = Cli_ConnectTo(Client, pRemAddress, Rack, Slot);
        if (res != 0)
        {
            AsyncWorker::SetError(CliErrorText(res));
        }
        Cli_GetExecTime(Client, &Time);
    }

    void OnError(Error const &error)
    {
        cout << "<<NAPI>> ConnectTo onError: " << RemAddress << " " << error.Message() << endl;
        cout << Time << endl;
        Callback().Call({Number::New(Env(), res)});
    }

    void OnOK()
    {
        cout << "<<NAPI>> ConnectTo onOk: " << RemAddress << " " << res << " " << CliErrorText(res) << endl;
        cout << Time << endl;
        Callback().Call({Env().Null(), Number::New(Env(), res)});
    }

private:
    mutex &Mutex;
    S7Object Client;
    string RemAddress;
    int Rack;
    int Slot;
    int res;
    int Time;
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
        deferred.Resolve(Number::New(Env(), res));
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
class DisconnectCallbackWorker : public AsyncWorker
{
public:
    DisconnectCallbackWorker(Function &callback, mutex &Mutex, S7Object Client)
        : AsyncWorker(callback),
          Mutex(Mutex),
          Client(Client) {}

    ~DisconnectCallbackWorker() {}

    void Execute()
    {
        lock_guard<mutex> guard(Mutex);
        cout << "<<NAPI>> Disconnect param: " << endl;
        res = Cli_Disconnect(Client);
        if (res != 0)
        {
            AsyncWorker::SetError(CliErrorText(res));
        }
    }

    void OnError(Error const &error)
    {
        cout << "<<NAPI>> Disconnect onError: " << error.Message() << endl;
        Callback().Call({Number::New(Env(), res)});
    }

    void OnOK()
    {
        cout << "<<NAPI>> Disconnect onOk: " << CliErrorText(res) << endl;
        Callback().Call({Env().Null(), Number::New(Env(), res)});
    }

private:
    mutex &Mutex;
    S7Object Client;
    int res;
};
//---------------------------------------------------------------------------
class DisconnectPromiseWorker : public AsyncWorker
{
public:
    DisconnectPromiseWorker(Napi::Env &env, mutex &Mutex, S7Object Client)
        : AsyncWorker(env),
          Mutex(Mutex),
          Client(Client),
          deferred(Promise::Deferred::New(env)) {}

    ~DisconnectPromiseWorker() {}

    void Execute()
    {
        lock_guard<mutex> guard(Mutex);
        res = Cli_Disconnect(Client);
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
        deferred.Resolve(Number::New(Env(), res));
    }

    Promise GetPromise() { return deferred.Promise(); }

private:
    mutex &Mutex;
    S7Object Client;
    int res;
    Promise::Deferred deferred;
};
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
        Cli_GetExecTime(Client, &Time);
    }

    void OnError(Error const &error)
    {
        cout << "<<NAPI>> ReadArea onError: " << error.Message() << endl;
        Callback().Call({Number::New(Env(), res)});
    }

    void OnOK()
    {
        cout << "<<NAPI>> ReadArea onOk: " << CliErrorText(res) << endl;
        cout << "Exec " << Time << endl;
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
    WriteCallbackWorker(Function &callback, mutex &Mutex, S7Object Client /*TS7Client *Client*/, int Area, int DBNumber, int Start, int Amount, int WordLen, uint8_t *Buffer)
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
        // lock_guard<mutex> guard(Client->mutex);
        lock_guard<mutex> guard(Mutex);
        res = Cli_WriteArea(Client, Area, DBNumber, Start, Amount, WordLen, Buffer);
    }

    void OnOK()
    {
        // HandleScope scope(Env());
        // if (res == 0)
        // {
        //     Callback().Call({Env().Null(), Buffer<uint8_t>::New(Env(), dataPtr, dataLength)});
        // }
        // else
        // {
        Callback().Call({Number::New(Env(), res)});
        // }
    }

private:
    mutex &Mutex;
    // TS7Client *Client;
    S7Object Client;
    int Area;
    int DBNumber;
    int Start;
    int Amount;
    int WordLen;
    // int WordSize;
    int res;
    uint8_t *Buffer;
};
//---------------------------------------------------------------------------
class WritePromiseWorker : public AsyncWorker
{
public:
    WritePromiseWorker(Napi::Env &env, mutex &Mutex, S7Object Client, int Area, int DBNumber, int Start, int Amount, int WordLen, int WordSize)
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

    ~WritePromiseWorker() {}

    void Execute() override
    {
        cout << "(1) " << this_thread::get_id() << endl;
        lock_guard<mutex> guard(Mutex);

        size_t dataLength = Amount * WordSize;
        uint8_t *pBuffer = new uint8_t[dataLength];

        for (int i = 0; i < dataLength; ++i)
        {
            cout << "--> " << hex << unsigned(*(pBuffer + i)) << endl;
        }
        res = Cli_WriteArea(Client, Area, DBNumber, Start, Amount, WordLen, pBuffer);
        if (res != 0)
        {
            AsyncWorker::SetError(CliErrorText(res));
        }
        cout << "(3) " << this_thread::get_id() << endl;
    }

    void OnOK() override
    {
        deferred.Resolve(Number::New(Env(), res));
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
    Napi::Buffer<uint8_t> Buffer;

    Promise::Deferred deferred;
};
//---------------------------------------------------------------------------
class WriteWorker : public AsyncWorker
{
public:
    WriteWorker(Napi::Env &env, mutex &Mutex, S7Object Client, int Area, int DBNumber, int Start, int Amount, int WordLen, Buffer<uint8_t> Buffer)
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

    ~WriteWorker() {}

    void Execute() override
    {
        // unique_lock lock(mutex_);
        // lock_guard<mutex> guard(Client->mutex);
        lock_guard<mutex> guard(Mutex);
        // lock_guard<mutex> guard(this->mutex_);
        // cout << "..........." << endl;
        // Napi::Reference<Napi::Buffer<uint8_t>> bufRef = Napi::Reference<Napi::Buffer<uint8_t>>::New(Buffer, 1);

        // uint8_t *pBuffer = reinterpret_cast<uint8_t *>(Buffer.Data());

        // size_t dataLength = Amount * 1;
        // uint8_t *pUsrData = new uint8_t[dataLength];
        // res = Cli_WriteArea(Client, Area, DBNumber, Start, Amount, WordLen, pUsrData);

        // Napi::Reference<Napi::Buffer<uint8_t>>* pBuffer = new Napi::Reference<Napi::Buffer<uint8_t>>();
        // *pBuffer = Napi::Weak(Buffer);

        cout << "The number of concurrent threads is " << thread::hardware_concurrency() << "\n";

        cout << "(1) " << this_thread::get_id() << endl;
        // lock_guard<mutex> guard(Mutex);

        // Mutex.lock();

        cout << "(2) " << Buffer.Length() << endl;
        uint8_t *pBuffer = reinterpret_cast<uint8_t *>(Buffer.Data());
        for (int i = 0; i < Buffer.Length(); ++i)
        {
            cout << "--> " << hex << unsigned(*(pBuffer + i)) << endl;
        }
        // res = Cli_WriteArea(Client, Area, DBNumber, Start, Amount, WordLen, Buffer.Data());

        // cout << "(2) " << *(pBuffer + 1) << endl;

        res = Cli_WriteArea(Client, Area, DBNumber, Start, Amount, WordLen, pBuffer);

        cout << "(3) " << this_thread::get_id() << endl;
        if (res != 0)
        {
            // delete pBuffer;
            AsyncWorker::SetError("Error");
        }
    }

    void OnOK() override
    {
        deferred.Resolve(Number::New(Env(), res));
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
    // int WordSize;
    int res;
    Napi::Buffer<uint8_t> Buffer;

    Promise::Deferred deferred;
};
//---------------------------------------------------------------------------
#endif