SDK Comments
c++ examples in sample_cc/lidar folder, main.cpp calls lds_lidar.cpp
lds_lidar.cpp catch the pointcloud and does the static_cast, conversions To Be Done

sdk_core/src/data_handler/ set up the structure, but does not define the callbacks, callbacks defined in lds_lidar.cpp
LdsLidar::OnDeviceBroadcast ->  LdsLidar::GetLidarDataCb
Why bd_list cannot be recognized?


Update:

ethernet socket received and callback setup in lidar_data_handler.cpp
data_handler.cpp DataHandler::OnDataCallback process the raw data and call another callback LdsLidar::GetLidarDataCb defined in lds_lidar.cpp