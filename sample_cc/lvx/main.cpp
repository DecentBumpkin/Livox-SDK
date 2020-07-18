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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "lds_lvx.h"
#include <iostream>

using namespace livox_ros;
int main(int argc, const char *argv[]) {

  double publish_freq = 10.0; /* Hz recommended values 5.0, 10.0, 20.0, 50.0, etc.*/

  LdsLvx *read_lvx = LdsLvx::GetInstance(1000 / publish_freq);
  int ret = read_lvx->InitLdsLvx("/home/wei/PolyExplore/Livox_Viewr_For_Linux_Ubuntu16.04_x64_0.7.0/data/record files/Horizon_道路场景点云数据_官网.lvx");
  if (!ret) {
      std::cout << "Init lds lvx file success!\n";
  } else {
      std::cerr<< "Init lds lvx file fail!\n";
  }

  printf("Start reading lvx file.\n");

#ifdef WIN32
  Sleep(100000);
#else
  sleep(100);
#endif

  /* unlike ros there is no main loop here, so I assume everything has to happen in this SDK-version of LdsLidar::InitLdsLidar */
  read_lvx->DeInitLdsLvx();
  printf("Livox lidar demo end!\n");

}
