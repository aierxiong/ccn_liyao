#ifndef NDN_FW_MAX_REPLACE_RATE_TAG_H
#define NDN_FW_MAX_REPLACE_RATE_TAG_H

#include "ns3/tag.h"

namespace ns3 {
namespace ndn {

/**
 * @ingroup ndn-fw
 * @brief Packet tag that is used to track hop count for Interest-Data pairs
 */
class FwMaxReplacerateTag : public Tag
{
public:
  static TypeId
  GetTypeId (void);

  /**
   * @brief Default constructor
   */
  FwMaxReplacerateTag () : 
  maxReplacerate(0.0)
 { 
 };

  /**
   * @brief Destructor
   */
  ~FwMaxReplacerateTag () { }

  /**
   * @brief Increment hop count
   */
  void
  SetMaxReplacerate(double Replacerate) 
{
 maxReplacerate=Replacerate;
 }

  /**
   * @brief Get value of hop count
   */
  double
  GetMax () const { return maxReplacerate; }

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
  double maxReplacerate;
};

} // namespace ndn
} // namespace ns3

#endif // NDN_FW_MAX_REPLACERATE_TAG_H
