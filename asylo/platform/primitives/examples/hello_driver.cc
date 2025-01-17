/*
 *
 * Copyright 2019 Asylo authors
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
#include "asylo/platform/primitives/examples/hello_enclave.h"

#include <iostream>
#include <string>

#include "gflags/gflags.h"
#include "asylo/platform/primitives/extent.h"
#include "asylo/platform/primitives/test/test_backend.h"
#include "asylo/platform/primitives/untrusted_primitives.h"
#include "asylo/platform/primitives/util/dispatch_table.h"

namespace asylo {
namespace primitives {

static constexpr char kHello[] = "Hello";

// When the enclave asks for it, send "Hello"
Status hello_handler(std::shared_ptr<Client> client, void *context,
                     NativeParameterStack *params) {
  // Push our message on to the parameter stack to pass to the enclave
  params->PushByCopy(Extent{const_cast<char *>(kHello), strlen(kHello)});
  return Status::OkStatus();
}

Status call_enclave() {
  // Trusted code must exit the enclave to interact with untrusted
  // components like the host operating system. In the Asylo
  // primitives model this is accomplished via "exit handlers," where
  // an exit handler is a callback installed by the client to
  // implement an untrusted service. In this example, the client loads the
  // enclave with an empty table of exit handlers and then adds a trivial
  // callback function "exit_handler" to service the exit type specified by the
  // selector "kExitHandler."
  std::shared_ptr<Client> client =
      test::TestBackend::Get()->LoadTestEnclaveOrDie(
          /*enclave_name=*/"hello_test", absl::make_unique<DispatchTable>());

  auto status = client->exit_call_provider()->RegisterExitHandler(
      kExternalHelloHandler, ExitHandler{hello_handler});

  NativeParameterStack params;
  status = client->EnclaveCall(kHelloEnclaveSelector, &params);
  if (status.ok()) {
    auto span = params.Pop();
    char *hello = reinterpret_cast<char *>(span->data());
    std::cerr << hello << std::endl;
  } else {
    std::cerr << status.ToString() << std::endl;
  }
  return Status::OkStatus();
}

}  // namespace primitives
}  // namespace asylo

int main(int argc, char *argv[]) {
  google::ParseCommandLineFlags(&argc, &argv, true);
  auto status = ::asylo::primitives::call_enclave();
  std::cout << status.ToString() << std::endl;
  return 0;
}
