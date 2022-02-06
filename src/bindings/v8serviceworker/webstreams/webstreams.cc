#include <bindings/v8serviceworker/serviceworker.h>
#include <bindings/v8serviceworker/webstreams/readablestream.h>
#include <bindings/v8serviceworker/webstreams/webstreams.h>
#include <bindings/v8serviceworker/webstreams/readablestream_controller.h>

namespace v8serviceworker {

void register_webstreams_api(v8wrap::IsolateData *isolateData, v8wrap::ClassBuilder *classBuilder) {
  auto rs = create_readablestream_template(isolateData);
  classBuilder->setMethod(CLASS_READABLE_STREAM, rs);

  auto rsDefaultController = create_readablestream_controller_template(isolateData);
  classBuilder->setMethod(CLASS_READABLE_STREAM_DEFAULT_CONTROLLER,rsDefaultController);
}

}  // namespace v8serviceworker
