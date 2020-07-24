//
// The MIT License (MIT)
//
// Copyright (c) 2019 Livox. All rights reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

#include "lds_lvx.h"
#include "lds.h"
#include <functional>
#include <memory>
#include <stdio.h>
#include <string.h>
#include <thread>
#include <unistd.h> /*usleep*/

#include "lvx_file.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

namespace livox_ros {

/** Const varible
 * --------------------------------------------------------------------------------
 */
const uint32_t kMaxPacketsNumOfFrame = 8192;

/** For device connect use
 * ---------------------------------------------------------------------- */
LdsLvx::LdsLvx(uint32_t interval_ms) : Lds(interval_ms, kSourceLvxFile) {
  start_read_lvx_ = false;
  is_initialized_ = false;
  lvx_file_ = std::make_shared<LvxFileHandle>();
  packets_of_frame_.buffer_capacity =
      kMaxPacketsNumOfFrame * sizeof(LvxFilePacket);
  packets_of_frame_.packet =
      new uint8_t[kMaxPacketsNumOfFrame * sizeof(LvxFilePacket)];
}

LdsLvx::~LdsLvx() {
  if (packets_of_frame_.packet != nullptr) {
    delete[] packets_of_frame_.packet;
  }
  if (t_read_lvx_->joinable()) {
    t_read_lvx_->join();
  }
  if (t_show_lvx_->joinable()) {
    t_show_lvx_->join();
  }
}

void LdsLvx::PrepareExit(void) {
  lvx_file_->CloseLvxFile();
  printf("Lvx to rosbag convert complete and exit!\n");
}

int LdsLvx::InitLdsLvx(const char *lvx_path) {
  if (is_initialized_) {
    printf("Livox file data source is already inited!\n");
    return -1;
  }

  int ret = lvx_file_->Open(lvx_path, std::ios::in);
  if (ret) {
    printf("Open %s file fail[%d]!\n", lvx_path, ret);
    return ret;
  }

  if (lvx_file_->GetFileVersion() == kLvxFileV1) {
    ResetLds(kSourceRawLidar);
  } else {
    ResetLds(kSourceLvxFile);
  }

  lidar_count_ = lvx_file_->GetDeviceCount();
  if (!lidar_count_ || (lidar_count_ >= kMaxSourceLidar)) {
    lvx_file_->CloseLvxFile();
    printf("Lidar count error in %s : %d\n", lvx_path, lidar_count_);
    return -1;
  }
  printf("LvxFile[%s] have %d lidars\n", lvx_path, lidar_count_);

  for (int i = 0; i < lidar_count_; i++) {
    LvxFileDeviceInfo lvx_dev_info;
    lvx_file_->GetDeviceInfo(i, &lvx_dev_info);
    lidars_[i].handle = i;
    lidars_[i].connect_state = kConnectStateSampling;
    lidars_[i].info.handle = i;
    lidars_[i].info.type = lvx_dev_info.device_type;
    memcpy(lidars_[i].info.broadcast_code, lvx_dev_info.lidar_broadcast_code,
           sizeof(lidars_[i].info.broadcast_code));

    if (lvx_file_->GetFileVersion() == kLvxFileV1) {
      lidars_[i].data_src = kSourceRawLidar;
    } else {
      lidars_[i].data_src = kSourceLvxFile;
    }

    ExtrinsicParameter *p_extrinsic = &lidars_[i].extrinsic_parameter;
    p_extrinsic->euler[0] = lvx_dev_info.roll * PI / 180.0;
    p_extrinsic->euler[1] = lvx_dev_info.pitch * PI / 180.0;
    p_extrinsic->euler[2] = lvx_dev_info.yaw * PI / 180.0;
    p_extrinsic->trans[0] = lvx_dev_info.x;
    p_extrinsic->trans[1] = lvx_dev_info.y;
    p_extrinsic->trans[2] = lvx_dev_info.z;
    EulerAnglesToRotationMatrix(p_extrinsic->euler, p_extrinsic->rotation);
    p_extrinsic->enable = lvx_dev_info.extrinsic_enable;

    uint32_t queue_size = kMaxEthPacketQueueSize * 16;
    // InitQueue(&lidars_[i].data, queue_size);
    queue_size = kMaxEthPacketQueueSize;
    // InitQueue(&lidars_[i].imu_data, queue_size);
  }

  t_read_lvx_ = std::make_shared<std::thread>(std::bind(&LdsLvx::ReadLvxFile, this));
  t_show_lvx_ = std::make_shared<std::thread>(std::bind(&LdsLvx::ShowLvxFile, this));
  is_initialized_ = true;

  StartRead();

  return ret;
}

/** Global function in LdsLvx for callback */
void LdsLvx::ReadLvxFile() {
  while (!start_read_lvx_)
    ;
  printf("Start to read lvx file.\n");

  int file_state = kLvxFileOk;
  int progress = 0;
  while (start_read_lvx_) {
    file_state = lvx_file_->GetPacketsOfFrame(&packets_of_frame_);
    if (!file_state) {
      uint32_t data_size = packets_of_frame_.data_size;
      uint8_t *packet_base = packets_of_frame_.packet;
      uint32_t data_offset = 0;
      while (data_offset < data_size) {
        LivoxEthPacket *data;
        int32_t handle;
        uint8_t data_type;
        if (lvx_file_->GetFileVersion()) {
          LvxFilePacket *detail_packet =
              (LvxFilePacket *)&packet_base[data_offset];
          data = (LivoxEthPacket *)(&detail_packet->version);
          handle = detail_packet->device_index;
        } else {
          LvxFilePacketV0 *detail_packet =
              (LvxFilePacketV0 *)&packet_base[data_offset];
          data = (LivoxEthPacket *)(&detail_packet->version);
          handle = detail_packet->device_index;
        }

        data_type = data->data_type;
        /** Packet length + device index */
        data_offset += (GetEthPacketLen(data_type) + 1); 

        uint64_t cur_timestamp = *((uint64_t *)(data->timestamp));

        if ( data ->data_type == kExtendCartesian) {

          LivoxExtendRawPoint *p_point_data = (LivoxExtendRawPoint *)data->data; /* length is 96 */
          LivoxPointXyzrtl* dst_point = new LivoxPointXyzrtl[GetPointsPerPacket(data->data_type)];
          // printf("%d %d %d\n",p_point_data[95].x, p_point_data[95].y, p_point_data[95].z);
          dst_point = (LivoxPointXyzrtl*) FillZeroPointXyzrtl( (uint8_t*) dst_point, 96); /* points to the first element after the array */
          dst_point -= GetPointsPerPacket(data->data_type); /* revert to first elem*/
          dst_point = (LivoxPointXyzrtl*) LivoxExtendRawPointToPxyzrtl( (uint8_t*) dst_point, data ,lidars_[handle].extrinsic_parameter); /* point to first after end */
          dst_point -= GetPointsPerPacket(data->data_type); /* revert to first elem */
          // printf("%f %f %f\n",dst_point[95].x, dst_point[95].y, dst_point[95].z);
          if(_scene->GLSceneReady_){
            // glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
            // glBufferSubData(GL_ARRAY_BUFFER, 0, 96 * sizeof(LivoxPointXyzrtl), dst_point);
            _scene->update(dst_point);
          }
          delete[] dst_point;

          usleep(100);

        }else if ( data ->data_type == kImu) {
          LivoxImuPoint *p_point_data = (LivoxImuPoint *)data->data;
        }
      }
    } else {
      if (file_state != kLvxFileAtEnd) {
        printf("Exit read the lvx file, read file state[%d]!\n", file_state);
      } else {
        printf("Read the lvx file complete!\n");
      }
      break;
    }

    if (progress != lvx_file_->GetLvxFileReadProgress()) {
      progress = lvx_file_->GetLvxFileReadProgress();
      printf("Read progress : %d \n", progress);
    }
  }

  RequestExit();
}

void LdsLvx::ShowLvxFile(){
  _scene->init();
}

int LdsLvx::DeInitLdsLvx(void) {

  if (!is_initialized_) {
    printf("LVX data source is not exit");
    return -1;
  }

  Uninit();
  printf("Livox SDK Deinit completely!\n");

  return 0;
}

} // namespace livox_ros
