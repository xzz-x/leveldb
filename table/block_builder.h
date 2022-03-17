// Copyright (c) 2011 The LevelDB Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file. See the AUTHORS file for names of contributors.

#ifndef STORAGE_LEVELDB_TABLE_BLOCK_BUILDER_H_
#define STORAGE_LEVELDB_TABLE_BLOCK_BUILDER_H_

#include <cstdint>
#include <vector>

#include "leveldb/slice.h"

namespace leveldb {

struct Options;

// 用于构建index_block/data_block/meta_block的构建
class BlockBuilder {
 public:
//  刚刚开始restart的第一个偏移量为0
  explicit BlockBuilder(const Options* options);

  BlockBuilder(const BlockBuilder&) = delete;
  BlockBuilder& operator=(const BlockBuilder&) = delete;

  // Reset the contents as if the BlockBuilder was just constructed.
  // 清空数据
  void Reset();

  // REQUIRES: Finish() has not been called since the last call to Reset().
  // REQUIRES: key is larger than any previously added key
  void Add(const Slice& key, const Slice& value);

  // Finish building the block and return a slice that refers to the
  // block contents.  The returned slice will remain valid for the
  // lifetime of this builder or until Reset() is called.
  // 因为flush在循环中进行的；最后一次没办法flush；需要额外进行Finish一把
  Slice Finish();

  // Returns an estimate of the current (uncompressed) size of the block
  // we are building.
  // 当前block的大小；检查是否超过了
  size_t CurrentSizeEstimate() const;

  // Return true iff no entries have been added since the last Reset()
  bool empty() const { return buffer_.empty(); }

 private:
//配置相关的  
  const Options* options_;
  // 序列化后的数据
  std::string buffer_;              // Destination buffer
  // 内部重启点的具体数值;这是一个偏移量
  std::vector<uint32_t> restarts_;  // Restart points
  // 重启点的计数器
  int counter_;                     // Number of entries emitted since restart
  // 是否结束
  bool finished_;                   // Has Finish() been called?
  // 上次记录的key
  std::string last_key_;
};

}  // namespace leveldb

#endif  // STORAGE_LEVELDB_TABLE_BLOCK_BUILDER_H_
