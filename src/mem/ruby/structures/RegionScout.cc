#include <mem/ruby/structures/RegionScout.hh>

inline bool RegionScout::verify_region(Addr address)
{
	uint64_t region =  address >> 14;
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
	uint64_t region = address >> 14;
	nsrt_map.insert({region, 1});
	MRU = region;
}

inline void RegionScout::removeMRU_NSRT()
{
	nsrt_map.erase(MRU);
}

inline void RegionScout::invalidate_NSRT(Addr address)
{
	uint64_t region = address >> 14;
	nsrt_map.erase(region);

}

inline void RegionScout::increment_CRH(Addr address)
{
	int index = (address >> 14) && 0xFF;
	crh_vector[index] += 1;
	DPRINTF(RubySlicc, "crh_vector after incr: %d\n", crh_vector[index]);
}

inline void RegionScout::decrement_CRH(Addr address)
{
	int index = (address >> 14) && 0xFF;
	crh_vector[index] -= 1;
	DPRINTF(RubySlicc, "crh_vector after dec: %d\n", crh_vector[index]);
}

inline bool RegionScout::check_CRH(Addr address)
{
	int index = (address >> 14) && 0xFF;
	if(crh_vector[index] > 0) {return true;}
	else {return false;}

}
///////////////////////////// CRH //////////////////////////////
