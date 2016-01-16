/* -*-  Mode: C++; c-file-style: "gnu"; indent-tabs-mode:nil; -*- */
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
 * Author: Ilya Moiseenko <iliamo@cs.ucla.edu>
 */

#include "ndn-consumer.h"
#include "ns3/ptr.h"
#include "ns3/log.h"
#include "ns3/simulator.h"
#include "ns3/packet.h"
#include "ns3/callback.h"
#include "ns3/string.h"
#include "ns3/boolean.h"
#include "ns3/uinteger.h"
#include "ns3/integer.h"
#include "ns3/double.h"

#include "ns3/ndn-app-face.h"
#include "ns3/ndn-interest.h"
#include "ns3/ndn-data.h"
#include "ns3/ndnSIM/utils/ndn-fw-hop-count-tag.h"
 //20160113  20160115 start
#include "ns3/ndnSIM/utils/ndn-fw-max-betw-tag.h"
#include "ns3/ndnSIM/utils/ndn-fw-min-betw-tag.h" 
#include <math.h>
 //20160113  20150115 end
 //20160115 maxhop start
#include "ns3/ndnSIM/utils/ndn-fw-max-hop-tag.h"
//20160115 maxhop end 
 //20160116 replacerate start
#include "ns3/ndnSIM/utils/ndn-fw-max-replacerate-tag.h"
#include "ns3/ndnSIM/utils/ndn-fw-min-replacerate-tag.h"
//20160116 replacerate end 
#include "ns3/ndnSIM/utils/ndn-rtt-mean-deviation.h"

#include <boost/ref.hpp>

#include "ns3/names.h"

 //20160114 start
#define CLASS_CONTENT_NUM 100
 //20160114 end

NS_LOG_COMPONENT_DEFINE ("ndn.Consumer");

namespace ns3 {
namespace ndn {

NS_OBJECT_ENSURE_REGISTERED (Consumer);

TypeId
Consumer::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::ndn::Consumer")
    .SetGroupName ("Ndn")
    .SetParent<App> ()
    .AddAttribute ("StartSeq", "Initial sequence number",
                   IntegerValue (0),
                   MakeIntegerAccessor(&Consumer::m_seq),
                   MakeIntegerChecker<int32_t>())

    .AddAttribute ("Prefix","Name of the Interest",
                   StringValue ("/"),
                   MakeNameAccessor (&Consumer::m_interestName),
                   MakeNameChecker ())
    .AddAttribute ("LifeTime", "LifeTime for interest packet",
                   StringValue ("2s"),
                   MakeTimeAccessor (&Consumer::m_interestLifeTime),
                   MakeTimeChecker ())

    .AddAttribute ("RetxTimer",
                   "Timeout defining how frequent retransmission timeouts should be checked",
                   StringValue ("50ms"),
                   MakeTimeAccessor (&Consumer::GetRetxTimer, &Consumer::SetRetxTimer),
                   MakeTimeChecker ())

    .AddTraceSource ("LastRetransmittedInterestDataDelay", "Delay between last retransmitted Interest and received Data",
                     MakeTraceSourceAccessor (&Consumer::m_lastRetransmittedInterestDataDelay))

    .AddTraceSource ("FirstInterestDataDelay", "Delay between first transmitted Interest and received Data",
                     MakeTraceSourceAccessor (&Consumer::m_firstInterestDataDelay))
    //20160114 zipf start
    .AddAttribute ("ConsumerNumberOfContents", "Number of the Contents in total",
                   StringValue ("100"),
                   MakeUintegerAccessor (&Consumer::SetNumberOfContents, &Consumer::GetNumberOfContents),
                   MakeUintegerChecker<uint32_t> ())

    .AddAttribute ("Consumer_q", "parameter of improve rank",
                   StringValue ("0.7"),
                   MakeDoubleAccessor (&Consumer::SetQ, &Consumer::GetQ),
                   MakeDoubleChecker<double> ())
    
    .AddAttribute ("Consumer_s", "parameter of power",
                   StringValue ("0.7"),
                   MakeDoubleAccessor (&Consumer::SetS, &Consumer::GetS),
                   MakeDoubleChecker<double> ())
    //20160114 end  zipf
    ;

  return tid;
}

Consumer::Consumer ()
  : m_rand (0, std::numeric_limits<uint32_t>::max ())
  , m_seq (0)
  , m_seqMax (0) // don't request anything
  //20160114 zipf start
  , m_N (100) // needed here to make sure when SetQ/SetS are called, there is a valid value of N
  , m_q (0.7)
  , m_s (0.7)
  , m_SeqRng (0.0, 1.0)
  //20160114 zipf end
{
  NS_LOG_FUNCTION_NOARGS ();

  m_rtt = CreateObject<RttMeanDeviation> ();
}

void
Consumer::SetRetxTimer (Time retxTimer)
{
  m_retxTimer = retxTimer;
  if (m_retxEvent.IsRunning ())
    {
      // m_retxEvent.Cancel (); // cancel any scheduled cleanup events
      Simulator::Remove (m_retxEvent); // slower, but better for memory
    }

  // schedule even with new timeout
  m_retxEvent = Simulator::Schedule (m_retxTimer,
                                     &Consumer::CheckRetxTimeout, this);
}

Time
Consumer::GetRetxTimer () const
{
  return m_retxTimer;
}

void
Consumer::CheckRetxTimeout ()
{
  Time now = Simulator::Now ();

  Time rto = m_rtt->RetransmitTimeout ();
  // NS_LOG_DEBUG ("Current RTO: " << rto.ToDouble (Time::S) << "s");

  while (!m_seqTimeouts.empty ())
    {
      SeqTimeoutsContainer::index<i_timestamp>::type::iterator entry =
        m_seqTimeouts.get<i_timestamp> ().begin ();
      if (entry->time + rto <= now) // timeout expired?
        {
          uint32_t seqNo = entry->seq;
          m_seqTimeouts.get<i_timestamp> ().erase (entry);
          OnTimeout (seqNo);
        }
      else
        break; // nothing else to do. All later packets need not be retransmitted
    }

  m_retxEvent = Simulator::Schedule (m_retxTimer,
                                     &Consumer::CheckRetxTimeout, this);
}

// Application Methods
void
Consumer::StartApplication () // Called at time specified by Start
{
  NS_LOG_FUNCTION_NOARGS ();

  // do base stuff
  App::StartApplication ();

  ScheduleNextPacket ();
}

void
Consumer::StopApplication () // Called at time specified by Stop
{
  NS_LOG_FUNCTION_NOARGS ();

  // cancel periodic packet generation
  Simulator::Cancel (m_sendEvent);

  // cleanup base stuff
  App::StopApplication ();
}

//20160114 zipf start
void
Consumer::SetNumberOfContents (uint32_t numOfContents)
{
  m_N = numOfContents;

  NS_LOG_DEBUG (m_q << " and " << m_s << " and " << m_N);

  m_Pcum = std::vector<double> (m_N + 1);

  m_Pcum[0] = 0.0;
  for (uint32_t i=1; i<=m_N; i++)
    {
      m_Pcum[i] = m_Pcum[i-1] + 1.0 / std::pow(i+m_q, m_s);
    }

  for (uint32_t i=1; i<=m_N; i++)
    {
      m_Pcum[i] = m_Pcum[i] / m_Pcum[m_N];
      NS_LOG_LOGIC ("Cumulative probability [" << i << "]=" << m_Pcum[i]);
  }
}

uint32_t
Consumer::GetNumberOfContents () const
{
  return m_N;
}

void
Consumer::SetQ (double q)
{
  m_q = q;
  SetNumberOfContents (m_N);
}

double
Consumer::GetQ () const
{
  return m_q;
}

void
Consumer::SetS (double s)
{
  m_s = s;
  SetNumberOfContents (m_N);
}

double
Consumer::GetS () const
{
  return m_s;
}


uint32_t
Consumer::GetNextSeq()
{
  uint32_t content_index = 1; //[1, m_N]
  double p_sum = 0;

  double p_random = m_SeqRng.GetValue();
  while (p_random == 0)
    {
      p_random = m_SeqRng.GetValue();
    }
  //if (p_random == 0)

    
  NS_LOG_LOGIC("p_random="<<p_random);
  for (uint32_t i=1; i<=m_N; i++)
    {
      p_sum = m_Pcum[i];   //m_Pcum[i] = m_Pcum[i-1] + p[i], p[0] = 0;   e.g.: p_cum[1] = p[1], p_cum[2] = p[1] + p[2]
      if (p_random <= p_sum)
        {
          content_index = i;
          break;
        } //if
    } //for
  //content_index = 1;
    //liyao20160107
  //   std::cout<<'p_random'<<p_random<<std::endl;
  //   //liyao20150713
  // std::cout<<"zipf   content_index  "<<content_index<<std::endl;
  //end
  NS_LOG_DEBUG("RandomNumber="<<content_index);
  return content_index;
  
}

//20160114  zipf end


//20160114 right start
// void
// Consumer::SendPacket ()
// {
//   if (!m_active) return;

//   NS_LOG_FUNCTION_NOARGS ();

//   uint32_t seq=std::numeric_limits<uint32_t>::max (); //invalid

//   while (m_retxSeqs.size ())
//     {
//       seq = *m_retxSeqs.begin ();
//       m_retxSeqs.erase (m_retxSeqs.begin ());
//       break;
//     }

//   if (seq == std::numeric_limits<uint32_t>::max ())
//     {
//       if (m_seqMax != std::numeric_limits<uint32_t>::max ())
//         {
//           if (m_seq >= m_seqMax)
//             {
//               return; // we are totally done
//             }
//         }

//       seq = m_seq++;
//     }

//     //20160114 start bumingbai
//     // int pp = rand();
//     // seq =(uint32_t) pp % CLASS_CONTENT_NUM ;
//     // std::cout << " seq " << seq <<std::endl;
//     //20160114 end bumingbai 
 
//   //
//   Ptr<Name> nameWithSequence = Create<Name> (m_interestName);
//   nameWithSequence->appendSeqNum (seq);
//   //

//   Ptr<Interest> interest = Create<Interest> ();
//   interest->SetNonce               (m_rand.GetValue ());
//   interest->SetName                (nameWithSequence);
//   interest->SetInterestLifetime    (m_interestLifeTime);
//   // interest->SetPreference          (m_face->GetNode()->GetPreference());

//   // NS_LOG_INFO ("Requesting Interest: \n" << *interest);
//   NS_LOG_INFO ("> Interest for " << seq);

//   WillSendOutInterest (seq);  

//   FwHopCountTag hopCountTag;
//   interest->GetPayload ()->AddPacketTag (hopCountTag);

//   //20160113 start
//   FwMaxBetwTag maxBetwTag;
//   interest->GetPayload ()->AddPacketTag (maxBetwTag);
//   //20160113 end


//   m_transmittedInterests (interest, this, m_face);
//   m_face->ReceiveInterest (interest);

//   ScheduleNextPacket ();
// }
//20160114 right end

//20160114 zipf start
void
Consumer::SendPacket() {
  if (!m_active) return;

  NS_LOG_FUNCTION_NOARGS ();

  uint32_t seq=std::numeric_limits<uint32_t>::max (); //invalid

  // std::cout << Simulator::Now ().ToDouble (Time::S) << "s max -> " << m_seqMax << "\n";

  while (m_retxSeqs.size ())
    {
      seq = *m_retxSeqs.begin ();
      m_retxSeqs.erase (m_retxSeqs.begin ());

      // NS_ASSERT (m_seqLifetimes.find (seq) != m_seqLifetimes.end ());
      // if (m_seqLifetimes.find (seq)->time <= Simulator::Now ())
      //   {

      //     NS_LOG_DEBUG ("Expire " << seq);
      //     m_seqLifetimes.erase (seq); // lifetime expired. Trying to find another unexpired sequence number
      //     continue;
      //   }
      NS_LOG_DEBUG("=interest seq "<<seq<<" from m_retxSeqs");
      break;
    }

  if (seq == std::numeric_limits<uint32_t>::max ()) //no retransmission
    {
      if (m_seqMax != std::numeric_limits<uint32_t>::max ())
        {
          if (m_seq >= m_seqMax)
            {
              return; // we are totally done
            }
        }

      seq = Consumer::GetNextSeq();
      m_seq ++;
    }

  // std::cout << Simulator::Now ().ToDouble (Time::S) << "s -> " << seq << "\n";

  //
  Ptr<Name> nameWithSequence = Create<Name> (m_interestName);
  nameWithSequence->appendSeqNum (seq);
  //

  Ptr<Interest> interest = Create<Interest> ();
  interest->SetNonce (m_rand.GetValue ());
  interest->SetName  (nameWithSequence);

  // NS_LOG_INFO ("Requesting Interest: \n" << *interest);
  NS_LOG_INFO ("> Interest for " << seq<<", Total: "<<m_seq<<", face: "<<m_face->GetId());
  NS_LOG_DEBUG ("Trying to add " << seq << " with " << Simulator::Now () << ". already " << m_seqTimeouts.size () << " items");

  m_seqTimeouts.insert (SeqTimeout (seq, Simulator::Now ()));
  m_seqFullDelay.insert (SeqTimeout (seq, Simulator::Now ()));

  m_seqLastDelay.erase (seq);
  m_seqLastDelay.insert (SeqTimeout (seq, Simulator::Now ()));

  m_seqRetxCounts[seq] ++;

  m_rtt->SentSeq (SequenceNumber32 (seq), 1);

  FwHopCountTag hopCountTag;
  interest->GetPayload ()->AddPacketTag (hopCountTag);

  //20160115 maxhop start
  FwMaxHopTag maxHopTag;
  interest->GetPayload ()->AddPacketTag (maxHopTag);
  //20160115 maxhop end

  //20160113 start betw
  FwMaxBetwTag maxBetwTag;
  interest->GetPayload ()->AddPacketTag (maxBetwTag);
  FwMinBetwTag minBetwTag;
  interest->GetPayload ()->AddPacketTag (minBetwTag);
  //20160113 end betw

  //20160116 replacerate start
  FwMaxReplacerateTag maxReplacerateTag;
  interest->GetPayload ()->AddPacketTag (maxReplacerateTag);
  FwMinReplacerateTag minReplacerateTag;
  interest->GetPayload ()->AddPacketTag (minReplacerateTag);
  //20160116 replacerate end

  m_transmittedInterests (interest, this, m_face);
  m_face->ReceiveInterest (interest);

  ScheduleNextPacket ();
}

///////////////////////////////////////////////////
//          Process incoming packets             //
///////////////////////////////////////////////////


void
Consumer::OnData (Ptr<const Data> data)
{
  if (!m_active) return;

  App::OnData (data); // tracing inside

  NS_LOG_FUNCTION (this << data);

  // NS_LOG_INFO ("Received content object: " << boost::cref(*data));

  uint32_t seq = data->GetName ().get (-1).toSeqNum ();
  NS_LOG_INFO ("< DATA for " << seq);

  int hopCount = -1;
  FwHopCountTag hopCountTag;
  if (data->GetPayload ()->PeekPacketTag (hopCountTag))
    {
      hopCount = hopCountTag.Get ();
    }

  SeqTimeoutsContainer::iterator entry = m_seqLastDelay.find (seq);
  if (entry != m_seqLastDelay.end ())
    {
      m_lastRetransmittedInterestDataDelay (this, seq, Simulator::Now () - entry->time, hopCount);
    }

  entry = m_seqFullDelay.find (seq);
  if (entry != m_seqFullDelay.end ())
    {
      m_firstInterestDataDelay (this, seq, Simulator::Now () - entry->time, m_seqRetxCounts[seq], hopCount);
    }

  m_seqRetxCounts.erase (seq);
  m_seqFullDelay.erase (seq);
  m_seqLastDelay.erase (seq);

  m_seqTimeouts.erase (seq);
  m_retxSeqs.erase (seq);

  m_rtt->AckSeq (SequenceNumber32 (seq));
}

void
Consumer::OnNack (Ptr<const Interest> interest)
{
  if (!m_active) return;

  App::OnNack (interest); // tracing inside

  // NS_LOG_DEBUG ("Nack type: " << interest->GetNack ());

  // NS_LOG_FUNCTION (interest->GetName ());

  // NS_LOG_INFO ("Received NACK: " << boost::cref(*interest));
  uint32_t seq = interest->GetName ().get (-1).toSeqNum ();
  NS_LOG_INFO ("< NACK for " << seq);
  // std::cout << Simulator::Now ().ToDouble (Time::S) << "s -> " << "NACK for " << seq << "\n";

  // put in the queue of interests to be retransmitted
  // NS_LOG_INFO ("Before: " << m_retxSeqs.size ());
  m_retxSeqs.insert (seq);
  // NS_LOG_INFO ("After: " << m_retxSeqs.size ());

  m_seqTimeouts.erase (seq);

  m_rtt->IncreaseMultiplier ();             // Double the next RTO ??
  ScheduleNextPacket ();
}

void
Consumer::OnTimeout (uint32_t sequenceNumber)
{
  NS_LOG_FUNCTION (sequenceNumber);
  // std::cout << Simulator::Now () << ", TO: " << sequenceNumber << ", current RTO: " << m_rtt->RetransmitTimeout ().ToDouble (Time::S) << "s\n";

  m_rtt->IncreaseMultiplier ();             // Double the next RTO
  m_rtt->SentSeq (SequenceNumber32 (sequenceNumber), 1); // make sure to disable RTT calculation for this sample
  m_retxSeqs.insert (sequenceNumber);
  ScheduleNextPacket ();
}

void
Consumer::WillSendOutInterest (uint32_t sequenceNumber)
{
  NS_LOG_DEBUG ("Trying to add " << sequenceNumber << " with " << Simulator::Now () << ". already " << m_seqTimeouts.size () << " items");

  m_seqTimeouts.insert (SeqTimeout (sequenceNumber, Simulator::Now ()));
  m_seqFullDelay.insert (SeqTimeout (sequenceNumber, Simulator::Now ()));

  m_seqLastDelay.erase (sequenceNumber);
  m_seqLastDelay.insert (SeqTimeout (sequenceNumber, Simulator::Now ()));

  m_seqRetxCounts[sequenceNumber] ++;

  m_rtt->SentSeq (SequenceNumber32 (sequenceNumber), 1);
}


} // namespace ndn
} // namespace ns3
