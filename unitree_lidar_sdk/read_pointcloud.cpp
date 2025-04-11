/**********************************************************************
 Copyright (c) 2020-2023, Unitree Robotics.Co.Ltd. All rights reserved.
***********************************************************************/

#include "unitree_lidar_sdk.h"
#include <fstream>

using namespace unitree_lidar_sdk;

int main(){

  // Initialize Lidar Object
  UnitreeLidarReader* lreader = createUnitreeLidarReader();
  int cloud_scan_num = 18;
  std::string port_name = "/dev/ttyUSB0";

  if ( lreader->initialize(cloud_scan_num, port_name) ){
    printf("Unilidar initialization failed! Exit here!\n");
    exit(-1);
  }else{
    printf("Unilidar initialization succeed!\n");
  }

  // Set Lidar Working Mode
  printf("Set Lidar working mode to: NORMAL ... \n");
  lreader->setLidarWorkingMode(NORMAL);
  sleep(1);

  printf("\n");

  // Print Lidar Version
  while(true){
    if (lreader->runParse() == VERSION){
      printf("lidar firmware version = %s\n", lreader->getVersionOfFirmware().c_str() );
      break;
    }
    usleep(500);
  }
  printf("lidar sdk version = %s\n\n", lreader->getVersionOfSDK().c_str());
  sleep(2);

  // Check lidar dirty percentange
  int count_percentage = 0;
  while(true){
    if( lreader->runParse() == AUXILIARY){
      printf("Dirty Percentage = %f %%\n", lreader->getDirtyPercentage());
      if (++count_percentage > 2){
        break;
      }
      if (lreader->getDirtyPercentage() > 10){
        printf("The protection cover is too dirty! Please clean it right now! Exit here ...\n");
        exit(0);
      }
    }
    usleep(500);
  }
  printf("\n");
  sleep(2);

  // Set LED
  uint8_t led_table[45];
  printf("Turn off all the LED lights ...\n");
  for (int i=0; i < 45; i++){
    led_table[i] = 0x00;
  }
  lreader->setLEDDisplayMode(led_table);

  printf("\n");
  sleep(2);

  // Parse PointCloud and IMU data
  MessageType result;
  std::string version;

  std::ofstream out_file;
  out_file.open("data.txt");

  while (true)
  {

    result = lreader->runParse(); // You need to call this function at least 1500Hz

    switch (result)
    {
    case NONE:
      break;
    
    case IMU:
      break;
    
    case POINTCLOUD:
      {
        printf("A Cloud msg is parsed! \n");
        long timestamp = static_cast<long>(lreader->getCloud().stamp);
        int id = lreader->getCloud().id;
        printf("\tstamp = %ld, id = %d\n", timestamp, id);
        long point_size = lreader->getCloud().points.size();
        int ring_num = lreader->getCloud().ringNum;
        printf("\tcloud size  = %ld, ringNum = %d\n", point_size, lreader->getCloud().ringNum);
        out_file << timestamp << ',' << id << ',' << point_size << ',' << ring_num << std::endl;
        std::vector<PointUnitree> points = lreader->getCloud().points;
        for (int i = 0; i< point_size; i++){ // print the first 10 points
          out_file << points[i].x << ',' << points[i].y << ',' << points[i].z << ',' << points[i].intensity << ',' << points[i].time << ',' << points[i].ring << std::endl;
        }
        printf("\t  ...\n");
        printf("\ttimedelay (us) = %d\n\n", lreader->getTimeDelay()); 
        break;
      }

    default:
      break;
    }

  }

  out_file.close();
  
  return 0;
}