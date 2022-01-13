#pragma once

#include <common/heap.h>
#include <hv/HttpServer.h>

#include <string>

namespace webapi {

class FetchRequest : public common::HeapObject {
 public:
  int init_once();
  void set_request(HttpRequest *request);

  std::string get_method() const;

 private:
  friend class common::Heap;
  FetchRequest() = default;
  ~FetchRequest();

 private:
  bool is_self_init_ = false;
  HttpRequest *request_ = nullptr;
};
}  // namespace webapi