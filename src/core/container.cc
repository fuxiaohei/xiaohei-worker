#include <common/common.h>
#include <core/container.h>
#include <core/worker.h>
#include <hv/ThreadLocalStorage.h>
#include <hv/hlog.h>
#include <hv/hthread.h>

namespace xhworker {
namespace core {

#define WORKER_CONTAINER_INDEX 4

Container *Container::FromThreadLocal_() {
  auto *data = hv::ThreadLocalStorage::get(WORKER_CONTAINER_INDEX);
  if (data == nullptr) {
    Container *ct = new Container();
    hv::ThreadLocalStorage::set(WORKER_CONTAINER_INDEX, ct);
    hlogd("manager: setup thread-local, tid=%ld", hv_gettid());
    return ct;
  }
  return static_cast<Container *>(data);
}

Worker *Container::GetWorker(const std::string &key) {
  if (key.empty()) {
    return nullptr;
  }
  Container *ct = FromThreadLocal_();
  if (ct) {
    return ct->find_worker(key);
  }
  return nullptr;
}

Worker *Container::find_worker(const std::string &key) {
  auto it = workers_.find(key);
  if (it == workers_.end()) {
    return install_worker(key);
  }
  hlogd("manager: find cached worker, key:%s, worker:%p", key.data(), it->second);
  return it->second;
}

Worker *Container::install_worker(const std::string &key) {
  Worker *worker = Worker::Create(key);

  if (worker->get_error_code() == common::ERROR_MANIFEST_NOT_FOUND) {
    hlogw("manager: notfound worker, key:%s, worker:%p, errcode:%d", key.data(), worker,
          worker->get_error_code());
    Worker::Destroy(worker);
    return nullptr;
  }

  hlogd("manager: install new worker, key:%s, worker:%p, errcode:%d", key.data(), worker,
        worker->get_error_code());

  // if worker is ok, it can be cached
  if (worker->get_error_code() == 0) {
    workers_[key] = worker;
    hlogd("manager: cache new worker, key:%s, worker:%p", key.data(), worker);
  }

  return worker;
}

}  // namespace core
}  // namespace xhworker