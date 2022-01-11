/*
 * Copyright (c) 2022 fuxiaohei. All rights reserved.
 * Licensed under the Apache 2.0 License. See License file in the project root for
 * license information.
 */

#include <webapi/fetch_event.h>

namespace webapi {

std::string FetchEvent::getType() const { return "fetch"; }

void FetchEvent::cancel() {}

bool FetchEvent::isCancelled() const { return false; }

}  // namespace webapi
