#ifndef __MEM_RUBY_STRUCTURES_REGIONSCOUT_HH__
#define __MEM_RUBY_STRUCTURES_REGIONSCOUT_HH__

#include <iostream>
#include <unordered_map>
#include <vector>
#include "mem/ruby/common/Address.hh"
#include "mem/ruby/common/MachineID.hh"
#define REGION_SIZE	(6)
#define CRH_SIZE (1024*1024)
#define CRH_MASK (CRH_SIZE - 1)

//ADDR is uint64_t

///////////////////////////// NSRT /////////////////////////////
class RegionScout
{
	public:
		uint32_t NSRT_size = 64;
		uint32_t MRU;
		std::unordered_map<uint64_t, bool> nsrt_map; //unordered map to contain (region, valid);

		bool verify_region(Addr address, MachineID machineID);						//verify if region is shared or not shared
		bool isNSRTFull();													//check to see if NSRT is full
		void append_NSRT(Addr address, MachineID machineID);							//region is unshared
		void invalidate_NSRT(Addr address, MachineID machineID);					//region becomes shared				
		void removeMRU_NSRT();											//NSRT is full, remove MRU

		//uint32_t CRH_size = 256;
		uint32_t crh_vector[CRH_SIZE]; 					//vector to contain (count);
		void increment_CRH(Addr addr, MachineID machineID);								//increment on a block access
		void decrement_CRH(Addr addr, MachineID machineID);								//decrement on a replacement of a block
		bool check_CRH(Addr addr, MachineID machineID);


};

inline bool RegionScout::verify_region(Addr address, MachineID machineID)
{
	uint64_t region =  address >> REGION_SIZE;
	if(nsrt_map.find(region) == nsrt_map.end()) //not found in
	{ return false; }
	else
	{
		if(nsrt_map[region] == 1) 								//valid 
		{ 
			DPRINTF(RubySlicc, "NSRT: %X region is unshared @ address %X - machine %d\n",region, address, machineID);
			return true;
		}
		else {return false;}
	}

};

inline bool RegionScout::isNSRTFull()
{
	return (nsrt_map.size() == NSRT_size);
}

inline void RegionScout::append_NSRT(Addr address, MachineID machineID)
{
	bool full = isNSRTFull();
	if(full)
	{
		removeMRU_NSRT();
	}
	uint64_t region = address >> REGION_SIZE;
	nsrt_map.insert({region, 1});
	MRU = region;
	DPRINTF(RubySlicc, "NSRT appened: %X region is validated @ address %X, machine: %d\n",region, address, machineID);
}

inline void RegionScout::removeMRU_NSRT()
{
	nsrt_map.erase(MRU);
}

inline void RegionScout::invalidate_NSRT(Addr address, MachineID machineID)
{
	uint64_t region = address >> REGION_SIZE;
	nsrt_map.erase(region);
	DPRINTF(RubySlicc, "NSRT invalidated: %X region is invalidated @ address %X, machine: %d\n",region, address, machineID);

}

inline void RegionScout::increment_CRH(Addr address, MachineID machineID)
{
	uint32_t index = (address >> REGION_SIZE) & CRH_MASK;
	crh_vector[index] += 1;
	DPRINTF(RubySlicc, "crh_vector[%d] after incr: %d @ address %X - machine %d\n",index, crh_vector[index], address, machineID);
}

inline void RegionScout::decrement_CRH(Addr address, MachineID machineID)
{
	uint32_t index = (address >> REGION_SIZE) & CRH_MASK;
	//if(!(crh_vector[index] <= 0)) {crh_vector[index] -= 1;}
	crh_vector[index] -= 1;
	DPRINTF(RubySlicc, "crh_vector[%d] after dec: %d\n @ address %X - machine %d",index, crh_vector[index], address, machineID);
}

inline bool RegionScout::check_CRH(Addr address, MachineID machineID)
{
	uint32_t index = (address >> REGION_SIZE) & CRH_MASK;
	DPRINTF(RubySlicc, "value of CRH[%d]: %d, @ %X\n - machine %d", index, crh_vector[index], address, machineID);
	if(crh_vector[index] > 0) {return true;}
	else {return false;}

}
///////////////////////////// CRH //////////////////////////////

#endif //__MEM_RUBY_STRUCTURES_REGIONSCOUT_HH__
