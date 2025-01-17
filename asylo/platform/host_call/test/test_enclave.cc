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

#include "asylo/platform/host_call/test/enclave_test_selectors.h"
#include "asylo/platform/host_call/trusted/host_call_dispatcher.h"
#include "asylo/platform/host_call/trusted/host_calls.h"
#include "asylo/platform/primitives/primitive_status.h"
#include "asylo/platform/primitives/trusted_primitives.h"
#include "asylo/platform/primitives/trusted_runtime.h"
#include "asylo/platform/system_call/system_call.h"
#include "asylo/platform/system_call/type_conversions/types_functions.h"
#include "asylo/util/status_macros.h"

using asylo::primitives::PrimitiveStatus;

namespace asylo {
namespace host_call {
namespace {

// Message handler that aborts the enclave.
primitives::PrimitiveStatus Abort(void *context,
                                  primitives::TrustedParameterStack *params) {
  ASYLO_RETURN_IF_STACK_EMPTY(params);
  primitives::TrustedPrimitives::BestEffortAbort("Aborting enclave");
  return primitives::PrimitiveStatus::OkStatus();
}

primitives::PrimitiveStatus TestAccess(
    void *context, primitives::TrustedParameterStack *params) {
  ASYLO_RETURN_IF_INCORRECT_ARGUMENTS(params, 2);

  int mode = params->Pop<int>();
  const auto path_name = params->Pop();

  params->PushByCopy<int>(enc_untrusted_access(path_name->As<char>(), mode));
  return primitives::PrimitiveStatus::OkStatus();
}

primitives::PrimitiveStatus TestChmod(
    void *context, primitives::TrustedParameterStack *params) {
  ASYLO_RETURN_IF_INCORRECT_ARGUMENTS(params, 2);

  mode_t mode = params->Pop<mode_t>();
  const auto path_name = params->Pop();

  params->PushByCopy<int>(enc_untrusted_chmod(path_name->As<char>(), mode));
  return primitives::PrimitiveStatus::OkStatus();
}

primitives::PrimitiveStatus TestClose(
    void *context, primitives::TrustedParameterStack *params) {
  ASYLO_RETURN_IF_INCORRECT_ARGUMENTS(params, 1);

  int fd = params->Pop<int>();
  params->PushByCopy<int>(enc_untrusted_close(fd));
  return primitives::PrimitiveStatus::OkStatus();
}

primitives::PrimitiveStatus TestFchmod(
    void *context, primitives::TrustedParameterStack *params) {
  ASYLO_RETURN_IF_INCORRECT_ARGUMENTS(params, 2);

  mode_t mode = params->Pop<mode_t>();
  int fd = params->Pop<int>();

  params->PushByCopy<int>(enc_untrusted_fchmod(fd, mode));
  return primitives::PrimitiveStatus::OkStatus();
}

primitives::PrimitiveStatus TestGetpid(
    void *context, primitives::TrustedParameterStack *params) {
  ASYLO_RETURN_IF_STACK_EMPTY(params);

  params->PushByCopy<pid_t>(enc_untrusted_getpid());
  return primitives::PrimitiveStatus::OkStatus();
}

primitives::PrimitiveStatus TestGetPpid(
    void *context, primitives::TrustedParameterStack *params) {
  ASYLO_RETURN_IF_STACK_EMPTY(params);

  params->PushByCopy<pid_t>(enc_untrusted_getppid());
  return primitives::PrimitiveStatus::OkStatus();
}

primitives::PrimitiveStatus TestSetSid(
    void *context, primitives::TrustedParameterStack *params) {
  ASYLO_RETURN_IF_STACK_EMPTY(params);

  params->PushByCopy<pid_t>(enc_untrusted_setsid());
  return primitives::PrimitiveStatus::OkStatus();
}

primitives::PrimitiveStatus TestKill(
    void *context, primitives::TrustedParameterStack *params) {
  ASYLO_RETURN_IF_INCORRECT_ARGUMENTS(params, 2);

  int sig = params->Pop<int>();
  pid_t pid = params->Pop<pid_t>();

  params->PushByCopy<int>(enc_untrusted_kill(pid, sig));
  return primitives::PrimitiveStatus::OkStatus();
}

primitives::PrimitiveStatus TestLink(
    void *context, primitives::TrustedParameterStack *params) {
  ASYLO_RETURN_IF_INCORRECT_ARGUMENTS(params, 2);

  const auto new_path = params->Pop();
  const auto old_path = params->Pop();

  params->PushByCopy<int>(
      enc_untrusted_link(old_path->As<char>(), new_path->As<char>()));
  return primitives::PrimitiveStatus::OkStatus();
}

primitives::PrimitiveStatus TestLseek(
    void *context, primitives::TrustedParameterStack *params) {
  ASYLO_RETURN_IF_INCORRECT_ARGUMENTS(params, 3);

  int whence = params->Pop<int>();
  off_t offset = params->Pop<off_t>();
  int fd = params->Pop<int>();

  params->PushByCopy<off_t>(enc_untrusted_lseek(fd, offset, whence));
  return primitives::PrimitiveStatus::OkStatus();
}

primitives::PrimitiveStatus TestMkdir(
    void *context, primitives::TrustedParameterStack *params) {
  ASYLO_RETURN_IF_INCORRECT_ARGUMENTS(params, 2);

  mode_t mode = params->Pop<mode_t>();
  const auto pathname = params->Pop();

  params->PushByCopy<int>(enc_untrusted_mkdir(pathname->As<char>(), mode));
  return primitives::PrimitiveStatus::OkStatus();
}

primitives::PrimitiveStatus TestOpen(
    void *context, primitives::TrustedParameterStack *params) {
  // open() can assume 2 or 3 arguments.
  if (params->size() == 3) {
    mode_t mode = params->Pop<mode_t>();
    int flags = params->Pop<int>();
    const auto pathname = params->Pop();
    params->PushByCopy<int>(
        enc_untrusted_open(pathname->As<char>(), flags, mode));
  } else if (params->size() == 2) {
    int flags = params->Pop<int>();
    const auto pathname = params->Pop();
    params->PushByCopy<int>(enc_untrusted_open(pathname->As<char>(), flags));
  } else {
    return {error::GoogleError::INVALID_ARGUMENT,
            "Unexpected number of arguments. open() expects 2 or 3 arguments."};
  }

  return primitives::PrimitiveStatus::OkStatus();
}

primitives::PrimitiveStatus TestUnlink(
    void *context, primitives::TrustedParameterStack *params) {
  ASYLO_RETURN_IF_INCORRECT_ARGUMENTS(params, 1);

  const auto pathname = params->Pop();

  params->PushByCopy<int>(enc_untrusted_unlink(pathname->As<char>()));
  return primitives::PrimitiveStatus::OkStatus();
}

primitives::PrimitiveStatus TestUmask(
    void *context, primitives::TrustedParameterStack *params) {
  ASYLO_RETURN_IF_INCORRECT_ARGUMENTS(params, 1);

  mode_t mask = params->Pop<mode_t>();

  params->PushByCopy<mode_t>(enc_untrusted_umask(mask));
  return primitives::PrimitiveStatus::OkStatus();
}

primitives::PrimitiveStatus TestGetuid(
    void *context, primitives::TrustedParameterStack *params) {
  ASYLO_RETURN_IF_STACK_EMPTY(params);

  params->PushByCopy<uid_t>(enc_untrusted_getuid());
  return primitives::PrimitiveStatus::OkStatus();
}

primitives::PrimitiveStatus TestGetgid(
    void *context, primitives::TrustedParameterStack *params) {
  ASYLO_RETURN_IF_STACK_EMPTY(params);

  params->PushByCopy<gid_t>(enc_untrusted_getgid());
  return primitives::PrimitiveStatus::OkStatus();
}

primitives::PrimitiveStatus TestGeteuid(
    void *context, primitives::TrustedParameterStack *params) {
  ASYLO_RETURN_IF_STACK_EMPTY(params);

  params->PushByCopy<uid_t>(enc_untrusted_geteuid());
  return primitives::PrimitiveStatus::OkStatus();
}

primitives::PrimitiveStatus TestGetegid(
    void *context, primitives::TrustedParameterStack *params) {
  ASYLO_RETURN_IF_STACK_EMPTY(params);

  params->PushByCopy<gid_t>(enc_untrusted_getegid());
  return primitives::PrimitiveStatus::OkStatus();
}

primitives::PrimitiveStatus TestRename(
    void *context, primitives::TrustedParameterStack *params) {
  ASYLO_RETURN_IF_INCORRECT_ARGUMENTS(params, 2);

  const auto newpath = params->Pop();
  const auto oldpath = params->Pop();

  params->PushByCopy<int>(
      enc_untrusted_rename(oldpath->As<char>(), newpath->As<char>()));
  return primitives::PrimitiveStatus::OkStatus();
}

primitives::PrimitiveStatus TestRead(
    void *context, primitives::TrustedParameterStack *params) {
  ASYLO_RETURN_IF_INCORRECT_ARGUMENTS(params, 2);
  size_t count = params->Pop<size_t>();
  int fd = params->Pop<int>();
  char read_buf[20];

  params->PushByCopy<ssize_t>(enc_untrusted_read(fd, read_buf, count));
  params->PushByCopy<char>(read_buf, strlen(read_buf) + 1);
  return primitives::PrimitiveStatus::OkStatus();
}

primitives::PrimitiveStatus TestWrite(
    void *context, primitives::TrustedParameterStack *params) {
  ASYLO_RETURN_IF_INCORRECT_ARGUMENTS(params, 3);
  size_t count = params->Pop<size_t>();
  const auto write_buf = params->Pop();
  int fd = params->Pop<int>();

  params->PushByCopy<ssize_t>(
      enc_untrusted_write(fd, write_buf->As<char>(), count));
  return primitives::PrimitiveStatus::OkStatus();
}

primitives::PrimitiveStatus TestSymlink(
    void *context, primitives::TrustedParameterStack *params) {
  ASYLO_RETURN_IF_INCORRECT_ARGUMENTS(params, 2);
  const auto linkpath = params->Pop();
  const auto target = params->Pop();

  params->PushByCopy<ssize_t>(
      enc_untrusted_symlink(target->As<char>(), linkpath->As<char>()));
  return primitives::PrimitiveStatus::OkStatus();
}

primitives::PrimitiveStatus TestReadlink(
    void *context, primitives::TrustedParameterStack *params) {
  ASYLO_RETURN_IF_INCORRECT_ARGUMENTS(params, 1);
  const auto pathname = params->Pop();

  char buf[PATH_MAX];
  ssize_t len =
      enc_untrusted_readlink(pathname->As<char>(), buf, sizeof(buf) - 1);
  params->PushByCopy<ssize_t>(len);

  buf[len] = '\0';
  params->PushByCopy<char>(buf, strlen(buf) + 1);
  return primitives::PrimitiveStatus::OkStatus();
}

primitives::PrimitiveStatus TestTruncate(
    void *context, primitives::TrustedParameterStack *params) {
  ASYLO_RETURN_IF_INCORRECT_ARGUMENTS(params, 2);
  off_t length = params->Pop<off_t>();
  const auto path = params->Pop();

  params->PushByCopy<int>(enc_untrusted_truncate(path->As<char>(), length));

  return primitives::PrimitiveStatus::OkStatus();
}

primitives::PrimitiveStatus TestFTruncate(
    void *context, primitives::TrustedParameterStack *params) {
  ASYLO_RETURN_IF_INCORRECT_ARGUMENTS(params, 2);
  auto length = params->Pop<off_t>();
  int fd = params->Pop<int>();

  params->PushByCopy<int>(enc_untrusted_ftruncate(fd, length));

  return primitives::PrimitiveStatus::OkStatus();
}

primitives::PrimitiveStatus TestRmdir(
    void *context, primitives::TrustedParameterStack *params) {
  ASYLO_RETURN_IF_INCORRECT_ARGUMENTS(params, 1);
  const auto path = params->Pop();

  params->PushByCopy<int>(enc_untrusted_rmdir(path->As<char>()));
  return primitives::PrimitiveStatus::OkStatus();
}

primitives::PrimitiveStatus TestSocket(
    void *context, primitives::TrustedParameterStack *params) {
  ASYLO_RETURN_IF_INCORRECT_ARGUMENTS(params, 3);

  int protocol = params->Pop<int>();
  int type = params->Pop<int>();
  int domain = params->Pop<int>();
  params->PushByCopy<int>(enc_untrusted_socket(domain, type, protocol));

  return primitives::PrimitiveStatus::OkStatus();
}

primitives::PrimitiveStatus TestFcntl(
    void *context, primitives::TrustedParameterStack *params) {
  ASYLO_RETURN_IF_INCORRECT_ARGUMENTS(params, 3);

  int arg = params->Pop<int>();
  int cmd = params->Pop<int>();
  int fd = params->Pop<int>();
  params->PushByCopy<int>(enc_untrusted_fcntl(fd, cmd, arg));

  return primitives::PrimitiveStatus::OkStatus();
}

primitives::PrimitiveStatus TestChown(
    void *context, primitives::TrustedParameterStack *params) {
  ASYLO_RETURN_IF_INCORRECT_ARGUMENTS(params, 3);

  gid_t group = params->Pop<gid_t>();
  uid_t owner = params->Pop<uid_t>();
  const auto pathname = params->Pop();
  params->PushByCopy<int>(
      enc_untrusted_chown(pathname->As<char>(), owner, group));
  return primitives::PrimitiveStatus::OkStatus();
}

primitives::PrimitiveStatus TestFChown(
    void *context, primitives::TrustedParameterStack *params) {
  ASYLO_RETURN_IF_INCORRECT_ARGUMENTS(params, 3);

  auto group = params->Pop<gid_t>();
  auto owner = params->Pop<uid_t>();
  int fd = params->Pop<int>();
  params->PushByCopy<int>(enc_untrusted_fchown(fd, owner, group));
  return primitives::PrimitiveStatus::OkStatus();
}

primitives::PrimitiveStatus TestSetsockopt(
    void *context, primitives::TrustedParameterStack *params) {
  ASYLO_RETURN_IF_INCORRECT_ARGUMENTS(params, 4);

  int option = params->Pop<int>();
  int klinux_optname = params->Pop<int>();
  int level = params->Pop<int>();
  int sockfd = params->Pop<int>();

  int optname;
  FromkLinuxOptionName(&level, &klinux_optname, &optname);
  params->PushByCopy<int>(enc_untrusted_setsockopt(
      sockfd, level, optname, (void *)&option, sizeof(option)));

  return primitives::PrimitiveStatus::OkStatus();
}

primitives::PrimitiveStatus TestFlock(
    void *context, primitives::TrustedParameterStack *params) {
  ASYLO_RETURN_IF_INCORRECT_ARGUMENTS(params, 2);

  int operation =
      params->Pop<int>();  // The operation is expected to be
                           // already converted from a kLinux_ operation.
  int fd = params->Pop<int>();
  params->PushByCopy<int>(enc_untrusted_flock(fd, operation));

  return primitives::PrimitiveStatus::OkStatus();
}

primitives::PrimitiveStatus TestFsync(
    void *context, primitives::TrustedParameterStack *params) {
  ASYLO_RETURN_IF_INCORRECT_ARGUMENTS(params, 1);

  int fd = params->Pop<int>();
  params->PushByCopy<int>(enc_untrusted_fsync(fd));

  return primitives::PrimitiveStatus::OkStatus();
}

primitives::PrimitiveStatus TestInotifyInit1(
    void *context, primitives::TrustedParameterStack *params) {
  ASYLO_RETURN_IF_INCORRECT_ARGUMENTS(params, 1);

  int flags = params->Pop<int>();  // The operation is expected to be already
                                   // converted from a kLinux_ operation.
  params->PushByCopy<int>(enc_untrusted_inotify_init1(flags));
  return primitives::PrimitiveStatus::OkStatus();
}

primitives::PrimitiveStatus TestInotifyAddWatch(
    void *context, primitives::TrustedParameterStack *params) {
  ASYLO_RETURN_IF_INCORRECT_ARGUMENTS(params, 3);

  uint32_t mask =
      params->Pop<uint32_t>();  // The operation is expected to be already
                                // converted from a kLinux_ operation.
  const auto pathname = params->Pop();
  int fd = params->Pop<int>();
  params->PushByCopy<int>(
      enc_untrusted_inotify_add_watch(fd, pathname->As<char>(), mask));

  return primitives::PrimitiveStatus::OkStatus();
}

primitives::PrimitiveStatus TestInotifyRmWatch(
    void *context, primitives::TrustedParameterStack *params) {
  ASYLO_RETURN_IF_INCORRECT_ARGUMENTS(params, 2);

  int wd = params->Pop<int>();
  int fd = params->Pop<int>();
  params->PushByCopy<int>(enc_untrusted_inotify_rm_watch(fd, wd));
  return primitives::PrimitiveStatus::OkStatus();
}

primitives::PrimitiveStatus TestSchedYield(
    void *context, primitives::TrustedParameterStack *params) {
  ASYLO_RETURN_IF_STACK_EMPTY(params);

  params->PushByCopy<int>(enc_untrusted_sched_yield());
  return primitives::PrimitiveStatus::OkStatus();
}

primitives::PrimitiveStatus TestIsAtty(
    void *context, primitives::TrustedParameterStack *params) {
  ASYLO_RETURN_IF_INCORRECT_ARGUMENTS(params, 1);

  int fd = params->Pop<int>();
  params->PushByCopy<int>(enc_untrusted_isatty(fd));
  return primitives::PrimitiveStatus::OkStatus();
}

primitives::PrimitiveStatus TestUSleep(
    void *context, primitives::TrustedParameterStack *params) {
  ASYLO_RETURN_IF_INCORRECT_ARGUMENTS(params, 1);
  auto usec = params->Pop<unsigned int>();
  params->PushByCopy<int>(enc_untrusted_usleep(usec));
  return primitives::PrimitiveStatus::OkStatus();
}

}  // namespace

// Implements the required enclave initialization function.
extern "C" primitives::PrimitiveStatus asylo_enclave_init() {
  // Register the host call dispatcher.
  enc_set_dispatch_syscall(SystemCallDispatcher);

  ASYLO_RETURN_IF_ERROR(primitives::TrustedPrimitives::RegisterEntryHandler(
      kAbortEnclaveSelector, primitives::EntryHandler{Abort}));
  ASYLO_RETURN_IF_ERROR(primitives::TrustedPrimitives::RegisterEntryHandler(
      kTestAccess, primitives::EntryHandler{TestAccess}));
  ASYLO_RETURN_IF_ERROR(primitives::TrustedPrimitives::RegisterEntryHandler(
      kTestChmod, primitives::EntryHandler{TestChmod}));
  ASYLO_RETURN_IF_ERROR(primitives::TrustedPrimitives::RegisterEntryHandler(
      kTestClose, primitives::EntryHandler{TestClose}));
  ASYLO_RETURN_IF_ERROR(primitives::TrustedPrimitives::RegisterEntryHandler(
      kTestFchmod, primitives::EntryHandler{TestFchmod}));
  ASYLO_RETURN_IF_ERROR(primitives::TrustedPrimitives::RegisterEntryHandler(
      kTestGetPid, primitives::EntryHandler{TestGetpid}));
  ASYLO_RETURN_IF_ERROR(primitives::TrustedPrimitives::RegisterEntryHandler(
      kTestGetPpid, primitives::EntryHandler{TestGetPpid}));
  ASYLO_RETURN_IF_ERROR(primitives::TrustedPrimitives::RegisterEntryHandler(
      kTestSetSid, primitives::EntryHandler{TestSetSid}));
  ASYLO_RETURN_IF_ERROR(primitives::TrustedPrimitives::RegisterEntryHandler(
      kTestKill, primitives::EntryHandler{TestKill}));
  ASYLO_RETURN_IF_ERROR(primitives::TrustedPrimitives::RegisterEntryHandler(
      kTestLink, primitives::EntryHandler{TestLink}));
  ASYLO_RETURN_IF_ERROR(primitives::TrustedPrimitives::RegisterEntryHandler(
      kTestLseek, primitives::EntryHandler{TestLseek}));
  ASYLO_RETURN_IF_ERROR(primitives::TrustedPrimitives::RegisterEntryHandler(
      kTestMkdir, primitives::EntryHandler{TestMkdir}));
  ASYLO_RETURN_IF_ERROR(primitives::TrustedPrimitives::RegisterEntryHandler(
      kTestOpen, primitives::EntryHandler{TestOpen}));
  ASYLO_RETURN_IF_ERROR(primitives::TrustedPrimitives::RegisterEntryHandler(
      kTestUnlink, primitives::EntryHandler{TestUnlink}));
  ASYLO_RETURN_IF_ERROR(primitives::TrustedPrimitives::RegisterEntryHandler(
      kTestUmask, primitives::EntryHandler{TestUmask}));
  ASYLO_RETURN_IF_ERROR(primitives::TrustedPrimitives::RegisterEntryHandler(
      kTestGetUid, primitives::EntryHandler{TestGetuid}));
  ASYLO_RETURN_IF_ERROR(primitives::TrustedPrimitives::RegisterEntryHandler(
      kTestGetGid, primitives::EntryHandler{TestGetgid}));
  ASYLO_RETURN_IF_ERROR(primitives::TrustedPrimitives::RegisterEntryHandler(
      kTestGetEuid, primitives::EntryHandler{TestGeteuid}));
  ASYLO_RETURN_IF_ERROR(primitives::TrustedPrimitives::RegisterEntryHandler(
      kTestGetEgid, primitives::EntryHandler{TestGetegid}));
  ASYLO_RETURN_IF_ERROR(primitives::TrustedPrimitives::RegisterEntryHandler(
      kTestRename, primitives::EntryHandler{TestRename}));
  ASYLO_RETURN_IF_ERROR(primitives::TrustedPrimitives::RegisterEntryHandler(
      kTestRead, primitives::EntryHandler{TestRead}));
  ASYLO_RETURN_IF_ERROR(primitives::TrustedPrimitives::RegisterEntryHandler(
      kTestWrite, primitives::EntryHandler{TestWrite}));
  ASYLO_RETURN_IF_ERROR(primitives::TrustedPrimitives::RegisterEntryHandler(
      kTestSymlink, primitives::EntryHandler{TestSymlink}));
  ASYLO_RETURN_IF_ERROR(primitives::TrustedPrimitives::RegisterEntryHandler(
      kTestReadLink, primitives::EntryHandler{TestReadlink}));
  ASYLO_RETURN_IF_ERROR(primitives::TrustedPrimitives::RegisterEntryHandler(
      kTestTruncate, primitives::EntryHandler{TestTruncate}));
  ASYLO_RETURN_IF_ERROR(primitives::TrustedPrimitives::RegisterEntryHandler(
      kTestFTruncate, primitives::EntryHandler{TestFTruncate}));
  ASYLO_RETURN_IF_ERROR(primitives::TrustedPrimitives::RegisterEntryHandler(
      kTestRmdir, primitives::EntryHandler{TestRmdir}));
  ASYLO_RETURN_IF_ERROR(primitives::TrustedPrimitives::RegisterEntryHandler(
      kTestSocket, primitives::EntryHandler{TestSocket}));
  ASYLO_RETURN_IF_ERROR(primitives::TrustedPrimitives::RegisterEntryHandler(
      kTestFcntl, primitives::EntryHandler{TestFcntl}));
  ASYLO_RETURN_IF_ERROR(primitives::TrustedPrimitives::RegisterEntryHandler(
      kTestChown, primitives::EntryHandler{TestChown}));
  ASYLO_RETURN_IF_ERROR(primitives::TrustedPrimitives::RegisterEntryHandler(
      kTestFChown, primitives::EntryHandler{TestFChown}));
  ASYLO_RETURN_IF_ERROR(primitives::TrustedPrimitives::RegisterEntryHandler(
      kTestSetSockOpt, primitives::EntryHandler{TestSetsockopt}));
  ASYLO_RETURN_IF_ERROR(primitives::TrustedPrimitives::RegisterEntryHandler(
      kTestFlock, primitives::EntryHandler{TestFlock}));
  ASYLO_RETURN_IF_ERROR(primitives::TrustedPrimitives::RegisterEntryHandler(
      kTestFsync, primitives::EntryHandler{TestFsync}));
  ASYLO_RETURN_IF_ERROR(primitives::TrustedPrimitives::RegisterEntryHandler(
      kTestInotifyInit1, primitives::EntryHandler{TestInotifyInit1}));
  ASYLO_RETURN_IF_ERROR(primitives::TrustedPrimitives::RegisterEntryHandler(
      kTestInotifyAddWatch, primitives::EntryHandler{TestInotifyAddWatch}));
  ASYLO_RETURN_IF_ERROR(primitives::TrustedPrimitives::RegisterEntryHandler(
      kTestInotifyRmWatch, primitives::EntryHandler{TestInotifyRmWatch}));
  ASYLO_RETURN_IF_ERROR(primitives::TrustedPrimitives::RegisterEntryHandler(
      kTestSchedYield, primitives::EntryHandler{TestSchedYield}));
  ASYLO_RETURN_IF_ERROR(primitives::TrustedPrimitives::RegisterEntryHandler(
      kTestIsAtty, primitives::EntryHandler{TestIsAtty}));
  ASYLO_RETURN_IF_ERROR(primitives::TrustedPrimitives::RegisterEntryHandler(
      kTestUSleep, primitives::EntryHandler{TestUSleep}));

  return primitives::PrimitiveStatus::OkStatus();
}

// Implements the required enclave finalization function.
extern "C" primitives::PrimitiveStatus asylo_enclave_fini() {
  return primitives::PrimitiveStatus::OkStatus();
}

}  // namespace host_call
}  // namespace asylo

extern "C" PrimitiveStatus enc_init() { return PrimitiveStatus::OkStatus(); }
