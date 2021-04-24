#ifndef __MEM_RUBY_STRUCTURES_REGIONSCOUT_HH__
#define __MEM_RUBY_STRUCTURES_REGIONSCOUT_HH__

#include <iostream>
#include <unordered_map>
#include <vector>
#include "mem/ruby/common/Address.hh"

//ADDR is uint64_t

///////////////////////////// NSRT /////////////////////////////
class NSRT
{
	public:
		uint32_t NSRT_size = 64;
		uint32_t MRU;
		std::unordered_map<uint32_t, bool> nrst_map; //unordered map to contain (region, valid);

		bool verify_region(Addr address);						//verify if region is shared or not shared
		bool isNSRTFull();													//check to see if NSRT is full
		void append_NSRT(Addr address);							//region is unshared
		void invalidate_NSRT(Addr address);					//region becomes shared				
		void removeMRU_NSRT();											//NSRT is full, remove MRU


};

inline bool NSRT::verify_region(Addr address)
{
	uint32_t region =  Addr >> 14;
	if(nsrt_map.find(region) == nsrt_map.end()) //not found in
	{ return false; }
	else
	{
		if(nsrt_map[region] == 1) 								//valid 
		{ return true;}
		else {return false;}
	}

};

inline bool NSRT::isNSRTFull()
{
	return (nsrt_map.size() == NSRT_size)
}

inline void NSRT::append_NSRT(Addr address)
{
	bool full = isNSRTFull();
	if(full)
	{
		removeMRU_NSRT();
	}
	uint32_t region = Addr >> 14;
	nsrt_map.insert({region, 1});
	MRU = region;
}

inline void NSRT::removeMRU_NSRT();
{
	nsrt_map.erase(MRU);
}

inline void NSRT::invalidate_NSRT(Addr address)
{
	uint32_t region = Addr >> 14;
	nsrt_map.erase(region);

}

///////////////////////////// NSRT /////////////////////////////

///////////////////////////// CRH //////////////////////////////
class CRH
{
	public:
		uint32_t CRH_size = 256;
		std::vector<uint32_t> crh_vector; 					//vector to contain (count);
		void incrementCRH(Addr addr);								//increment on a block access
		void decrementCRH(Addr addr);								//decrement on a replacement of a block
		bool checkCRH(Addr addr);
		
};

inline void CRH::incrementCRH(Addr addr)
{
	int index = (Addr >> 14) && 0xFF;
	crh_vector[index] += 1;
}

inline void CRH::decrementCRH(Addr addr)
{
	int index = (Addr >> 14) && 0xFF;
	crh_vector[index] -= 1;
}

inline bool CRH::checkCRH(Addr addr)
{
	int index = (Addr >> 14) && 0xFF;
	if(crh_vector[index] > 0) {return true;}
	else {return false;}

}
///////////////////////////// CRH //////////////////////////////

#endif //__MEM_RUBY_STRUCTURES_REGIONSCOUT_HH__
