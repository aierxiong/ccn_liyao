#ifndef NDN_FW_MAX_BETW_TAG_H
#define NDN_FW_MAX_BETW_TAG_H

#include "ns3/tag.h"

namespace ns3 {
namespace ndn {

/**
 * @ingroup ndn-fw
 * @brief Packet tag that is used to track hop count for Interest-Data pairs
 */
class FwMaxBetwTag : public Tag
{
public:
  static TypeId
  GetTypeId (void);

  /**
   * @brief Default constructor
   */
  FwMaxBetwTag () : 
  maxBetw (0.0)
 { 
 };

  /**
   * @brief Destructor
   */
  ~FwMaxBetwTag () { }

  /**
   * @brief Increment hop count
   */
  void
  SetMaxBetw (double Betw) 
 {
    maxBetw = Betw;
 }

  /**
   * @brief Get value of hop count
   */
  double
  GetMax () const { return maxBetw; }

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
  double maxBetw;
};

} // namespace ndn
} // namespace ns3

#endif // NDN_FW_MAX_BETW_TAG_H
