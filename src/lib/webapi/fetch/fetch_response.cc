#include <webapi/fetch/fetch_headers.h>
#include <webapi/fetch/fetch_response.h>

namespace webapi {

void FetchResponse::setHeader(const std::string &key, const std::string &value) {
  response_->SetHeader(key.c_str(), value.c_str());
}

void FetchResponse::setHeaders(FetchHeaders *headers) {
  http_headers all_headers = headers->getAll();
  for (auto &header : all_headers) {
    response_->SetHeader(header.first.c_str(), header.second.c_str());
  }
}

}  // namespace webapi