/*
 * Copyright (c) 2022 fuxiaohei. All rights reserved.
 * Licensed under the Apache 2.0 License. See License file in the project root for
 * license information.
 */

#include <bindings/v8serviceworker/serviceworker.h>
#include <bindings/v8serviceworker/webstreams/readablestream.h>
#include <bindings/v8serviceworker/webstreams/readablestream_controller.h>
#include <bindings/v8serviceworker/webstreams/strategy.h>
#include <bindings/v8serviceworker/webstreams/webstreams.h>

namespace v8serviceworker {

void register_webstreams_api(v8wrap::IsolateData *isolateData, v8wrap::ClassBuilder *classBuilder) {
  auto rs = create_readablestream_template(isolateData);
  classBuilder->setMethod(CLASS_READABLE_STREAM, rs);

  auto rsDefaultController = create_readablestream_controller_template(isolateData);
  classBuilder->setMethod(CLASS_READABLE_STREAM_DEFAULT_CONTROLLER, rsDefaultController);

  auto countQueuingStrategy = create_countqueuing_strategy_template(isolateData);
  classBuilder->setMethod(CLASS_COUNT_QUEUING_STRATEGY, countQueuingStrategy);
}

}  // namespace v8serviceworker
