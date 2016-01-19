#include "ndn-fw-min-replacerate-tag.h"

namespace ns3 {
namespace ndn {

TypeId
FwMinReplacerateTag::GetTypeId ()
{
  static TypeId tid = TypeId("ns3::ndn::FwMinReplacerateTag")
    .SetParent<Tag>()
    .AddConstructor<FwMinReplacerateTag>()
    ;
  return tid;
}

TypeId
FwMinReplacerateTag::GetInstanceTypeId () const
{
  return FwMinReplacerateTag::GetTypeId ();
}

uint32_t
FwMinReplacerateTag::GetSerializedSize () const
{
  return sizeof(double);
}

void
FwMinReplacerateTag::Serialize (TagBuffer i) const
{
  i.WriteDouble(minReplacerate);
}
  
void
FwMinReplacerateTag::Deserialize (TagBuffer i)
{
  minReplacerate = i.ReadDouble ();
}

void
FwMinReplacerateTag::Print (std::ostream &os) const
{
  os << minReplacerate;
}

} // namespace ndn
} // namespace ns3