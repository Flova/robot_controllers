/*
 * Copyright (c) 2020, Michael Ferguson
 * Copyright (c) 2014-2020, Fetch Robotics Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the Fetch Robotics Inc. nor the names of its
 *       contributors may be used to endorse or promote products derived from
 *       this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL FETCH ROBOTICS INC. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

// Author: Michael Ferguson

#ifndef ROBOT_CONTROLLERS_INTERFACE_CONTROLLER_MANAGER_H
#define ROBOT_CONTROLLERS_INTERFACE_CONTROLLER_MANAGER_H

#include <string>
#include <rclcpp/rclcpp.hpp>
#include <rclcpp_action/rclcpp_action.hpp>

#include <robot_controllers_msgs/action/query_controller_states.hpp>

#include <robot_controllers_interface/joint_handle.h>
#include <robot_controllers_interface/gyro_handle.h>
#include <robot_controllers_interface/controller.h>
#include <robot_controllers_interface/controller_loader.h>

namespace robot_controllers
{

/** @brief Base class for a controller manager. */
class ControllerManager
{
  using QueryControllerStates = robot_controllers_msgs::action::QueryControllerStates;
  using QueryControllerStatesGoal = rclcpp_action::ServerGoalHandle<QueryControllerStates>;

  using ControllerList = std::vector<ControllerLoaderPtr>;
  using JointHandleList = std::vector<JointHandlePtr> ;
  using GyroHandleList = std::vector<GyroHandlePtr>;

public:
  ControllerManager();

  /** @brief Ensure proper shutdown with virtual destructor. */
  virtual ~ControllerManager()
  {
  }

  /**
   * @brief Startup the controller manager, loading default controllers.
   * @param nh The proper node handle for finding parameters.
   * @returns 0 if success, negative values are error codes.
   *
   * Note: JointHandles should be added before this is called.
   */
  virtual int init(std::shared_ptr<rclcpp::Node> node);

  /** @brief Start a controller. */
  virtual int requestStart(const std::string& name);

  /** @brief Stop a controller. */
  virtual int requestStop(const std::string& name);

  /** @brief Update active controllers. */
  virtual void update(const rclcpp::Time& time, const rclcpp::Duration& dt);

  /** @brief Reset all controllers. */
  virtual void reset();

  /** @brief Add a joint handle. */
  bool addJointHandle(JointHandlePtr& joint_handle_ptr);

  /** @brief Add a gyro handle. */
  bool addGyroHandle(GyroHandlePtr gyro_hanle_ptr);

  /**
   * @brief Get the handle associated with a particular joint/controller name.
   * @param name The name of the joint/controller.
   */
  HandlePtr getHandle(const std::string& name);

  /**
   * @brief Get the joint handle associated with a particular joint name.
   * @param name The name of the joint.
   *
   * This is mainly a convienence function.
   */
  JointHandlePtr getJointHandle(const std::string& name);

  /**
   * @brief Get the gyro handle associated with a particular gyro name.
   * @param name The name of the gyro.
   *
   * This is mainly a convienence function.
   */
  GyroHandlePtr getGyroHandle(const std::string& name);

private:
  rclcpp_action::GoalResponse handle_goal(
    const rclcpp_action::GoalUUID & uuid,
    std::shared_ptr<const QueryControllerStates::Goal> goal_handle);

  rclcpp_action::CancelResponse handle_cancel(
    const std::shared_ptr<QueryControllerStatesGoal> goal_handle);

  void handle_accepted(const std::shared_ptr<QueryControllerStatesGoal> goal_handle);

  /** @brief Action callback. */
  void execute(const std::shared_ptr<QueryControllerStatesGoal> goal_handle);

  /** @brief Fill in the current state of controllers. */
  void getState(std::shared_ptr<QueryControllerStates::Result> result);

  /** @brief Load a controller. */
  bool load(const std::string& name);

  ControllerList controllers_;
  JointHandleList joints_;
  GyroHandleList gyros_;

  std::shared_ptr<rclcpp::Node> node_;
  rclcpp_action::Server<robot_controllers_msgs::action::QueryControllerStates>::SharedPtr server_;
};

}  // namespace robot_controllers

#endif  // ROBOT_CONTROLLERS_INTERFACE_CONTROLLER_MANAGER_H
