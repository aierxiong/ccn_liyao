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
 *         Alexander Afanasyev <alexander.afanasyev@ucla.edu>
 */

#ifndef NDN_CONSUMER_CBR_H
#define NDN_CONSUMER_CBR_H

#include "ndn-consumer.h"

namespace ns3 {
namespace ndn {

/**
 * @ingroup ndn-apps
 * @brief Ndn application for sending out Interest packets at a "constant" rate (Poisson process)
 */
class ConsumerCbr: public Consumer
{
public: 
  static TypeId GetTypeId ();
        
  /**
   * \brief Default constructor 
   * Sets up randomizer function and packet sequence number
   */
  ConsumerCbr ();
  virtual ~ConsumerCbr ();

  //20160114 start  zipf
  void
  SetNumberOfContents (uint32_t numOfContents);

  uint32_t
  GetNumberOfContents () const;

  void
  SetQ (double q);

  double
  GetQ () const;

  void
  SetS (double s);

  double
  GetS () const;

  //20160114 end zipf

  //20160114 start zipf
  uint32_t GetNextSeq();
   //20160114 end zipf


  // From NdnApp
  // virtual void
  // OnInterest (const Ptr<const Interest> &interest);

  // virtual void
  // OnNack (const Ptr<const Interest> &interest);

  // virtual void
  // OnData (const Ptr<const Data> &contentObject,
  //                  const Ptr<const Packet> &payload);

protected:
  /**
   * \brief Constructs the Interest packet and sends it using a callback to the underlying NDN protocol
   */
  virtual void
  ScheduleNextPacket ();

  /**
   * @brief Set type of frequency randomization
   * @param value Either 'none', 'uniform', or 'exponential'
   */
  void
  SetRandomize (const std::string &value);

  /**
   * @brief Get type of frequency randomization
   * @returns either 'none', 'uniform', or 'exponential'
   */
  std::string
  GetRandomize () const;
  
private:
  // void
  // UpdateMean ();

  // virtual void
  // SetPayloadSize (uint32_t payload);

  // void
  // SetDesiredRate (DataRate rate);

  // DataRate
  // GetDesiredRate () const;
  
protected:
  double              m_frequency; // Frequency of interest packets (in hertz)
  bool                m_firstTime;
  RandomVariable      *m_random;
  std::string         m_randomType;
  //20160114 start zipf
  private:
  uint32_t m_N;  //number of the contents
  double m_q;  //q in (k+q)^s
  double m_s;  //s in (k+q)^s
  std::vector<double> m_Pcum;  //cumulative probability
  UniformVariable m_SeqRng; //RNG
  //20160114 end zipf
};

} // namespace ndn
} // namespace ns3

#endif
