#ifndef ASYNC_H
#define ASYNC_H

// using namespace Napi;
using namespace std;

class WriteWorker : public Napi::AsyncWorker
{
public:
  WriteWorker(Napi::Env &env, mutex &Mutex, TS7Client *Client, int Area, int DBNumber, int Start, int Amount, int WordLen, uint8_t *Buffer)
      : Napi::AsyncWorker(env),
        Mutex(Mutex),
        Client(Client),
        Area(Area),
        DBNumber(DBNumber),
        Start(Start),
        Amount(Amount),
        WordLen(WordLen),
        Buffer(Buffer),
        deferred(Napi::Promise::Deferred::New(env)) {}

  ~WriteWorker() {}

  void Execute()
  {
    lock_guard<mutex> guard(Mutex);

    res = Client->WriteArea(Area, DBNumber, Start, Amount, WordLen, Buffer);

    if (res != 0)
    {
      Napi::AsyncWorker::SetError("Error");
    }
  }

  void OnOK()
  {
    deferred.Resolve(Napi::Number::New(Env(), res));
  }

  void OnError(Napi::Error const &error)
  {
    deferred.Reject(Napi::Number::New(Env(), res));
  }

  Napi::Promise GetPromise() { return deferred.Promise(); }

private:
  mutex &Mutex;
  TS7Client *Client;
  int Area;
  int DBNumber;
  int Start;
  int Amount;
  int WordLen;
  int res;
  uint8_t *Buffer;
  Napi::Promise::Deferred deferred;
};
#endif