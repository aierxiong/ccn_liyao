#ifndef NDN_FW_MIN_REPLACE_RATE_TAG_H
#define NDN_FW_MIN_REPLACE_RATE_TAG_H

#include "ns3/tag.h"

namespace ns3 {
namespace ndn {

/**
 * @ingroup ndn-fw
 * @brief Packet tag that is used to track hop count for Interest-Data pairs
 */
class FwMinReplacerateTag : public Tag
{
public:
  static TypeId
  GetTypeId (void);

  /**
   * @brief Default constructor
   */
  FwMinReplacerateTag () : 
  minReplacerate(3.0)
 { 
 };

  /**
   * @brief Destructor
   */
  ~FwMinReplacerateTag () { }

  /**
   * @brief Increment hop count
   */
  void
  SetMinReplacerate (double Replacerate) 
{
    minReplacerate = Replacerate;
}

  /**
   * @brief Get value of hop count
   */
  double
  GetMin () const { return minReplacerate; }

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
  double minReplacerate;
};

} // namespace ndn
} // namespace ns3

#endif // NDN_FW_MIN_REPLACERATE_TAG_H