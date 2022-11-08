#if defined (_WINDOWS_)
#include <windows.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <list>

#include <k4a/k4a.h>
#include <k4abt.h>

#include <GestureDetector.hpp>
#include "tcpserver.hpp"

#define VERIFY(result, error)                                                                            \
    if(result != K4A_RESULT_SUCCEEDED)                                                                   \
    {                                                                                                    \
        printf("%s \n - (File: %s, Function: %s, Line: %d)\n", error, __FILE__, __FUNCTION__, __LINE__); \
        exit(EXIT_FAILURE);                                                                              \
    }                                                                                                    \

int main()
{
    k4a_device_t device = NULL;
    VERIFY(k4a_device_open(0, &device), "Open K4A Device failed!");

    // Start camera. Make sure depth camera is enabled.
    k4a_device_configuration_t deviceConfig = K4A_DEVICE_CONFIG_INIT_DISABLE_ALL;
    deviceConfig.depth_mode = K4A_DEPTH_MODE_NFOV_UNBINNED;
    deviceConfig.color_resolution = K4A_COLOR_RESOLUTION_OFF;
    VERIFY(k4a_device_start_cameras(device, &deviceConfig), "Start K4A cameras failed!");

    k4a_calibration_t sensor_calibration;
    VERIFY(k4a_device_get_calibration(device, deviceConfig.depth_mode, deviceConfig.color_resolution, &sensor_calibration),
        "Get depth camera calibration failed!");

    k4abt_tracker_t tracker = NULL;
    k4abt_tracker_configuration_t tracker_config = K4ABT_TRACKER_CONFIG_DEFAULT;
    VERIFY(k4abt_tracker_create(&sensor_calibration, tracker_config, &tracker), "Body tracker initialization failed!");

    // Initialize the socket.
    TCPServer tcpServer([&](int errorCode, std::string errorMessage){
        std::cerr << "Socket creation error:" << errorCode << " : " << errorMessage << std::endl;

        // close the camera.
        if ((size_t) tracker->_rsvd != 0) k4abt_tracker_shutdown(tracker);
        if ((size_t) tracker->_rsvd != 0) k4abt_tracker_destroy(tracker);
        if ((size_t) device->_rsvd != 0) k4a_device_stop_cameras(device);
        if ((size_t) device->_rsvd != 0) k4a_device_close(device);

        exit(EXIT_FAILURE);
    });

    // When a new client connected:
    std::list<TCPSocket*> tcpClients;
    tcpServer.onNewConnection = [&tcpClients](TCPSocket *newClient) {
        std::cout << "New client: [" << newClient->remoteAddress() << ":" << newClient->remotePort() << "]" << std::endl << std::flush;
        tcpClients.push_back(newClient);

        newClient->onSocketClosed = [newClient, &tcpClients](int errorCode) {
            std::cout << "Socket closed:" << newClient->remoteAddress() << ":" << newClient->remotePort() << " -> " << errorCode << std::endl;

            tcpClients.remove(newClient);

            std::cout << "Currently opended sockets: ";
            for (std::list<TCPSocket*>::iterator it= tcpClients.begin(); it != tcpClients.end(); ++it) {
                std::cout << (*it)->remoteAddress() << ":" << (*it)->remotePort() << " ";
            }
            std::cout << '\n' << std::flush;
        };
    };
    // Bind the server to a port.
    tcpServer.Bind(8888, [&](int errorCode, std::string errorMessage) {
        // BINDING FAILED:
        std::cerr << errorCode << " : " << errorMessage << std::endl;
        tcpServer.Close();

        // close the camera.
        if ((size_t) tracker->_rsvd != 0) k4abt_tracker_shutdown(tracker);
        if ((size_t) tracker->_rsvd != 0) k4abt_tracker_destroy(tracker);
        if ((size_t) device->_rsvd != 0) k4a_device_stop_cameras(device);
        if ((size_t) device->_rsvd != 0) k4a_device_close(device);

        exit(EXIT_FAILURE);
    });
    // Start Listening the server.
    tcpServer.Listen([&](int errorCode, std::string errorMessage) {
        // LISTENING FAILED:
        std::cerr << errorCode << " : " << errorMessage << std::endl;

        if (errorCode == SOCKET_ERROR) { // fail to listen(...)
            tcpServer.Close();

            // close the camera.
            if ((size_t) tracker->_rsvd != 0) k4abt_tracker_shutdown(tracker);
            if ((size_t) tracker->_rsvd != 0) k4abt_tracker_destroy(tracker);
            if ((size_t) device->_rsvd != 0) k4a_device_stop_cameras(device);
            if ((size_t) device->_rsvd != 0) k4a_device_close(device);

            exit(EXIT_FAILURE);
        }
    });

    int frame_count = 0;
    do
    {
        k4a_capture_t sensor_capture;
        k4a_wait_result_t get_capture_result = k4a_device_get_capture(device, &sensor_capture, K4A_WAIT_INFINITE);
        if (get_capture_result == K4A_WAIT_RESULT_SUCCEEDED)
        {
            frame_count++;
            k4a_wait_result_t queue_capture_result = k4abt_tracker_enqueue_capture(tracker, sensor_capture, K4A_WAIT_INFINITE);
            k4a_capture_release(sensor_capture); // Remember to release the sensor capture once you finish using it
            if (queue_capture_result == K4A_WAIT_RESULT_TIMEOUT)
            {
                // It should never hit timeout when K4A_WAIT_INFINITE is set.
                printf("Error! Add capture to tracker process queue timeout!\n");
                break;
            }
            else if (queue_capture_result == K4A_WAIT_RESULT_FAILED)
            {
                printf("Error! Add capture to tracker process queue failed!\n");
                break;
            }

            k4abt_frame_t body_frame = NULL;
            k4a_wait_result_t pop_frame_result = k4abt_tracker_pop_result(tracker, &body_frame, K4A_WAIT_INFINITE);
            if (pop_frame_result == K4A_WAIT_RESULT_SUCCEEDED)
            {
                // Successfully popped the body tracking result. Start your processing

                std::string msg = GestureTracker::createMessage(body_frame);
                printf("In frame %d, the message is... %s\n", frame_count, msg.c_str());
                
                // Send the message to clients.
                for(TCPSocket* client : tcpClients) {
                    client->Send(msg);
                }

                k4abt_frame_release(body_frame); // Remember to release the body frame once you finish using it
            }
            else if (pop_frame_result == K4A_WAIT_RESULT_TIMEOUT)
            {
                //  It should never hit timeout when K4A_WAIT_INFINITE is set.
                printf("Error! Pop body frame result timeout!\n");
                break;
            }
            else
            {
                printf("Pop body frame result failed!\n");
                break;
            }
        }
        else if (get_capture_result == K4A_WAIT_RESULT_TIMEOUT)
        {
            // It should never hit time out when K4A_WAIT_INFINITE is set.
            printf("Error! Get depth frame time out!\n");
            break;
        }
        else
        {
            printf("Get depth capture returned error: %d\n", get_capture_result);
            break;
        }

        fflush(stdout);
    } while (frame_count < 2000 && !(GetAsyncKeyState(VK_CONTROL) && GetAsyncKeyState(0x51)));

    printf("Finished body tracking processing!\n");

    // Close the server before exiting the program.
    tcpServer.Close();

    // close the camera.
    if ((size_t) tracker->_rsvd != 0) k4abt_tracker_shutdown(tracker);
    if ((size_t) tracker->_rsvd != 0) k4abt_tracker_destroy(tracker);
    if ((size_t) device->_rsvd != 0) k4a_device_stop_cameras(device);
    if ((size_t) device->_rsvd != 0) k4a_device_close(device);

    return 0;
}