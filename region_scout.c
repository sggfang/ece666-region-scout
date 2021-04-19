#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#define NSRT_size 64
#define CRH_size 256
#define num_machines 8
#define BLOCK_SIZE 32
#define CACHE_SIZE 4096
#define count_field log(CACHE_SIZE/BLOCK_SIZE)

typedef struct NSRT{

	bool valid[NSRT_size];
	uint32_t region[NSRT_size];

}NSRT;

typedef struct CRH{
	uint32_t count[CRH_size];
	bool p[CRH_size];

}CRH;
uint32_t gen_add()
{
	uint32_t x;
	x = rand() & 0xff;
	x |= (rand() & 0xff) << 8;
	x |= (rand() & 0xff) << 16;
	x |= (rand() & 0xff) << 24;
	return x;


}
int gen_machine()
{
	int n;
	n = rand() % num_machines;
	return n;

}

void check_regions(NSRT* nsrt, int machine_id, uint32_t add_region, int index)
{
	int i = 0;
	for(i = 0; i < num_machines; i++)
	{
		if(i != machine_id)
		{
			int j = 0;
			for(j = 0; j < NSRT_size; j++)
			{
				if(nsrt[i].valid[j] && (nsrt[i].region[j] == add_region))
				{
					nsrt[i].valid[j] = 0;
					nsrt[machine_id].valid[index] = 0;
					printf("invalidated machine %d index %d region %X\n", i, j, add_region);
				}
			}

		}
	}

	return;


}

void append_to_NSRT(NSRT* nsrt, int machine_id, uint32_t address)
{
	int i = 0;
	uint32_t add_region = address >> 15;
	for(i = 0; i < NSRT_size; i++)
	{
		if(nsrt[machine_id].valid[i] && (nsrt[machine_id].region[i] == add_region))
		{
			//printf("region already recorded\n");
			break; //region already recorded
		}
		if(!(nsrt[machine_id].valid[i]))
		{
			//printf("appended machine %d -> region: %X\n", machine_id, add_region);
			nsrt[machine_id].valid[i] = 1;
			nsrt[machine_id].region[i] = add_region;
			check_regions(nsrt, machine_id, add_region, i);
			
			break;
		}
		
	}
	if(i == 64)
	{
		uint32_t victim = rand() % NSRT_size;
		nsrt[machine_id].valid[victim] = 1;
		nsrt[machine_id].region[victim] = add_region;
		check_regions(nsrt, machine_id, add_region, victim);
		//printf("appended machine %d, victim %d -> region: %X\n", machine_id, victim, add_region);		

	}


}

void append_to_CRH(CRH* crh, int machine_id, uint32_t address)
{
	int index = (address >> 4) & 0xff;
	crh[machine_id].count[index] += 1;

	return;
}

int main(void)
{
	NSRT* nsrt = (NSRT*)malloc(sizeof(NSRT) * num_machines);
	CRH* crh= (CRH*)malloc(sizeof(CRH) * num_machines);
	int i = 0;
	for(i = 0; i < 3000; i++)
	{
		uint32_t random = gen_add();
		int random_machine = gen_machine();	
		append_to_CRH(crh, random_machine, random);
		append_to_NSRT(nsrt, random_machine, random);
		
	}
	return 0;
}


//////////// NOTES IN REGARD TO STEPS //////////
/*

NODE N reqests block B in region RB -- First Request
1)N checks its NSRT, checks if shared or not
2)request is broadcasted to all nodes
3)all remote nodes probe their CRH and report that they do not cache any block in region RB
4)Node N records RB as not shared and incrememnts the corresponding CRH entry

NODE N requests block B' in region RB -- subsequent request
1)Node N first checks its NSRT
2)entry is found in NSRT the request is sent only to main memory

NODE N' requests block B'' in region RB -- another node requests region
1)Node N' requests block B'' in region RB, checks its NSRT
2)Since region is not recorded it its NSRT, broadcasts its request
3)Node N invalidates its NSRT entry and now RB is shared
hello
*/
///////////////////////////////////////////////
