#include "ndn-fw-max-betw-tag.h"

namespace ns3 {
namespace ndn {

TypeId
FwMaxBetwTag::GetTypeId ()
{
  static TypeId tid = TypeId("ns3::ndn::FwMaxBetwTag")
    .SetParent<Tag>()
    .AddConstructor<FwMaxBetwTag>()
    ;
  return tid;
}

TypeId
FwMaxBetwTag::GetInstanceTypeId () const
{
  return FwMaxBetwTag::GetTypeId ();
}

uint32_t
FwMaxBetwTag::GetSerializedSize () const
{
  return sizeof(double);
}

void
FwMaxBetwTag::Serialize (TagBuffer i) const
{
  i.WriteDouble(maxBetw);
}
  
void
FwMaxBetwTag::Deserialize (TagBuffer i)
{
  maxBetw = i.ReadDouble ();
}

void
FwMaxBetwTag::Print (std::ostream &os) const
{
  os << maxBetw;
}

} // namespace ndn
} // namespace ns3
