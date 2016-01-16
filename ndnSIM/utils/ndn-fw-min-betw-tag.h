#ifndef NDN_FW_MIN_BETW_TAG_H
#define NDN_FW_MIN_BETW_TAG_H

#include "ns3/tag.h"

namespace ns3 {
namespace ndn {

/**
 * @ingroup ndn-fw
 * @brief Packet tag that is used to track hop count for Interest-Data pairs
 */
class FwMinBetwTag : public Tag
{
public:
  static TypeId
  GetTypeId (void);

  /**
   * @brief Default constructor
   */
  FwMinBetwTag () : minBetw (5)
 { 
 };

  /**
   * @brief Destructor
   */
  ~FwMinBetwTag () { }

  /**
   * @brief Increment hop count
   */
  void
  SetMinBetw (double Betw) 
{
 minBetw=Betw;
 }

  /**
   * @brief Get value of hop count
   */
  double
  GetMin () const { return minBetw; }

  ////////////////////////////////////////////////////////
  // from ObjectBase
  ////////////////////////////////////////////////////////
  virtual TypeId
  GetInstanceTypeId () const;
  
  ////////////////////////////////////////////////////////
  // from Tag
  ////////////////////////////////////////////////////////
  
  virtual uint32_t
  GetSerializedSize () const;

  virtual void
  Serialize (TagBuffer i) const;
  
  virtual void
  Deserialize (TagBuffer i);

  virtual void
  Print (std::ostream &os) const;
  
private:
  double minBetw;
};

} // namespace ndn
} // namespace ns3

#endif // NDN_FW_MIN_BETW_TAG_H
