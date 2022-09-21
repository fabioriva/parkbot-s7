#include<napi.h>

#include <chrono>
#include <thread>

using namespace Napi;

class EchoWorker : public AsyncWorker {
    public:
        EchoWorker(Function& callback, std::string& echo)
        : AsyncWorker(callback), echo(echo) {}

        ~EchoWorker() {}
    // This code will be executed on the worker thread
    void Execute() override {
        // Need to simulate cpu heavy task
        std::this_thread::sleep_for(std::chrono::seconds(3));
    }

    void OnOK() override {
        HandleScope scope(Env());
        Callback().Call({Env().Null(), String::New(Env(), echo)});
    }

    private:
        std::string echo;
};