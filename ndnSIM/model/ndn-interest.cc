/* -*- Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil -*- */
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
 */

 

#include "ndn-interest.h"

#include "ns3/log.h"
#include "ns3/unused.h"
#include "ns3/packet.h"

//liyao20150709
 // #include "ns3/ndn-consumer-zipf-mandelbrot.h"
 //liyao20150709 end


NS_LOG_COMPONENT_DEFINE ("ndn.Interest");

namespace ns3 {
namespace ndn {

Interest::Interest (Ptr<Packet> payload/* = Create<Packet> ()*/)
  : m_name ()
  , m_scope (0xFF)
  , m_interestLifetime (Seconds (0))
  , m_nonce (0)
  , m_nackType (NORMAL_INTEREST)
  , m_exclude (0)
  , m_payload (payload)
  , m_wire (0)
  //edit start
  , m_preference(0.0)

  //edit end

  //liyao20160107start
  // ,m_hopcountmax(0.0)
  //liyao20160107end

  //liyao20150709 
  ,m_sequence(0.0)
{
  if (m_payload == 0) // just in case
    {
      m_payload = Create<Packet> ();
    }
}

Interest::Interest (const Interest &interest)
  : m_name             (Create<Name> (interest.GetName ()))
  , m_scope            (interest.m_scope)
  , m_interestLifetime (interest.m_interestLifetime)
  , m_nonce            (interest.m_nonce)
  , m_nackType         (interest.m_nackType)
  , m_exclude          (interest.m_exclude ? Create<Exclude> (*interest.GetExclude ()) : 0)
  , m_payload          (interest.GetPayload ()->Copy ())
  , m_wire             (0)
  //edit start
  ,m_preference        (interest.m_preference)

  //edit end

  //liyao20150709
  ,m_sequence          (interest.m_sequence) 

  //liyao20150709 end

  //liyao20160107start
  // ,m_hopcountmax       (interest.m_hopcountmax)
  //liyao20160107end
{
  NS_LOG_FUNCTION ("correct copy constructor");
}

void
Interest::SetName (Ptr<Name> name)
{
  m_name = name;
  m_wire = 0;
}

void
Interest::SetName (const Name &name)
{
  m_name = Create<Name> (name);
  m_wire = 0;
}

const Name&
Interest::GetName () const
{
  if (m_name==0) throw InterestException();
  return *m_name;
}

Ptr<const Name>
Interest::GetNamePtr () const
{
  return m_name;
}

void
Interest::SetScope (int8_t scope)
{
  m_scope = scope;
  m_wire = 0;
}

int8_t
Interest::GetScope () const
{
  return m_scope;
}

void
Interest::SetInterestLifetime (Time lifetime)
{
  m_interestLifetime = lifetime;
  m_wire = 0;
}

Time
Interest::GetInterestLifetime () const
{
  return m_interestLifetime;
}

void
Interest::SetNonce (uint32_t nonce)
{
  m_nonce = nonce;
  m_wire = 0;
}

uint32_t
Interest::GetNonce () const
{
  return m_nonce;
}

//liyao20150709

 void
 Interest::SetSeq (double sequence)
 {
  m_sequence=sequence;
  m_wire = 0;
 }

 double
 Interest::GetSeq () const
 {
  // ns3::ndn::ConsumerZipfMandelbrot zipf;
  // m_sequence=zipf.sequence;

  return m_sequence ;
 }

  //liyao20150709 end

void
Interest::SetNack (uint8_t nackType)
{
  m_nackType = nackType;
  m_wire = 0;
}

uint8_t
Interest::GetNack () const
{
  return m_nackType;
}

void
Interest::SetExclude (Ptr<Exclude> exclude)
{
  m_exclude = exclude;
  m_wire = 0;
}

Ptr<const Exclude>
Interest::GetExclude () const
{
  return m_exclude;
}

void
Interest::SetPayload (Ptr<Packet> payload)
{
  m_payload = payload;
  m_wire = 0;
}

Ptr<const Packet>
Interest::GetPayload () const
{
  return m_payload;
}

//edit start
void
Interest::SetPreference (double preference)
{
  m_preference = preference;
}

double
Interest::GetPreference () const
{
  return m_preference;
}

//edit end

//liyao20160107start
// void
// Interest::SetHopmax(double hopcountmax);
// {
//   m_hopcountmax = hopcountmax;
// }

// double
// Interest::GetHopmax () const
// {
//   return m_hopcountmax;
// }

//liyao20160107end

void
Interest::Print (std::ostream &os) const
{
  os << "I: " << GetName ();
  
  return;
  os << "<Interest>\n  <Name>" << GetName () << "</Name>\n";
  if (GetNack ()>0)
    {
      os << "  <NACK>";
      switch (GetNack ())
        {
        case NACK_LOOP:
          os << "loop";
          break;
        case NACK_CONGESTION:
          os << "congestion";
          break;
        default:
          os << "unknown";
          break;
        }
      os << "</NACK>\n";
    }
  os << "  <Scope>" << GetScope () << "</Scope>\n";
  if ( !GetInterestLifetime ().IsZero() )
    os << "  <InterestLifetime>" << GetInterestLifetime () << "</InterestLifetime>\n";
  if (GetNonce ()>0)
    os << "  <Nonce>" << GetNonce () << "</Nonce>\n";
  os << "</Interest>";
}

} // namespace ndn
} // namespace ns3

