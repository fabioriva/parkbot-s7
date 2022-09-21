#include <napi.h>
#include "v3/snap7_client.h"

Napi::Object InitAll(Napi::Env env, Napi::Object exports) {
  return Snap7Client::Init(env, exports);
}

NODE_API_MODULE(addon, InitAll)