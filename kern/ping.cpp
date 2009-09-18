/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2009 University of Strasbourg
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Pierre Weiss <3weissp@gmail.com>
 */

/**
 * \file ping.cpp
 * \brief Ping Class.
 * \author Pierre Weiss
 * \date 2009
 */

#include "ping.h"
#include "generator.h"

#include "utils.h"

Ping::Ping(const size_t &indice, const std::string &senderNode, const std::string &receiverNode, const size_t &startTime, const size_t &endTime) : Application(indice, senderNode, receiverNode, startTime, endTime)
{
  this->setAppName(std::string("ping_" + this->getIndice()));
}

Ping::~Ping()
{
}

std::vector<std::string> Ping::GenerateHeader()
{
  std::vector<std::string> headers;
  headers.push_back("#include \"ns3/helper-module.h\"");

  return headers;
}

std::vector<std::string> Ping::GenerateApplication(std::string netDeviceContainer, size_t numberIntoNetDevice)
{
  std::vector<std::string> apps;

  apps.push_back("InetSocketAddress dst_" + this->getAppName() + " = InetSocketAddress (iface_" + netDeviceContainer + ".GetAddress(" + utils::toString(numberIntoNetDevice) + "));");
  apps.push_back("OnOffHelper onoff_" + this->getAppName() + " = OnOffHelper (\"ns3::Ipv4RawSocketFactory\", dst_" + this->getAppName() + ");");
  apps.push_back("onoff_" + this->getAppName() + ".SetAttribute (\"OnTime\", RandomVariableValue (ConstantVariable (1.0)));");
  apps.push_back("onoff_" + this->getAppName() + ".SetAttribute (\"OffTime\", RandomVariableValue (ConstantVariable (0.0)));");

  apps.push_back("ApplicationContainer apps_" + this->getAppName() + " = onoff_" + this->getAppName() + ".Install(" + this->getSenderNode() + ".Get(0));");
  apps.push_back("apps_" + this->getAppName() + ".Start (Seconds (" + this->getStartTime() + ".1));");
  apps.push_back("apps_" + this->getAppName() + ".Stop (Seconds (" + this->getEndTime() + ".1));");

  apps.push_back("PacketSinkHelper sink_" + this->getAppName() + " = PacketSinkHelper (\"ns3::Ipv4RawSocketFactory\", dst_" + this->getAppName() + ");");
  apps.push_back("apps_" + this->getAppName() + " = sink_" + this->getAppName() + ".Install (" + this->getReceiverNode() + ".Get(0));");
  apps.push_back("apps_" + this->getAppName() + ".Start (Seconds (" + this->getStartTime() + ".0));");
  apps.push_back("apps_" + this->getAppName() + ".Stop (Seconds (" + this->getEndTime() + ".2));");

  apps.push_back("V4PingHelper ping_" + this->getAppName() + " = V4PingHelper(iface_" + netDeviceContainer + ".GetAddress(" + utils::toString(numberIntoNetDevice) + "));");
  apps.push_back("apps_" + this->getAppName() + " = ping_" + this->getAppName() + ".Install(" + this->getSenderNode() + ".Get(0));");
  apps.push_back("apps_" + this->getAppName() + ".Start (Seconds (" + this->getStartTime() + ".2));");
  apps.push_back("apps_" + this->getAppName() + ".Stop (Seconds (" + this->getEndTime() + ".0));");

  return apps;
}

