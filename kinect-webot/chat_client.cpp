//
// chat_client.cpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2012 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdlib>
#include <deque>
#include <iostream>
#include <fstream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include "chat_message.hpp"
#include "VRobotMotionFacadeImpl.h"

using boost::asio::ip::tcp;

typedef std::deque<chat_message> chat_message_queue;

class chat_client
{

    const std::string None = "NONE";
    const std::string RaiseRightHand = "RRHD";
    const std::string RaiseLeftHand = "RLHD";
    const std::string Psi = "PSII";
    const std::string Tpose = "TPSE";
    const std::string Stop = "STOP";
    const std::string Wave = "WAVE";
    const std::string Click = "CLCK";
    const std::string SwipeLeft = "SWLT";
    const std::string SwipeRight = "SWRT";
    const std::string SwipeUp = "SWUP";
    const std::string SwipeDown = "SWDN";
    const std::string RightHandCursor = "RHCR";
    const std::string LeftHandCursor = "LHCR";
    const std::string ZoomOut = "ZMOT";
    const std::string ZoomIn = "ZMIN";
    const std::string Wheel = "WHEL";
    const std::string Jump = "JUMP";
    const std::string Squat = "SQUT";
    const std::string Push = "PUSH";
    const std::string Pull = "PULL";

public:
  chat_client(boost::asio::io_service& io_service,
      tcp::resolver::iterator endpoint_iterator,
      boost::asio::io_service& robot_service)
    : io_service_(io_service),
      socket_(io_service),
      robot_service_(robot_service)
  {
    logfile.open("log.txt");

    boost::asio::async_connect(socket_, endpoint_iterator,
        boost::bind(&chat_client::handle_connect, this,
          boost::asio::placeholders::error));
  }

  void write(const chat_message& msg)
  {
    io_service_.post(boost::bind(&chat_client::do_write, this, msg));
  }

  void close()
  {
    io_service_.post(boost::bind(&chat_client::do_close, this));
  }

  void startRobotMotion()
  {
      robot = new VRobotMotionFacadeImpl;
      robot->readParameters("/vendor", 1, 0, 0, 0, 1, 0, "IG32E-139K.xml", "IG32E-139K.xml", 0.346, 0.101, 0.6, 1.6);
      robot->connectToDevices();
  }

  void moveRight()
  {
      robot->setSpeed(0, 50);
      std::this_thread::sleep_for(std::chrono::milliseconds(200));
      robot->setSpeed(0, 50);
      std::this_thread::sleep_for(std::chrono::milliseconds(200));
      robot->setSpeed(0, 50);
      std::this_thread::sleep_for(std::chrono::milliseconds(200));
  }

  void moveLeft()
  {
      robot->setSpeed(0, -50);
      std::this_thread::sleep_for(std::chrono::milliseconds(200));
      robot->setSpeed(0, -50);
      std::this_thread::sleep_for(std::chrono::milliseconds(200));
      robot->setSpeed(0, -50);
      std::this_thread::sleep_for(std::chrono::milliseconds(200));
  }

  void moveForward()
  {
      robot->setSpeed(-50, 0);
      std::this_thread::sleep_for(std::chrono::milliseconds(200));
      robot->setSpeed(-50, 0);
      std::this_thread::sleep_for(std::chrono::milliseconds(200));
      robot->setSpeed(-50, 0);
      std::this_thread::sleep_for(std::chrono::milliseconds(200));
  }

  void moveBack()
  {
      robot->setSpeed(50, 0);
      std::this_thread::sleep_for(std::chrono::milliseconds(200));
      robot->setSpeed(50, 0);
      std::this_thread::sleep_for(std::chrono::milliseconds(200));
      robot->setSpeed(50, 0);
      std::this_thread::sleep_for(std::chrono::milliseconds(200));
  }

private:

  void handle_connect(const boost::system::error_code& error)
  {
    if (!error)
    {
      robot_service_.post(boost::bind(&chat_client::startRobotMotion, this));

      logfile << "Connected" << std::endl;
      boost::asio::async_read(socket_,
          boost::asio::buffer(read_msg_.data(), 4),
          boost::bind(&chat_client::handle_read_message, this,
            boost::asio::placeholders::error));
    }
    else
    {
      logfile << "Error: " << error.message() << std::endl;
    }
  }

  void handle_read_message(const boost::system::error_code& error)
  {
    if (!error)
    {
      logfile << "Received a message" << std::endl;
      logfile << read_msg_.data() << std::endl;
      logfile << "End of message" << std::endl;

      if (std::strstr(read_msg_.data(), SwipeRight.c_str())) {
          logfile << "Swipe right action" << std::endl;
          robot_service_.post(boost::bind(&chat_client::moveRight, this));
      } else if (std::strstr(read_msg_.data(), SwipeLeft.c_str())) {
          logfile << "Swipe left action" << std::endl;
          robot_service_.post(boost::bind(&chat_client::moveLeft, this));
      } else if (std::strstr(read_msg_.data(), SwipeUp.c_str())) {
          logfile << "Swipe up action" << std::endl;
          robot_service_.post(boost::bind(&chat_client::moveBack, this));
      } else if (std::strstr(read_msg_.data(), SwipeDown.c_str())) {
          logfile << "Swipe down action" << std::endl;
          robot_service_.post(boost::bind(&chat_client::moveForward, this));
      }

      boost::asio::async_read(socket_,
          boost::asio::buffer(read_msg_.data(), 4),
          boost::bind(&chat_client::handle_read_message, this,
            boost::asio::placeholders::error));
    }
    else
    {
      logfile << error.message() << std::endl;
      do_close();
    }
  }

  void do_write(chat_message msg)
  {
    bool write_in_progress = !write_msgs_.empty();
    write_msgs_.push_back(msg);
    if (!write_in_progress)
    {
      boost::asio::async_write(socket_,
          boost::asio::buffer(write_msgs_.front().data(),
            write_msgs_.front().length()),
          boost::bind(&chat_client::handle_write, this,
            boost::asio::placeholders::error));
    }
  }

  void handle_write(const boost::system::error_code& error)
  {
    if (!error)
    {
      write_msgs_.pop_front();
      if (!write_msgs_.empty())
      {
        boost::asio::async_write(socket_,
            boost::asio::buffer(write_msgs_.front().data(),
              write_msgs_.front().length()),
            boost::bind(&chat_client::handle_write, this,
              boost::asio::placeholders::error));
      }
    }
    else
    {
      do_close();
    }
  }

  void do_close()
  {
    logfile.close();
    socket_.close();
  }

private:
  VRobotMotionFacadeImpl * robot;
  std::ofstream logfile;
  boost::asio::io_service& robot_service_;
  boost::asio::io_service& io_service_;
  tcp::socket socket_;
  chat_message read_msg_;
  chat_message_queue write_msgs_;
};
