#include "ndn-fw-max-replacerate-tag.h"

namespace ns3 {
namespace ndn {

TypeId
FwMaxReplacerateTag::GetTypeId ()
{
  static TypeId tid = TypeId("ns3::ndn::FwMaxReplacerateTag")
    .SetParent<Tag>()
    .AddConstructor<FwMaxReplacerateTag>()
    ;
  return tid;
}

TypeId
FwMaxReplacerateTag::GetInstanceTypeId () const
{
  return FwMaxReplacerateTag::GetTypeId ();
}

uint32_t
FwMaxReplacerateTag::GetSerializedSize () const
{
  return sizeof(double);
}

void
FwMaxReplacerateTag::Serialize (TagBuffer i) const
{
  i.WriteDouble(maxReplacerate);
}
  
void
FwMaxReplacerateTag::Deserialize (TagBuffer i)
{
  maxReplacerate = i.ReadDouble ();
}

void
FwMaxReplacerateTag::Print (std::ostream &os) const
{
  os << maxReplacerate;
}

} // namespace ndn
} // namespace ns3
