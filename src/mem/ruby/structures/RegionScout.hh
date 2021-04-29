#ifndef __MEM_RUBY_STRUCTURES_REGIONSCOUT_HH__
#define __MEM_RUBY_STRUCTURES_REGIONSCOUT_HH__

#include <iostream>
#include <unordered_map>
#include <vector>
#include "mem/ruby/common/Address.hh"
#include "mem/ruby/common/MachineID.hh"
#define REGION_SIZE	(14)
#define CRH_SIZE (256)
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
		void invalidate_NSRT(Addr address, MachineID machineID, MachineID requestor);					//region becomes shared				
		void removeMRU_NSRT(Addr address, MachineID machineID);											//NSRT is full, remove MRU

		//uint32_t CRH_size = 256;
		uint32_t crh_vector_id0[CRH_SIZE]; 																//vector to contain (count);
		uint32_t crh_vector_id1[CRH_SIZE]; 																//vector to contain (count);
		uint32_t crh_vector_id2[CRH_SIZE]; 																//vector to contain (count);
		uint32_t crh_vector_id3[CRH_SIZE]; 																//vector to contain (count);
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
		removeMRU_NSRT(address, machineID);
	}
	uint64_t region = address >> REGION_SIZE;
	nsrt_map.insert({region, 1});
	MRU = region;
	DPRINTF(RubySlicc, "NSRT appened: %X region is validated @ address %X, machine: %d\n",region, address, machineID);
}

inline void RegionScout::removeMRU_NSRT(Addr address, MachineID machineID)
{
	uint64_t region = address >> REGION_SIZE;
	DPRINTF(RubySlicc, "NSRT MRU removed: %X region is invalidated @ address %X, machine: %d\n",region, address, machineID);
	nsrt_map.erase(MRU);
}

inline void RegionScout::invalidate_NSRT(Addr address, MachineID machineID, MachineID requestor)
{
	uint64_t region = address >> REGION_SIZE;
	nsrt_map.erase(region);
	DPRINTF(RubySlicc, "NSRT invalidated: %X region is invalidated @ address %X, machine: %d because of machine: %d\n",region, address, machineID, requestor);

}

inline void RegionScout::increment_CRH(Addr address, MachineID machineID)
{
	uint32_t index0 = (address >> REGION_SIZE) & CRH_MASK;
	uint32_t index1 = (address >> (REGION_SIZE + 8)) & CRH_MASK;
	uint32_t index2 = (address >> (REGION_SIZE + 16)) & CRH_MASK;
	uint32_t index3 = (address >> (REGION_SIZE + 24)) & CRH_MASK;
	//if(!(crh_vector[index] <= 0)) {crh_vector[index] -= 1;}
	crh_vector_id0[index0] += 1;
	crh_vector_id1[index1] += 1;
	crh_vector_id2[index2] += 1;
	crh_vector_id3[index3] += 1;
	DPRINTF(RubySlicc, "crh_vector_id0[%d] after incr: %d @ address %X - machine %d\n",index0, crh_vector_id0[index0], address, machineID);
	DPRINTF(RubySlicc, "crh_vector_id1[%d] after incr: %d @ address %X - machine %d\n",index1, crh_vector_id1[index1], address, machineID);
	DPRINTF(RubySlicc, "crh_vector_id2[%d] after incr: %d @ address %X - machine %d\n",index2, crh_vector_id2[index2], address, machineID);
	DPRINTF(RubySlicc, "crh_vector_id3[%d] after incr: %d @ address %X - machine %d\n",index3, crh_vector_id3[index3], address, machineID);
}

inline void RegionScout::decrement_CRH(Addr address, MachineID machineID)
{
	uint32_t index0 = (address >> REGION_SIZE) & CRH_MASK;
	uint32_t index1 = (address >> (REGION_SIZE + 8)) & CRH_MASK;
	uint32_t index2 = (address >> (REGION_SIZE + 16)) & CRH_MASK;
	uint32_t index3 = (address >> (REGION_SIZE + 24)) & CRH_MASK;
	crh_vector_id0[index0] -= 1;
	crh_vector_id1[index1] -= 1;
	crh_vector_id2[index2] -= 1;
	crh_vector_id3[index3] -= 1;
	DPRINTF(RubySlicc, "crh_vector_id0[%d] after dec: %d @ address %X - machine %d\n",index0, crh_vector_id0[index0], address, machineID);
	DPRINTF(RubySlicc, "crh_vector_id1[%d] after dec: %d @ address %X - machine %d\n",index1, crh_vector_id1[index1], address, machineID);
	DPRINTF(RubySlicc, "crh_vector_id2[%d] after dec: %d @ address %X - machine %d\n",index2, crh_vector_id2[index2], address, machineID);
	DPRINTF(RubySlicc, "crh_vector_id3[%d] after dec: %d @ address %X - machine %d\n",index3, crh_vector_id3[index3], address, machineID);
	
}

inline bool RegionScout::check_CRH(Addr address, MachineID machineID)
{
	uint32_t index0 = (address >> REGION_SIZE) & CRH_MASK;
	uint32_t index1 = (address >> (REGION_SIZE + 8)) & CRH_MASK;
	uint32_t index2 = (address >> (REGION_SIZE + 16)) & CRH_MASK;
	uint32_t index3 = (address >> (REGION_SIZE + 24)) & CRH_MASK;
	DPRINTF(RubySlicc, "value of crh_vector_id0[%d]: %d, @ %X - machine %d\n", index0, crh_vector_id0[index0], address, machineID);
	DPRINTF(RubySlicc, "value of crh_vector_id1[%d]: %d, @ %X - machine %d\n", index1, crh_vector_id1[index1], address, machineID);
	DPRINTF(RubySlicc, "value of crh_vector_id2[%d]: %d, @ %X - machine %d\n", index2, crh_vector_id2[index2], address, machineID);
	DPRINTF(RubySlicc, "value of crh_vector_id3[%d]: %d, @ %X - machine %d\n", index3, crh_vector_id3[index3], address, machineID);
	if((crh_vector_id0[index0] > 0) && (crh_vector_id1[index1] > 0) && (crh_vector_id2[index2] > 0) && (crh_vector_id3[index3] > 0)) 
	{
		DPRINTF(RubySlicc, "all indicies are greater than 0\n");
		return true;
	}
	else 
	{
		DPRINTF(RubySlicc, "one or more indicies are eqaul to 0\n");
		return false;
}

}
///////////////////////////// CRH //////////////////////////////

#endif //__MEM_RUBY_STRUCTURES_REGIONSCOUT_HH__
