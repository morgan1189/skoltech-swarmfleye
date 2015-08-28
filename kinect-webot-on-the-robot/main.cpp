#include <iostream>
#include "VRobotMotionFacadeImpl.h"
#include "chat_client.cpp"

// Server: 192.168.1.175 port: 42209
// Me: 192.168.1.73

using namespace std;

int main() {
    try
    {
      boost::asio::io_service robot_service;

      boost::asio::io_service io_service;

      tcp::resolver resolver(io_service);
      tcp::resolver::query query("192.168.1.175", "42209");
      tcp::resolver::iterator iterator = resolver.resolve(query);

      chat_client c(io_service, iterator, robot_service);

      boost::thread t(boost::bind(&boost::asio::io_service::run, &io_service));

      boost::asio::io_service::work work(robot_service);
      robot_service.run();

      //robot_service.run();

      /*char line[chat_message::max_body_length + 1];
      while (std::cin.getline(line, chat_message::max_body_length + 1))
      {
        using namespace std; // For strlen and memcpy.
        chat_message msg;
        msg.body_length(strlen(line));
        memcpy(msg.data(), line, msg.body_length());
        c.write(msg);
      }
      c.close();
      t.join();*/
    }
    catch (std::exception& e)
    {
      std::cerr << "Exception: " << e.what() << "\n";
    }

    /*VRobotMotionFacadeImpl * robotMotion = new VRobotMotionFacadeImpl;
    robotMotion->readParameters("/vendor", 1, 0, 0, 0, 1, 0, "IG32E-139K.xml", "IG32E-139K.xml", 0.346, 0.101, 0.6, 1.6);
    robotMotion->connectToDevices();

    robotMotion->setSpeed(50, 0);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    robotMotion->setSpeed(50, 0);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    robotMotion->setSpeed(50, 0);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    robotMotion->setSpeed(50, 0);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    robotMotion->setSpeed(50, 0);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    robotMotion->setSpeed(50, 0);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    robotMotion->setSpeed(50, 0);
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    cout << "Hello World!" << endl;*/
    return 0;
}
