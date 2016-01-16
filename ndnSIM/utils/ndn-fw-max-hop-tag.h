#ifndef NDN_FW_MAX_HOP_TAG_H
#define NDN_FW_MAX_HOP_TAG_H

#include "ns3/tag.h"

namespace ns3 {
namespace ndn {

/**
 * @ingroup ndn-fw
 * @brief Packet tag that is used to track hop count for Interest-Data pairs
 */
class FwMaxHopTag : public Tag
{
public:
  static TypeId
  GetTypeId (void);

  /**
   * @brief Default constructor
   */
  FwMaxHopTag () : m_maxHop (0) { };

  /**
   * @brief Destructor
   */
  ~FwMaxHopTag () { }

  /**
   * @brief Increment hop count
   */
  void
  Increment () { m_maxHop ++; }

  /**
   * @brief Get value of hop count
   */
  uint32_t
  GetMax () const { return m_maxHop; }

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
  uint32_t m_maxHop;
};

} // namespace ndn
} // namespace ns3

#endif // NDN_FW_HOP_COUNT_TAG_H
