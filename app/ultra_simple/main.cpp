/*
 *  RPLIDAR
 *  Ultra Simple Data Grabber Demo App
 *
 *  Copyright (c) 2009 - 2014 RoboPeak Team
 *  http://www.robopeak.com
 *  Copyright (c) 2014 - 2016 Shanghai Slamtec Co., Ltd.
 *  http://www.slamtec.com
 *
 */
/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <netdb.h>
#include <netinet/in.h>

#include "rplidar.h" //RPLIDAR standard sdk, all-in-one header

#ifndef _countof
#define _countof(_Array) (int)(sizeof(_Array) / sizeof(_Array[0]))
#endif

#ifdef _WIN32
#include <Windows.h>
#define delay(x) ::Sleep(x)
#else

#include <unistd.h>

static inline void delay(_word_size_t ms)
{
    while (ms >= 1000)
    {
        usleep(1000 * 1000);
        ms -= 1000;
    };
    if (ms != 0)
        usleep(ms * 1000);
}

#endif

using namespace rp::standalone::rplidar;

bool checkRPLIDARHealth(RPlidarDriver *drv)
{
    u_result op_result;
    rplidar_response_device_health_t healthinfo;

    op_result = drv->getHealth(healthinfo);
    if (IS_OK(op_result))
    { // the macro IS_OK is the preperred way to judge whether the operation is succeed.
        printf("RPLidar health status : %d\n", healthinfo.status);
        if (healthinfo.status == RPLIDAR_STATUS_ERROR)
        {
            fprintf(stderr, "Error, rplidar internal error detected. Please reboot the device to retry.\n");
            // enable the following code if you want rplidar to be reboot by software
            // drv->reset();
            return false;
        }
        else
        {
            return true;
        }
    }
    else
    {
        fprintf(stderr, "Error, cannot retrieve the lidar health code: %x\n", op_result);
        return false;
    }
}

#include <signal.h>

bool ctrl_c_pressed;

void ctrlc(int)
{
    ctrl_c_pressed = true;
}

struct NodeData
{
    char *sync;
    float distance;
    float angle;
    int quality;
};

/*void cycleOneCircle(NodeData nodeData) {
    printf("%s theta: %03.2f Dist: %08.2f Q: %d \n",
//           nodeData.sync,
           nodeData.angle,
           nodeData.distance,
           nodeData.quality);
}*/

/*void restart(int socket, sockaddr_in *addrcli) {
    unsigned int clilen;
    clilen = sizeof(&addrcli);

    int newsockfd = accept(socket, addrcli, &clilen);
}*/

int main(int argc, const char *argv[])
{
    int sockfd, newsockfd, portno;
    unsigned int clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    int  n;

    /* First call to socket() function */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) {
       perror("ERROR opening socket");
       exit(1);
    }

    /* Initialize socket structure */
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = 5555;

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    /* Now bind the host address using bind() call.*/
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
       perror("ERROR on binding");
       exit(1);
    }

   /* Now start listening for the clients, here process will
      * go in sleep mode and will wait for the incoming connection
   */

   printf("listening to port : %d", portno);
   listen(sockfd,5);
   clilen = sizeof(cli_addr);

accept_new:
   /* Accept actual connection from the client */
   newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);

   if (newsockfd < 0) {
      perror("ERROR on accept");
      exit(1);
   }

    /* If connection is established then start communicating */
    bzero(buffer,256);

    const char *opt_com_path = NULL;
    _u32 opt_com_baudrate = 115200;
    u_result op_result;

    printf("Ultra simple LIDAR data grabber for RPLIDAR.\n Version: " RPLIDAR_SDK_VERSION
           "\n");

    // read serial port from the command line...
    if (argc > 1)
        opt_com_path = argv[1]; // or set to a fixed value: e.g. "com3"

    // read baud rate from the command line if specified...
    if (argc > 2)
        opt_com_baudrate = strtoul(argv[2], NULL, 10);

    if (!opt_com_path)
    {
#ifdef _WIN32
        // use default com port
        opt_com_path = "\\\\.\\com3";
#else
        opt_com_path = "/dev/ttyUSB0";
#endif
    }

    // create the driver instance
    RPlidarDriver *drv = RPlidarDriver::CreateDriver(RPlidarDriver::DRIVER_TYPE_SERIALPORT);

    if (!drv)
    {
        fprintf(stderr, "insufficent memory, exit\n");
        exit(-2);
    }

    // make connection...
    if (IS_FAIL(drv->connect(opt_com_path, opt_com_baudrate)))
    {
        fprintf(stderr, "Error, cannot bind to the specified serial port %s.\n", opt_com_path);
        goto on_finished;
    }

    signal(SIGINT, ctrlc);

    rplidar_response_device_info_t devinfo;

    // check health...
    if (!checkRPLIDARHealth(drv))
    {
        goto on_finished;
    }

    //signal(SIGINT, ctrlc);
    drv->startMotor();
    // start scan...
    drv->startScan();

    // fetech result and print it out...
    while (1)
    {
        rplidar_response_measurement_node_t nodes[360 * 2];
        size_t count = _countof(nodes);

        op_result = drv->grabScanData(nodes, count);

        if (IS_OK(op_result))
        {
            drv->ascendScanData(nodes, count);
            char *sync;
            float angle;
            float distance;
            int quality;
            float minDisRight = 6000;
            float minDisLeft = 6000;

            // 0 caution
            // 1 normal
            // 2 right obs
            // 3 left obs
            // 4 slow down
            int state = 1;
            for (int pos = 0; pos < (int)count; ++pos)
            {
                //                nodeData.sync = (nodes[pos].sync_quality & RPLIDAR_RESP_MEASUREMENT_SYNCBIT) ? "S " : "  ";
                angle = (nodes[pos].angle_q6_checkbit >> RPLIDAR_RESP_MEASUREMENT_ANGLE_SHIFT) / 64.0f;
                distance = nodes[pos].distance_q2 / 4.0f;
                quality = nodes[pos].sync_quality >> RPLIDAR_RESP_MEASUREMENT_QUALITY_SHIFT;

                if (angle > 70 && angle <= 90)
                {
                    if (quality != 0 && distance != 0)
                    {
                        if (minDisLeft > distance)
                        {
                            minDisLeft = distance;
                        }
                    }
                    else if (quality != 0 && distance == 0)
                        state = 0;
                }
                else if (angle > 90 && angle <= 110)
                {
                    if (quality != 0 && distance != 0)
                    {
                        if (minDisRight > distance)
                        {
                            minDisRight = distance;
                        }
                    }
                    else if (quality != 0 && distance == 0)
                        state = 0;
                }
                // printf("theta: %03.2f Dist: %08.2f Q: %d \n",
                //            angle,
                //            distance,
                //            quality);
            }

            if (minDisLeft < 2000 && state != 0)
            {
                state = 3;
            }
            if (minDisRight < 2000 && state != 0)
            {
                state = 2;
                if (minDisLeft < 2000)
                {
                    if (minDisLeft < minDisRight)
                    {
                        state = 3;
                    }
                }
            }
            if (minDisLeft < 800 || minDisRight < 800) {
                state = 4;
            }

            char tmp1[10];
            bzero(tmp1, 10);
            sprintf(tmp1, "%d", state);

            n = write(newsockfd, tmp1, 10);
            printf("n = %d state: %d mL: %.0f mR: %.0f \n", n, state, minDisLeft, minDisRight);
            if (n < 0) {
                drv->stop();
                drv->stopMotor();
                RPlidarDriver::DisposeDriver(drv);
                goto accept_new;
            }
        }

        if (ctrl_c_pressed)
        {
            break;
        }
    }

    drv->stop();
    drv->stopMotor();
// done!
on_finished:
    RPlidarDriver::DisposeDriver(drv);
    return 0;
}
