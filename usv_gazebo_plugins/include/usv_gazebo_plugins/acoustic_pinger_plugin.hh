/*
 * Copyright (C) 2020 Open Source Robotics Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
*/

#ifndef VRX_GAZEBO_ACOUSTIC_PINGER_PLUGIN_HH_
#define VRX_GAZEBO_ACOUSTIC_PINGER_PLUGIN_HH_

#include <geometry_msgs/Vector3.h>
#include <ros/ros.h>
#include <memory>
#include <mutex>
#include <string>
#include <gazebo/common/Events.hh>
#include <gazebo/common/Plugin.hh>
#include <gazebo/common/Time.hh>
#include <gazebo/physics/physics.hh>
#include <gazebo/sensors/GaussianNoiseModel.hh>
#include <ignition/math/Vector3.hh>

namespace gazebo
{
/// \brief Implements a simulated range and bearing pinger localisation system
///
/// Implements a range and bearing pinger system.  This assumes that the pinger
/// localisation has a mechanism for estimating the range and bearing
/// of the pinger.  Pinger estimates are published using a custom message to the
/// ROS system along with a standard header.  This should allow the tf library
/// to transform the sensor reading between frames.
///
/// Accepts the following SDF parameters:
/// <robotNamespace> - Set the namespace of the robot. Used to setup the ROS
///   nodehandle.
/// <frameId> - Tf frame of the sensor message. Used as part of the sensor
///   message publication.
/// <topicName> - Name of the topic that the sensor message will be published on
/// <setPositionTopicName> - Name of the topic that is used to set the position
///   of the simulated pinger sensor.
/// <position> - Position of the simulated pinger. Defaults to origin.
/// <updateRate> - Rate of simulated sensor messages.
/// <rangeNoise> - Noise model for the range to the simulated pinger.
/// <bearingNoise> - Noise model for the bearing to the simulated pinger.
/// <elevationNoise> - Noise model for the elevation to the simulated pinger.
class AcousticPinger : public ModelPlugin
{
  /// \brief Constructor.
  public: AcousticPinger();

  /// \brief Destructor.
  public: virtual ~AcousticPinger();

  // Documentation inherited.
  public: void Load(physics::ModelPtr _parent,
                    sdf::ElementPtr _sdf);

  /// \brief Callback used by gazebo to update the plugin.
  protected: virtual void UpdateChild();

  /// \brief Callback function called when receiving a new pinger position
  /// via the pinger subscription callback.
  /// \param[in] _pos New pinger position.
  public: void PingerPositionCallback(
    const geometry_msgs::Vector3ConstPtr &_pos);

  // ROS integration
  /// \brief Nodehandle used to integrate with the ROS system.
  private: std::unique_ptr<ros::NodeHandle> rosNodeHandle;

  /// \brief Subscribes to the topic that set the pinger position.
  public: ros::Subscriber setPositionSub;

  /// \brief Publisher used to send sensor messages generated by the plugin.
  private: ros::Publisher rangeBearingPub;

  /// \brief Mutex to protect the position vector.
  public: std::mutex mutex;

  /// \brief Pointer to model object.
  private: physics::ModelPtr model;

  /// \brief Vector storing the position of the pinger.
  private: ignition::math::Vector3d position;

  // Variables that contain parameters of sensor simulation.
  /// \brief String holding the frame id of the sensor.
  private: std::string frameId;

  /// \brief Sensor update rate.
  private: float updateRate;

  /// \brief Variable used to track time of last update.  This is used to
  /// produce data at the correct rate.
  private: common::Time lastUpdateTime;

  /// \brief Pointer used to connect gazebo callback to plugins update function.
  private: event::ConnectionPtr updateConnection;

  // From Brian Bingham's rangebearing_gazebo_plugin.
  /// \brief rangeNoise - Gazebo noise object for range.
  private: gazebo::sensors::NoisePtr rangeNoise = nullptr;

  /// \brief Gazebo noise object for bearing angle.
  private: gazebo::sensors::NoisePtr bearingNoise = nullptr;

  /// \brief Gazebo noise object for elevation angle.
  private: gazebo::sensors::NoisePtr elevationNoise = nullptr;
};
}

#endif
