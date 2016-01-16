/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/*
 * Copyright (c) 2011 University of California, Los Angeles
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
 * Author: Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 *
 */

#include "ndn-face.h"

#include "ns3/packet.h"
#include "ns3/log.h"
#include "ns3/node.h"
#include "ns3/assert.h"
#include "ns3/uinteger.h"
#include "ns3/double.h"
#include "ns3/boolean.h"
#include "ns3/simulator.h"
#include "ns3/random-variable.h"
#include "ns3/pointer.h"

#include "ns3/ndn-header-helper.h"
#include "ns3/ndnSIM/utils/ndn-fw-hop-count-tag.h"
 //20160114 start
#include "ns3/ndnSIM/utils/ndn-fw-max-betw-tag.h"
#include "ns3/ndnSIM/utils/ndn-fw-min-betw-tag.h" 
#include <math.h>
#include "ns3/ndn-interest.h"
#include "ns3/ndn-data.h"
#include <fstream>
#include <iostream>
 //20160114 end
 //20160115 maxhop start
#include "ns3/ndnSIM/utils/ndn-fw-max-hop-tag.h"
 //20160115 maxhop end
 //20160116 replacerate start
#include "ns3/ndnSIM/utils/ndn-fw-max-replacerate-tag.h"
#include "ns3/ndnSIM/utils/ndn-fw-min-replacerate-tag.h" 
 //20160116 replacerate end
#include "ns3/ndn-wire.h"

#include <boost/ref.hpp>

NS_LOG_COMPONENT_DEFINE ("ndn.Face");

namespace ns3 {
namespace ndn {

NS_OBJECT_ENSURE_REGISTERED (Face);

//20160114 start
using namespace std;
//20160114 end

TypeId
Face::GetTypeId ()
{
  static TypeId tid = TypeId ("ns3::ndn::Face")
    .SetParent<Object> ()
    .SetGroupName ("Ndn")
    .AddAttribute ("Id", "Face id (unique integer for the Ndn stack on this node)",
                   TypeId::ATTR_GET, // allow only getting it.
                   UintegerValue (0),
                   MakeUintegerAccessor (&Face::m_id),
                   MakeUintegerChecker<uint32_t> ())
    ;
  return tid;
}

/**
 * By default, Ndn face are created in the "down" state
 *  with no IP addresses.  Before becoming useable, the user must
 * invoke SetUp on them once an Ndn address and mask have been set.
 */
Face::Face (Ptr<Node> node)
  : m_node (node)
  , m_upstreamInterestHandler (MakeNullCallback< void, Ptr<Face>, Ptr<Interest> > ())
  , m_upstreamDataHandler (MakeNullCallback< void, Ptr<Face>, Ptr<Data> > ())
  , m_ifup (false)
  , m_id ((uint32_t)-1)
  , m_metric (0)
  , m_flags (0)
{
  NS_LOG_FUNCTION (this << node);

  NS_ASSERT_MSG (node != 0, "node cannot be NULL. Check the code");
}

Face::~Face ()
{
  NS_LOG_FUNCTION_NOARGS ();
}

Face::Face (const Face &)
{
}

Face& Face::operator= (const Face &)
{
  return *this;
}

Ptr<Node>
Face::GetNode () const
{
  return m_node;
}

void
Face::RegisterProtocolHandlers (const InterestHandler &interestHandler, const DataHandler &dataHandler)
{
  NS_LOG_FUNCTION_NOARGS ();

  m_upstreamInterestHandler = interestHandler;
  m_upstreamDataHandler = dataHandler;
}

void
Face::UnRegisterProtocolHandlers ()
{
  NS_LOG_FUNCTION_NOARGS ();

  m_upstreamInterestHandler = MakeNullCallback< void, Ptr<Face>, Ptr<Interest> > ();
  m_upstreamDataHandler = MakeNullCallback< void, Ptr<Face>, Ptr<Data> > ();
}


bool
Face::SendInterest (Ptr<const Interest> interest)
{
  NS_LOG_FUNCTION (this << boost::cref (*this) << interest->GetName ());

//20160114 start
  string a = interest->GetName().toUri();
  std::cout << "  a  " << a <<std::endl;
  //20160114 end


  if (!IsUp ())
    {
      return false;
    }
//20160114 start
    DisInterestData = 20 ;
//20160114 end
  return Send (Wire::FromInterest (interest));
}

bool
Face::SendData (Ptr<const Data> data)
{
  NS_LOG_FUNCTION (this << data);

  if (!IsUp ())
    {
      return false;
    }
//20160114 start
   DisInterestData = 40 ; 
//20160114 end
  return Send (Wire::FromData (data));
}

bool
Face::Send (Ptr<Packet> packet)
{
  FwHopCountTag hopCount;
  bool tagExists = packet->RemovePacketTag (hopCount);
  if (tagExists)
    {
      hopCount.Increment ();
      packet->AddPacketTag (hopCount);
    }
    //20160115 maxhop start
    FwMaxHopTag maxHopTag;
   bool tagMaxHop = packet->RemovePacketTag (maxHopTag);
  if (tagMaxHop)
    {
      if( DisInterestData == 20 )
      {
        maxHopTag.Increment ();
      }
      
      packet->AddPacketTag (maxHopTag);
    }
    //20160115 maxhop end

    //20160114 start
    FwMaxBetwTag maxBetwTag;
    bool tagMax = packet->RemovePacketTag (maxBetwTag);
  if (tagMax)
    {
      if((maxBetwTag.GetMax() < GetNode()->GetBetw()) &&(DisInterestData == 20))
      {
        maxBetwTag.SetMaxBetw(GetNode()->GetBetw());
      }
      packet->AddPacketTag (maxBetwTag);
    }
     FwMinBetwTag minBetwTag;
    bool tagMin = packet->RemovePacketTag (minBetwTag);
  if (tagMin)
    {
      if((minBetwTag.GetMin() > GetNode()->GetBetw()) &&(DisInterestData == 20))
      {
        minBetwTag.SetMinBetw(GetNode()->GetBetw());
      }
      packet->AddPacketTag (minBetwTag);
    }
  //20160114 end

    //20160116 replacerate start
     FwMaxReplacerateTag maxReplacerateTag;
    bool tagMaxReplacerate = packet->RemovePacketTag (maxReplacerateTag);
  if (tagMaxReplacerate)
    {
      if((maxReplacerateTag.GetMax() < GetNode()->GetReplaceRate()) &&(DisInterestData == 20))
      {
        maxReplacerateTag.SetMaxReplacerate(GetNode()->GetReplaceRate());
      }
      packet->AddPacketTag (maxReplacerateTag);
    }

    FwMinReplacerateTag minReplacerateTag;
    bool tagMinReplacerate = packet->RemovePacketTag (minReplacerateTag);
  if (tagMinReplacerate)
    {
      if((minReplacerateTag.GetMin() > GetNode()->GetReplaceRate()) && (DisInterestData == 20))
      {
        minReplacerateTag.SetMinReplacerate(GetNode()->GetReplaceRate());
      }
      packet->AddPacketTag (minReplacerateTag);
    }

    
    //20160116 replacerate end


  return true;
}

bool
Face::Receive (Ptr<const Packet> p)
{
  NS_LOG_FUNCTION (this << p << p->GetSize ());

  if (!IsUp ())
    {
      // no tracing here. If we were off while receiving, we shouldn't even know that something was there
      return false;
    }

  Ptr<Packet> packet = p->Copy (); // give upper layers a rw copy of the packet
  try
    {
      HeaderHelper::Type type = HeaderHelper::GetNdnHeaderType (packet);
      switch (type)
        {
        case HeaderHelper::INTEREST_NDNSIM:
          return ReceiveInterest (Wire::ToInterest (packet, Wire::WIRE_FORMAT_NDNSIM));
        case HeaderHelper::INTEREST_CCNB:
          return ReceiveInterest (Wire::ToInterest (packet, Wire::WIRE_FORMAT_CCNB));
        case HeaderHelper::CONTENT_OBJECT_NDNSIM:
          return ReceiveData (Wire::ToData (packet, Wire::WIRE_FORMAT_NDNSIM));
        case HeaderHelper::CONTENT_OBJECT_CCNB:
          return ReceiveData (Wire::ToData (packet, Wire::WIRE_FORMAT_CCNB));
        default:
          NS_FATAL_ERROR ("Not supported NDN header");
          return false;
        }

      // exception will be thrown if packet is not recognized
    }
  catch (UnknownHeaderException)
    {
      NS_FATAL_ERROR ("Unknown NDN header. Should not happen");
      return false;
    }

  return false;
}

bool
Face::ReceiveInterest (Ptr<Interest> interest)
{
  if (!IsUp ())
    {
      // no tracing here. If we were off while receiving, we shouldn't even know that something was there
      return false;
    }

  m_upstreamInterestHandler (this, interest);
  return true;
}

bool
Face::ReceiveData (Ptr<Data> data)
{
  if (!IsUp ())
    {
      // no tracing here. If we were off while receiving, we shouldn't even know that something was there
      return false;
    }

  m_upstreamDataHandler (this, data);
  return true;
}

void
Face::SetMetric (uint16_t metric)
{
  NS_LOG_FUNCTION (metric);
  m_metric = metric;
}

uint16_t
Face::GetMetric (void) const
{
  return m_metric;
}

void
Face::SetFlags (uint32_t flags)
{
  m_flags = flags;
}

bool
Face::operator== (const Face &face) const
{
  NS_ASSERT_MSG (m_node->GetId () == face.m_node->GetId (),
                 "Faces of different nodes should not be compared to each other: " << *this << " == " << face);

  return (m_id == face.m_id);
}

bool
Face::operator< (const Face &face) const
{
  NS_ASSERT_MSG (m_node->GetId () == face.m_node->GetId (),
                 "Faces of different nodes should not be compared to each other: " << *this << " == " << face);

  return (m_id < face.m_id);
}

std::ostream&
Face::Print (std::ostream &os) const
{
  os << "id=" << GetId ();
  return os;
}

std::ostream&
operator<< (std::ostream& os, const Face &face)
{
  face.Print (os);
  return os;
}

} // namespace ndn
} // namespace ns3

