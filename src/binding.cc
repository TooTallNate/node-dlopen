#include "uv.h"
#include "node.h"
#include "node_buffer.h"
#include "node_version.h"
#include "nan.h"

namespace {

/**
 * dlopen()
 */

NAN_METHOD(Dlopen) {
  NanEscapableScope();

  const char *filename;
  if (args[0]->IsNull()) {
    filename = NULL;
  } else if (args[0]->IsString()) {
    v8::String::Utf8Value name(args[0]);
    filename = *name;
  } else {
    return NanThrowTypeError("a string filename, or null must be passed as the first argument");
  }

  v8::Local<v8::Object> buf = args[1].As<v8::Object>();

  uv_lib_t *lib = reinterpret_cast<uv_lib_t *>(node::Buffer::Data(buf));
  int r = 0;

#if NODE_VERSION_AT_LEAST(0, 7, 9)
  r = uv_dlopen(filename, lib);
#else
  uv_err_t err = uv_dlopen(filename, lib);
  if (err.code != UV_OK) {
    r = err.code;
  }
#endif

  NanReturnValue(NanNew<v8::Integer>(r));
}

/**
 * dlclose()
 */

NAN_METHOD(Dlclose) {
  NanEscapableScope();

  v8::Local<v8::Object> buf = args[0].As<v8::Object>();

  uv_lib_t *lib = reinterpret_cast<uv_lib_t *>(node::Buffer::Data(buf));

  uv_dlclose(lib);

  NanReturnUndefined();
}

/**
 * dlsym()
 */

NAN_METHOD(Dlsym) {
  NanEscapableScope();

  v8::Local<v8::Object> buf = args[0].As<v8::Object>();
  v8::String::Utf8Value name(args[1]);
  v8::Local<v8::Object> sym_buf = args[2].As<v8::Object>();

  uv_lib_t *lib = reinterpret_cast<uv_lib_t *>(node::Buffer::Data(buf));
  void *sym = reinterpret_cast<void *>(node::Buffer::Data(sym_buf));

  int r = 0;
#if NODE_VERSION_AT_LEAST(0, 7, 9)
  r = uv_dlsym(lib, *name, &sym);
#else
  uv_err_t err = uv_dlsym(lib, *name, &sym);
  if (err.code != UV_OK) {
    r = err.code;
  }
#endif

  NanReturnValue(NanNew<v8::Integer>(r));
}

/**
 * dlerror()
 */

#if NODE_VERSION_AT_LEAST(0, 7, 9)

NAN_METHOD(Dlerror) {
  NanEscapableScope();

  v8::Local<v8::Object> buf = args[0].As<v8::Object>();

  uv_lib_t *lib = reinterpret_cast<uv_lib_t *>(node::Buffer::Data(buf));

  NanReturnValue(NanNew<v8::String>(uv_dlerror(lib)));
}

#endif

} // anonymous namespace

void init (v8::Handle<v8::Object> target) {
  NanScope();

  target->Set(NanNew<v8::String>("sizeof_uv_lib_t"), NanNew<v8::Integer>(sizeof(uv_lib_t)));
  target->Set(NanNew<v8::String>("sizeof_void_ptr"), NanNew<v8::Integer>(sizeof(void *)));

  NODE_SET_METHOD(target, "dlopen", Dlopen);
  NODE_SET_METHOD(target, "dlclose", Dlclose);
  NODE_SET_METHOD(target, "dlsym", Dlsym);
#if NODE_VERSION_AT_LEAST(0, 7, 9)
  // libuv with node < 0.7.9 didn't have any dlerror() function
  NODE_SET_METHOD(target, "dlerror", Dlerror);
#endif
}
NODE_MODULE(binding, init);
