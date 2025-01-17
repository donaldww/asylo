/*
 *
 * Copyright 2018 Asylo authors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef ASYLO_PLATFORM_PRIMITIVES_PRIMITIVES_H_
#define ASYLO_PLATFORM_PRIMITIVES_PRIMITIVES_H_

#include <cstddef>
#include <cstdint>

namespace asylo {
namespace primitives {

// This file declares types and constants used by both trusted and untrusted
// code, but which are not passed across the enclave boundary.

// Invalid entry point selector.
static constexpr uint64_t kSelectorAsyloInvalid = 0;

// Enclave initialization entry point selector.
static constexpr uint64_t kSelectorAsyloInit = 1;

// Enclave run entry point selector.
static constexpr uint64_t kSelectorAsyloRun = 2;

// Enclave enter and donate thread entry point selector.
static constexpr uint64_t kSelectorAsyloDonateThread = 3;

// Enclave finalization entry point selector.
static constexpr uint64_t kSelectorAsyloFini = 4;

// Selector values less than `kSelectorUser` are reserved by the runtime and may
// not be registered by the applications.
static constexpr uint64_t kSelectorUser = 128;

// Selector values in [kSelectorHostCall, kSelectorRemote) range are reserved
// for untrusted host call handlers and cannot be used by any other component.
static constexpr uint64_t kSelectorHostCall = 112;

}  // namespace primitives
}  // namespace asylo

#endif  // ASYLO_PLATFORM_PRIMITIVES_PRIMITIVES_H_
