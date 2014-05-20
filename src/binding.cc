#include "uv.h"
#include "node.h"
#include "node_buffer.h"
#include "nan.h"

namespace {

/**
 * dlopen()
 */

NAN_METHOD(Dlopen) {
  NanEscapableScope();

  v8::String::Utf8Value name(args[0]);
  v8::Local<v8::Object> buf = args[1].As<v8::Object>();

  uv_lib_t *lib = reinterpret_cast<uv_lib_t *>(node::Buffer::Data(buf));

  int r = uv_dlopen(*name, lib);

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

  int r = uv_dlsym(lib, *name, &sym);

  NanReturnValue(NanNew<v8::Integer>(r));
}

/**
 * dlerror()
 */

NAN_METHOD(Dlerror) {
  NanEscapableScope();

  v8::Local<v8::Object> buf = args[0].As<v8::Object>();

  uv_lib_t *lib = reinterpret_cast<uv_lib_t *>(node::Buffer::Data(buf));

  NanReturnValue(NanNew<v8::String>(uv_dlerror(lib)));
}

} // anonymous namespace

void init (v8::Handle<v8::Object> target) {
  NanScope();

  target->Set(NanNew<v8::String>("sizeof_uv_lib_t"), NanNew<v8::Integer>(sizeof(uv_lib_t)));
  target->Set(NanNew<v8::String>("sizeof_void_ptr"), NanNew<v8::Integer>(sizeof(void *)));

  NODE_SET_METHOD(target, "dlopen", Dlopen);
  NODE_SET_METHOD(target, "dlclose", Dlclose);
  NODE_SET_METHOD(target, "dlsym", Dlsym);
  NODE_SET_METHOD(target, "dlerror", Dlerror);
}
NODE_MODULE(binding, init);
