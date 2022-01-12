/*
 * Copyright (c) 2022 fuxiaohei. All rights reserved.
 * Licensed under the Apache 2.0 License. See License file in the project root for
 * license information.
 */

#include <webapi/fetch/fetch_event.h>

namespace webapi {

std::string FetchEvent::getType() const { return "fetch"; }

void FetchEvent::cancel() { is_cancelled_ = true; }

bool FetchEvent::isCancelled() const { return is_cancelled_; }

}  // namespace webapi
