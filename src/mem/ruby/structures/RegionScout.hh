#ifndef __MEM_RUBY_STRUCTURES_REGIONSCOUT_HH__
#define __MEM_RUBY_STRUCTURES_REGIONSCOUT_HH__

#include <iostream>
#include <unordered_map>
#include <vector>
#include "mem/ruby/common/Address.hh"
#define REGION_SIZE	(8)
#define CRH_SIZE (1024)
#define CRH_MASK (CRH_SIZE - 1)

//ADDR is uint64_t

///////////////////////////// NSRT /////////////////////////////
class RegionScout
{
	public:
		uint32_t NSRT_size = 64;
		uint32_t MRU;
		std::unordered_map<uint64_t, bool> nsrt_map; //unordered map to contain (region, valid);

		bool verify_region(Addr address);						//verify if region is shared or not shared
		bool isNSRTFull();													//check to see if NSRT is full
		void append_NSRT(Addr address);							//region is unshared
		void invalidate_NSRT(Addr address);					//region becomes shared				
		void removeMRU_NSRT();											//NSRT is full, remove MRU

		uint32_t CRH_size = 256;
		uint32_t crh_vector[CRH_SIZE]; 					//vector to contain (count);
		void increment_CRH(Addr addr);								//increment on a block access
		void decrement_CRH(Addr addr);								//decrement on a replacement of a block
		bool check_CRH(Addr addr);


};

inline bool RegionScout::verify_region(Addr address)
{
	uint64_t region =  address >> REGION_SIZE;
	if(nsrt_map.find(region) == nsrt_map.end()) //not found in
	{ return false; }
	else
	{
		if(nsrt_map[region] == 1) 								//valid 
		{ return true;}
		else {return false;}
	}

};

inline bool RegionScout::isNSRTFull()
{
	return (nsrt_map.size() == NSRT_size);
}

inline void RegionScout::append_NSRT(Addr address)
{
	bool full = isNSRTFull();
	if(full)
	{
		removeMRU_NSRT();
	}
	uint64_t region = address >> REGION_SIZE;
	nsrt_map.insert({region, 1});
	MRU = region;
}

inline void RegionScout::removeMRU_NSRT()
{
	nsrt_map.erase(MRU);
}

inline void RegionScout::invalidate_NSRT(Addr address)
{
	uint64_t region = address >> REGION_SIZE;
	nsrt_map.erase(region);

}

inline void RegionScout::increment_CRH(Addr address)
{
	int index = (address >> REGION_SIZE) && CRH_MASK;
	crh_vector[index] += 1;
	DPRINTF(RubySlicc, "crh_vector[%d] after incr: %d @ address %X\n",index, crh_vector[index], address);
}

inline void RegionScout::decrement_CRH(Addr address)
{
	int index = (address >> REGION_SIZE) && CRH_MASK;
	//if(!(crh_vector[index] <= 0)) {crh_vector[index] -= 1;}
	crh_vector[index] -= 1;
	DPRINTF(RubySlicc, "crh_vector[%d] after dec: %d\n @ address %X",index, crh_vector[index], address);
}

inline bool RegionScout::check_CRH(Addr address)
{
	int index = (address >> REGION_SIZE) && CRH_MASK;
	DPRINTF(RubySlicc, "value of CRH[%d]: %d, @ %X\n", index, crh_vector[index], address);
	if(crh_vector[index] > 0) {return true;}
	else {return false;}

}
///////////////////////////// CRH //////////////////////////////

#endif //__MEM_RUBY_STRUCTURES_REGIONSCOUT_HH__
