#include <common/common.h>
#include <common/json.h>
#include <core/main_context.h>
#include <core/worker.h>
#include <hv/hlog.h>

#include <filesystem>

namespace xhworker {
namespace core {

Manifest *Worker::ReadManifest(const std::string &dir, const std::string &key) {
  Manifest *manifest = new Manifest();
  manifest->key = key;

  // read manifest
  std::filesystem::path fpath(dir);
  fpath.append(key);
  fpath.append("manifest.json");
  manifest->manifest_fpath = fpath.string();
  if (!std::filesystem::exists(fpath)) {
    hlogw("workify: manifest not exist, fpath:%s", fpath.c_str());
    manifest->error_code = common::ERROR_MANIFEST_NOT_FOUND;
    return manifest;
  }

  // parse manifest
  nlohmann::json json;
  std::string error_message;
  auto flag = common::json_parse_file(fpath.string(), &(json), &(error_message));
  if (flag != 0) {
    hlogw("workify: manifest parse failed, msg:%s, fpath:%s", error_message.data(), fpath.c_str());
    manifest->error_code = common::ERROR_MANIFEST_ERROR;
    return manifest;
  }

  auto content_type = json["content_type"].get<std::string>();
  auto entry = json["entry"].get<std::string>();

  // read json
  if (content_type == "application/javascript") {
    manifest->type = 1;  // runner::RT_JS_V8;

    // read entry file
    std::filesystem::path entry_path(dir);
    entry_path.append(key);
    entry_path.append(entry);
    manifest->fpath = entry_path.string();

    if (!std::filesystem::exists(entry_path)) {
      hlogd("workify: entry not exist, entry:%s", entry_path.c_str());
      manifest->error_code = common::ERROR_ENTRY_NOT_FOUND;
      return manifest;
    }

    if (common::read_file(entry_path.string(), &(manifest->content)) != 0) {
      hlogw("workify: entry read failed, entry:%s", entry_path.c_str());
      manifest->error_code = common::ERROR_ENTRY_READ_ERROR;
      return manifest;
    }
  }

  return manifest;
}

Worker *Worker::Create(const std::string &key) {
  auto dir = MainContext::Shared(0, nullptr)->get_direcotry();
  Manifest *manifest = ReadManifest(dir, key);
  Worker *worker = new Worker(manifest);
  return worker;
}
void Worker::Destroy(Worker *worker) { worker->release(); }

Worker::Worker(Manifest *manifest) : manifest_(manifest) {
  retain();

  error_code_ = manifest->error_code;
  if (error_code_ != 0) {
    return;
  }
  /*
  error_code_ = prepare_runtime();
  if (error_code_ != 0) {
      error_msg_ = runner_->get_compile_error_message();
  }*/
  hlogd("worker: prepare runtime, worker:%p, errcode:%d, errmsg:%s", this, error_code_,
        error_msg_.c_str());
}

Worker::~Worker() {
  hlogd("worker: destroy, worker:%p", this);
  delete manifest_;
  // delete runner_;
}

void Worker::destroy() { delete this; }

}  // namespace core
}  // namespace xhworker