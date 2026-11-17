#ifndef PHONGTHAN_CONNECTION_INFO_H
#define PHONGTHAN_CONNECTION_INFO_H
// Local Game/CoreClient query only; no network or persisted layout changes.
#define PT_CONNECTION_INFO_QUERY 0x50544e
struct PT_CONNECTION_INFO {unsigned int HasSample,RoundTripMs,SampleAgeMs;};
inline int PhongThanConnectionQuality(bool connected,const PT_CONNECTION_INFO& sample){
 if(!connected)return 0;
 if(!sample.HasSample)return 1;
 // Old samples have unknown current quality; they are not evidence of latency.
 if(sample.SampleAgeMs>10000)return 1;
 return sample.RoundTripMs<=100?2:sample.RoundTripMs<=300?3:4;
}
#endif
