#include "param.h"
#include "types.h"
#include "defs.h"
#include "x86.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "spinlock.h"

struct
{
  struct spinlock lock;
  struct shm_page
  {
    uint id;
    char *frame;
    int refcnt;
  }
  shm_pages[64];
}
shm_table;

void shminit() {
  int i;
  initlock(&(shm_table.lock), "SHM lock");
  acquire(&(shm_table.lock));
  for (i = 0; i< 64; i++) {
    shm_table.shm_pages[i].id =0;
    shm_table.shm_pages[i].frame =0;
    shm_table.shm_pages[i].refcnt =0;
  }
  release(&(shm_table.lock));
}

int shm_open(int id, char **pointer)
{
  pte_t* pte;
  acquire(&(shm_table.lock));
  for (int i = 0; i< 64; i++)
  {
    //CASE1 : IT ALREADY EXIST
    if(shm_table.shm_pages[i].id==id)//S1: if it finds id in table
    {
       //S2: find the physical address of the page in the table
      //  pte = walkpgdir(shm_table.shm_pages[i].id,);

       //S3:  map it to an available page
       //ex: mappages(pgdir, 0, PGSIZE, V2P(mem), PTE_W|PTE_U);
       mappages(pte, void *va,PGSIZE, uint pa, PTE_W|PTE_U);//uses mappages to add the mapping

       shm_table.shm_pages[i].refcnt+=1;
       //pointer=virtual adress
       //update sz sice virt adress space expanded
       release(&(shm_table.lock));
       return 0;
    }
  }
//CASE 2:case it doesnt find the id in shm_table

  //S1:find an empty entry in shm_table

   int pos=-1;
   for(int i=0;<64;i++)
   {
    if(shm_table.shm_pages[i].id==0)
    {
      pos=i;
      break;
    }

   }
   if(pos==-1) //no empty position in the shm_table
    return 0;

  //if((pte = walkpgdir(pgdir, a, 1)) == 0)//call walkpgdir to find address of PTE
  //  return -1;
   shm_table.shm_pages[pos].id=id; //initialize its id to the id passed in
  // shm_table.shm_pages[pos].frame = kmalloc(); //kmalloc a page and store its address in frame
   shm_table.shm_pages[pos].refcnt=1;//set refcnt to 1
   //mappage to avaliable VA space page
   //store page mapping in shm_table

   //pointer=virtual adress
   //update sz sice virt adress space expanded
  release(&(shm_table.lock));
  return 0;
}
int shm_close(int id)
{
  //you write this too!
  acquire(&(shm_table.lock));
  for (i = 0; i< 64; i++)
  {
    if(shm_table.shm_pages[i].id==id)//if it finds id in table
    {
       shm_table.shm_pages[i].refcnt-=1;
       //uses mappages to add the mapping
       //pointer=virtual adress
       release(&(shm_table.lock));
      return 0;
    }
  }
 return 0; //added to remove compiler warning -- you should decide what to return
}
