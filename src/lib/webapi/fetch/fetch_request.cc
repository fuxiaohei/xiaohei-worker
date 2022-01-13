#include <webapi/fetch/fetch_request.h>

namespace webapi {

int FetchRequest::init_once() {
  {
    if (request_ == nullptr) {
      request_ = new HttpRequest();
      is_self_init_ = true;
      return 0;
    }
    return -1;
  }
}

void FetchRequest::set_request(HttpRequest *request) {
  if (request_ != nullptr) {
    delete request_;
    request_ = nullptr;
  }
  request_ = request;
  is_self_init_ = false;
}

std::string FetchRequest::get_method() const { return http_method_str(request_->method); }

FetchRequest::~FetchRequest() {
  if (is_self_init_) {
    delete request_;
  }
}

}  // namespace webapi