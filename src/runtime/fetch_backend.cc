#include <hv/hlog.h>
#include <runtime/fetch_backend.h>
#include <runtime/runtime.h>

namespace runtime {

FetchBackend* FetchBackend::getInstance() {
  static FetchBackend* b = new FetchBackend();
  return b;
}

void FetchBackend::Save(int id, FetchContext* context) { getInstance()->save(id, context); }

int FetchBackend::Call(int id) { return getInstance()->call(id); }

int FetchBackend::Termiate(int id) { return getInstance()->terminate(id); }

void FetchBackend::save(int id, FetchContext* context) {
  hlogd("FetchBackend::save, id:%d, context:%p", id, context);
  contexts_[id] = context;
}

int FetchBackend::call(int id) {
  hlogd("FetchBackend::call, id:%d", id);
  auto it = contexts_.find(id);
  if (it == contexts_.end()) {
    hlogw("FetchBackend::call, id:%d, not found", id);
    return -1;
  }
  it->second->do_request();
  return 0;
}

int FetchBackend::terminate(int id) {
  hlogd("FetchBackend::terminate, id:%d", id);
  return 0;
}

}  // namespace runtime