/*
 * Copyright (c) 2026 OceanBase.
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
 */

#include <gtest/gtest.h>
#include "lib/utility/ob_common_utility.h"

namespace oceanbase
{
namespace common
{
namespace
{

int fill_success(unsigned char *buf, const int64_t len)
{
  const unsigned char bytes[] = {0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc};
  if (OB_ISNULL(buf) || static_cast<int64_t>(sizeof(bytes)) != len) {
    return 0;
  }
  MEMCPY(buf, bytes, sizeof(bytes));
  return 1;
}

int fill_failure(unsigned char *buf, const int64_t len)
{
  UNUSED(buf);
  UNUSED(len);
  return 0;
}

TEST(ObUuidNodeRandomTest, UsesMulticastBitAndFillsAllBytes)
{
  unsigned char node[6] = {0};
  ASSERT_EQ(OB_SUCCESS, fill_random_uuid_node(node, sizeof(node), fill_success));
  EXPECT_EQ(0x13, node[0]);
  EXPECT_EQ(0x34, node[1]);
  EXPECT_EQ(0x56, node[2]);
  EXPECT_EQ(0x78, node[3]);
  EXPECT_EQ(0x9a, node[4]);
  EXPECT_EQ(0xbc, node[5]);
}

TEST(ObUuidNodeRandomTest, DoesNotPublishPartialNodeOnEntropyFailure)
{
  unsigned char node[6] = {0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};
  const unsigned char expected[6] = {0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};
  EXPECT_EQ(OB_ERR_UNEXPECTED,
      fill_random_uuid_node(node, sizeof(node), fill_failure));
  for (int64_t i = 0; i < static_cast<int64_t>(sizeof(node)); ++i) {
    EXPECT_EQ(expected[i], node[i]);
  }
}

TEST(ObUuidNodeRandomTest, RejectsInvalidArguments)
{
  unsigned char node[6] = {0};
  EXPECT_EQ(OB_INVALID_ARGUMENT, fill_random_uuid_node(NULL, sizeof(node), fill_success));
  EXPECT_EQ(OB_INVALID_ARGUMENT, fill_random_uuid_node(node, 5, fill_success));
  EXPECT_EQ(OB_INVALID_ARGUMENT, fill_random_uuid_node(node, sizeof(node), NULL));
}

} // namespace
} // namespace common
} // namespace oceanbase
