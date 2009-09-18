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
 * \file ap.cpp
 * \brief Ap link subclass.
 * \author Pierre Weiss
 * \date 2009
 */

#include "ap.h"

Ap::Ap(const size_t &indice, const std::string &apNode) : Link(indice)
{
  this->Install(apNode);
  this->setLinkName(std::string("ap_" + this->getIndice()));
  this->setNdcName(std::string("ndc_" + this->getLinkName()));
  this->setAllNodeContainer(std::string("all_" + this->getLinkName()));
  this->setApNode(apNode);
  this->setMobility(false);
  this->setApName(std::string("wifi-default-" + this->getIndice()));
}

Ap::~Ap()
{
}

std::string Ap::getApNode()
{
  return this->m_apNode;
}

void Ap::setApNode(const std::string &apNode)
{
  this->m_apNode = apNode;
}

std::vector<std::string> Ap::GenerateHeader()
{
  std::vector<std::string> headers;
  headers.push_back("#include \"ns3/wifi-module.h\"");
  headers.push_back("#include \"ns3/mobility-module.h\"");
  headers.push_back("#include \"ns3/contrib-module.h\"");

  return headers;
}

std::vector<std::string> Ap::GenerateLink()
{
  std::vector<std::string> generatedLink;
  /* creation of the link. */
  generatedLink.push_back("YansWifiPhyHelper wifiPhy_" + this->getLinkName() + " = YansWifiPhyHelper::Default ();");
  generatedLink.push_back("YansWifiChannelHelper wifiChannel_" + this->getLinkName() + " = YansWifiChannelHelper::Default ();");
  generatedLink.push_back("wifiPhy_" + this->getLinkName() + ".SetChannel (wifiChannel_" + this->getLinkName() + ".Create ());");

  return generatedLink;
}

std::vector<std::string> Ap::GenerateNetDevice()
{
  std::vector<std::string> ndc;

  std::vector<std::string> allNodes = this->GroupAsNodeContainer();//all station nodes !
  for(size_t i = 0; i < (size_t) allNodes.size(); i++)
  {
    ndc.push_back(allNodes.at(i));
  }
  ndc.push_back("NetDeviceContainer " + this->getNdcName() + ";");
  ndc.push_back("Ssid ssid_" + this->getLinkName() + " = Ssid (\"" + this->getApName() + "\");");
  ndc.push_back("WifiHelper wifi_" + this->getLinkName() + " = WifiHelper::Default ();");
  ndc.push_back("NqosWifiMacHelper wifiMac_" + this->getLinkName() + " = NqosWifiMacHelper::Default ();");
  ndc.push_back("wifi_" + this->getLinkName() + ".SetRemoteStationManager (\"ns3::ArfWifiManager\");");

  ndc.push_back("wifiMac_" + this->getLinkName() + ".SetType (\"ns3::NqapWifiMac\", ");
  ndc.push_back("   \"Ssid\", SsidValue (ssid_" + this->getLinkName() + "), ");
  ndc.push_back("   \"BeaconGeneration\", BooleanValue (true),"); 
  ndc.push_back("   \"BeaconInterval\", TimeValue (Seconds (2.5)));");
  ndc.push_back(this->getNdcName() + ".Add(wifi_" + this->getLinkName() + ".Install (wifiPhy_" + this->getLinkName() + ", wifiMac_" + this->getLinkName() + ", " + this->getApNode() + "));");

  ndc.push_back("wifiMac_" + this->getLinkName() + ".SetType (\"ns3::NqstaWifiMac\",");
  ndc.push_back("   \"Ssid\", SsidValue (ssid_" + this->getLinkName() + "), ");
  ndc.push_back("   \"ActiveProbing\", BooleanValue (false));");
  ndc.push_back(this->getNdcName() + ".Add (wifi_" + this->getLinkName() + ".Install (wifiPhy_" + this->getLinkName() + ", wifiMac_" + this->getLinkName() + ", " + this->getAllNodeContainer() + " ));");

  ndc.push_back("MobilityHelper mobility_" + this->getLinkName() + ";");
  ndc.push_back("mobility_" + this->getLinkName() + ".SetMobilityModel (\"ns3::ConstantPositionMobilityModel\");");
  ndc.push_back("mobility_" + this->getLinkName() + ".Install(" + this->getApNode() + ");"); 

  if(this->getMobility())//if random walk is activated.
  {
    ndc.push_back("mobility_" + this->getLinkName() + ".SetMobilityModel (\"ns3::RandomWalk2dMobilityModel\",\"Bounds\", RectangleValue (Rectangle (-50, 50, -50, 50)));");
  }
  ndc.push_back("mobility_" + this->getLinkName() + ".Install(" + this->getAllNodeContainer() + ");");

  return ndc;
}

void Ap::setMobility(const bool &mobility)
{
  this->m_mobility = mobility;
}

bool Ap::getMobility()
{
  return this->m_mobility;
}

void Ap::setApName(const std::string &apName)
{
  this->m_apName = apName;
}

std::string Ap::getApName()
{
  return this->m_apName;
}

std::vector<std::string> Ap::GenerateTrace()
{
  std::vector<std::string> trace;

  if(this->getTrace())
  {
    trace.push_back("wifiPhy_" + this->getLinkName() + ".EnablePcap (\"" + this->getLinkName() + "\", " + this->getNdcName() + ".Get(0));");
  }

  return trace;
}  

