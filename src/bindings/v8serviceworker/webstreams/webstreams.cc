#include <bindings/v8serviceworker/serviceworker.h>
#include <bindings/v8serviceworker/webstreams/readablestream.h>
#include <bindings/v8serviceworker/webstreams/webstreams.h>

namespace v8serviceworker {

void register_webstreams_api(v8wrap::IsolateData *isolateData, v8wrap::ClassBuilder *classBuilder) {
  auto rs = create_readablestream_template(isolateData);
  classBuilder->setMethod(CLASS_READABLE_STREAM, rs);
}

}  // namespace v8serviceworker
