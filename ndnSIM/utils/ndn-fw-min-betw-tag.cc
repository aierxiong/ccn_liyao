#include "ndn-fw-min-betw-tag.h"

namespace ns3 {
namespace ndn {

TypeId
FwMinBetwTag::GetTypeId ()
{
  static TypeId tid = TypeId("ns3::ndn::FwMinBetwTag")
    .SetParent<Tag>()
    .AddConstructor<FwMinBetwTag>()
    ;
  return tid;
}

TypeId
FwMinBetwTag::GetInstanceTypeId () const
{
  return FwMinBetwTag::GetTypeId ();
}

uint32_t
FwMinBetwTag::GetSerializedSize () const
{
  return sizeof(double);
}

void
FwMinBetwTag::Serialize (TagBuffer i) const
{
  i.WriteDouble(minBetw);
}
  
void
FwMinBetwTag::Deserialize (TagBuffer i)
{
  minBetw = i.ReadDouble ();
}

void
FwMinBetwTag::Print (std::ostream &os) const
{
  os << minBetw;
}

} // namespace ndn
} // namespace ns3