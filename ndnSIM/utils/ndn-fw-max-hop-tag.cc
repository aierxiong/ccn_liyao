#include "ndn-fw-max-hop-tag.h"

namespace ns3 {
namespace ndn {

TypeId
FwMaxHopTag::GetTypeId ()
{
  static TypeId tid = TypeId("ns3::ndn::FwMaxHopTag")
    .SetParent<Tag>()
    .AddConstructor<FwMaxHopTag>()
    ;
  return tid;
}

TypeId
FwMaxHopTag::GetInstanceTypeId () const
{
  return FwMaxHopTag::GetTypeId ();
}

uint32_t
FwMaxHopTag::GetSerializedSize () const
{
  return sizeof(uint32_t);
}

void
FwMaxHopTag::Serialize (TagBuffer i) const
{
  i.WriteU32 (m_maxHop);
}
  
void
FwMaxHopTag::Deserialize (TagBuffer i)
{
   m_maxHop = i.ReadU32 ();
}

void
FwMaxHopTag::Print (std::ostream &os) const
{
  os << m_maxHop;
}

} // namespace ndn
} // namespace ns3
