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

#include "asylo/identity/sgx/platform_provisioning.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "asylo/crypto/util/trivial_object_util.h"
#include "asylo/identity/sgx/identity_key_management_structs.h"
#include "asylo/identity/sgx/platform_provisioning.pb.h"
#include "asylo/test/util/status_matchers.h"
#include "asylo/util/status.h"

namespace asylo {
namespace sgx {
namespace {

using ::testing::Eq;

TEST(ProvisioningPlatformTest, PpidWithoutValueFieldIsInvalid) {
  EXPECT_THAT(ValidatePpid(Ppid()),
              StatusIs(error::GoogleError::INVALID_ARGUMENT));
}

TEST(ProvisioningPlatformTest, PpidWithValueFieldOfBadLengthIsInvalid) {
  Ppid ppid;
  *ppid.mutable_value() = "short";
  EXPECT_THAT(ValidatePpid(ppid),
              StatusIs(error::GoogleError::INVALID_ARGUMENT));

  *ppid.mutable_value() = "waaaaaaaaaaaaaaaaaaaaaaaaaaytoolong";
  EXPECT_THAT(ValidatePpid(ppid),
              StatusIs(error::GoogleError::INVALID_ARGUMENT));
}

TEST(ProvisioningPlatformTest, ValidPpidIsValid) {
  Ppid ppid;
  *ppid.mutable_value() = "0123456789abcdef";
  ASYLO_EXPECT_OK(ValidatePpid(ppid));
}

TEST(ProvisioningPlatformTest, CpuSvnWithoutValueFieldIsInvalid) {
  EXPECT_THAT(ValidateCpuSvn(CpuSvn()),
              StatusIs(error::GoogleError::INVALID_ARGUMENT));
}

TEST(ProvisioningPlatformTest, CpuSvnWithValueFieldOfBadLengthIsInvalid) {
  CpuSvn cpu_svn;
  *cpu_svn.mutable_value() = "short";
  EXPECT_THAT(ValidateCpuSvn(cpu_svn),
              StatusIs(error::GoogleError::INVALID_ARGUMENT));

  *cpu_svn.mutable_value() = "waaaaaaaaaaaaaaaaaaaaaaaaaaytoolong";
  EXPECT_THAT(ValidateCpuSvn(cpu_svn),
              StatusIs(error::GoogleError::INVALID_ARGUMENT));
}

TEST(ProvisioningPlatformTest, ValidCpuSvnIsValid) {
  CpuSvn cpu_svn;
  *cpu_svn.mutable_value() = "0123456789abcdef";
  ASYLO_EXPECT_OK(ValidateCpuSvn(cpu_svn));
}

TEST(PlatformProvisioningTest, PceSvnWithoutValueFieldIsInvalid) {
  EXPECT_THAT(ValidatePceSvn(PceSvn()),
              StatusIs(error::GoogleError::INVALID_ARGUMENT));
}

TEST(PlatformProvisioningTest, PceSvnWithTooLargeValueFieldIsInvalid) {
  PceSvn pce_svn;
  pce_svn.set_value(100000);
  EXPECT_THAT(ValidatePceSvn(pce_svn),
              StatusIs(error::GoogleError::INVALID_ARGUMENT));
}

TEST(PlatformProvisioningTest, ValidPceSvnIsValid) {
  PceSvn pce_svn;
  pce_svn.set_value(10000);
  ASYLO_EXPECT_OK(ValidatePceSvn(pce_svn));
}

TEST(PlatformProvisioningTest, PceIdWithoutValueFieldIsInvalid) {
  EXPECT_THAT(ValidatePceId(PceId()),
              StatusIs(error::GoogleError::INVALID_ARGUMENT));
}

TEST(PlatformProvisioningTest, PceIdWithTooLargeValueFieldIsInvalid) {
  PceId pce_id;
  pce_id.set_value(100000);
  EXPECT_THAT(ValidatePceId(pce_id),
              StatusIs(error::GoogleError::INVALID_ARGUMENT));
}

TEST(PlatformProvisioningTest, ValidPceIdIsValid) {
  PceId pce_id;
  pce_id.set_value(10000);
  ASYLO_EXPECT_OK(ValidatePceId(pce_id));
}

TEST(PlatformProvisioningTest, FmspcWithoutValueFieldIsInvalid) {
  EXPECT_THAT(ValidateFmspc(Fmspc()),
              StatusIs(error::GoogleError::INVALID_ARGUMENT));
}

TEST(PlatformProvisioningTest, FmspcWithValueFieldOfBadLengthIsInvalid) {
  Fmspc fmspc;
  fmspc.set_value("short");
  EXPECT_THAT(ValidateFmspc(fmspc),
              StatusIs(error::GoogleError::INVALID_ARGUMENT));

  fmspc.set_value("toolong");
  EXPECT_THAT(ValidateFmspc(fmspc),
              StatusIs(error::GoogleError::INVALID_ARGUMENT));
}

TEST(PlatformProvisioningTest, ValidFmspcIsValid) {
  Fmspc fmspc;
  fmspc.set_value("000000");
  ASYLO_EXPECT_OK(ValidateFmspc(fmspc));
}

TEST(PlatformProvisioningTest, ReportProtoWithoutValueFieldIsInvalid) {
  ReportProto report_proto;
  EXPECT_THAT(ValidateReportProto(report_proto),
              StatusIs(error::GoogleError::INVALID_ARGUMENT));
  EXPECT_THAT(ConvertReportProtoToHardwareReport(report_proto).status(),
              StatusIs(error::GoogleError::INVALID_ARGUMENT));
}

TEST(PlatformProvisioningTest, ReportProtoWithValueFieldOfBadLengthIsInvalid) {
  ReportProto report_proto;
  report_proto.set_value("short");
  EXPECT_THAT(ValidateReportProto(report_proto),
              StatusIs(error::GoogleError::INVALID_ARGUMENT));
  EXPECT_THAT(ConvertReportProtoToHardwareReport(report_proto).status(),
              StatusIs(error::GoogleError::INVALID_ARGUMENT));

  Report report = TrivialRandomObject<Report>();
  std::string report_bin = ConvertTrivialObjectToBinaryString(report);
  // One byte too long.
  report_bin.push_back('a');
  report_proto.set_value(report_bin);
  EXPECT_THAT(ValidateReportProto(report_proto),
              StatusIs(error::GoogleError::INVALID_ARGUMENT));
  EXPECT_THAT(ConvertReportProtoToHardwareReport(report_proto).status(),
              StatusIs(error::GoogleError::INVALID_ARGUMENT));
}

TEST(PlatformProvisioningTest, TargetInfoProtoWithoutValueFieldIsInvalid) {
  TargetInfoProto target_info_proto;
  EXPECT_THAT(ValidateTargetInfoProto(target_info_proto),
              StatusIs(error::GoogleError::INVALID_ARGUMENT));
  EXPECT_THAT(ConvertTargetInfoProtoToTargetinfo(target_info_proto).status(),
              StatusIs(error::GoogleError::INVALID_ARGUMENT));
}

TEST(PlatformProvisioningTest,
     TargetInfoProtoWithValueFieldOfBadLengthIsInvalid) {
  TargetInfoProto target_info_proto;
  target_info_proto.set_value("short");
  EXPECT_THAT(ValidateTargetInfoProto(target_info_proto),
              StatusIs(error::GoogleError::INVALID_ARGUMENT));
  EXPECT_THAT(ConvertTargetInfoProtoToTargetinfo(target_info_proto).status(),
              StatusIs(error::GoogleError::INVALID_ARGUMENT));

  Targetinfo targetinfo = TrivialRandomObject<Targetinfo>();
  std::string targetinfo_bin = ConvertTrivialObjectToBinaryString(targetinfo);
  // One byte too long.
  targetinfo_bin.push_back('a');
  target_info_proto.set_value(targetinfo_bin);
  EXPECT_THAT(ValidateTargetInfoProto(target_info_proto),
              StatusIs(error::GoogleError::INVALID_ARGUMENT));
  EXPECT_THAT(ConvertTargetInfoProtoToTargetinfo(target_info_proto).status(),
              StatusIs(error::GoogleError::INVALID_ARGUMENT));
}

TEST(PlatformProvisioningTest, ValidReportCanBeConvertedToHardwareReport) {
  Report expected = TrivialRandomObject<Report>();

  ReportProto report_proto;
  report_proto.set_value(ConvertTrivialObjectToBinaryString(expected));

  auto report_result = ConvertReportProtoToHardwareReport(report_proto);
  ASYLO_ASSERT_OK(report_result);

  std::string expected_hex = ConvertTrivialObjectToHexString<Report>(expected);
  std::string actual_hex =
      ConvertTrivialObjectToHexString(report_result.ValueOrDie());
  EXPECT_THAT(actual_hex, Eq(expected_hex));
}

TEST(PlatformProvisioningTest, ValidTargetInfoProtoCanBeConvertedToTargetinfo) {
  Targetinfo expected = TrivialRandomObject<Targetinfo>();

  TargetInfoProto target_info_proto;
  target_info_proto.set_value(ConvertTrivialObjectToBinaryString(expected));

  auto targetinfo_result =
      ConvertTargetInfoProtoToTargetinfo(target_info_proto);
  ASYLO_ASSERT_OK(targetinfo_result);

  std::string expected_hex =
      ConvertTrivialObjectToHexString<Targetinfo>(expected);
  std::string actual_hex =
      ConvertTrivialObjectToHexString(targetinfo_result.ValueOrDie());
  EXPECT_THAT(actual_hex, Eq(expected_hex));
}

}  // namespace
}  // namespace sgx
}  // namespace asylo
