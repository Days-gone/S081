#include "types.h"
#include "riscv.h"
#include "param.h"
#include "defs.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;


  argint(0, &n);
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}


#ifdef LAB_PGTBL
uint64
sys_pgaccess(void)
{
  // lab pgtbl: your code here.
  uint64 start_addr_u;
  int check_num_of_page;
  uint64 bitmask_uaddr;
  
  argaddr(0, &start_addr_u);
  argint(1, &check_num_of_page);
  argaddr(2, &bitmask_uaddr);
  // above is getting parameter, follows are functionality impl
  if (check_num_of_page > 32 || check_num_of_page < 0)
    panic("Pgaccess: wrong check page number");
  unsigned int kbitsmask = 0;
  struct proc* p = myproc();
  for (int i = 0; i < check_num_of_page; i ++){
    pte_t* target_pte = walk(p->pagetable, start_addr_u + i * PGSIZE, 0);
    if ((*target_pte & PTE_V) && (*target_pte & PTE_A) != 0) {
      kbitsmask |= (1L << i);
      *target_pte &= (~PTE_A);
    }
  }

  // show the bitmask bit pattern
  for (int i = 31; i >= 0; i --)
  {
    if (i == 31)printf("Bitmask:");
    printf("%d", (kbitsmask >> i) & 1);
    if (i == 0)printf("\n");
  }

  // type cast here should use C-like style such as: (int)bitmask
  // instead of functional style in cpp: such as int(bitmask)
  int rc = copyout(myproc()->pagetable, bitmask_uaddr, (char*)&kbitsmask, sizeof(kbitsmask));
  if (rc != 0)
    panic("Pgaccess: copy out error!\n");
  return 0;
}
#endif

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}
