#include "snap7_client.h"

using namespace Napi;

Napi::Object Snap7Client::Init(Napi::Env env, Napi::Object exports)
{
  Napi::Function func =
      DefineClass(env,
                  "Snap7Client",
                  {
                      InstanceMethod("WriteArea", &Snap7Client::WriteArea),
                  });

  Napi::FunctionReference *constructor = new Napi::FunctionReference();
  *constructor = Napi::Persistent(func);

  exports.Set("Snap7Client", func);
  return exports;
}
//---------------------------------------------------------------------------
Snap7Client::Snap7Client(const Napi::CallbackInfo &info)
    : Napi::ObjectWrap<Snap7Client>(info)
{
  Client = new TS7Client();
}
//---------------------------------------------------------------------------
Snap7Client::~Snap7Client()
{
  delete Client;
}
//---------------------------------------------------------------------------
Napi::Value Snap7Client::WriteArea(const Napi::CallbackInfo &info)
{
  // ParametersCheck(info, 5);

  Napi::Env env = info.Env();

  int Area = info[0].As<Napi::Number>().Int32Value();
  int DBNumber = info[1].As<Napi::Number>().Int32Value();
  int Start = info[2].As<Napi::Number>().Int32Value();
  int Amount = info[3].As<Napi::Number>().Int32Value();
  int WordLen = info[4].As<Napi::Number>().Int32Value();

  Napi::Buffer<uint8_t> Buffer = info[5].As<Napi::Buffer<uint8_t>>();
  uint8_t *pBuffer = reinterpret_cast<uint8_t *>(Buffer.Data());

  // return env.Undefined();

  WriteWorker *wk = new WriteWorker(env, mutex, Client, Area, DBNumber, Start, Amount, WordLen, pBuffer);
  auto promise = wk->GetPromise();
  wk->Queue();
  return promise;
}
//---------------------------------------------------------------------------