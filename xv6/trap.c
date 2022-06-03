#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "x86.h"
#include "traps.h"
#include "spinlock.h"

# define WEXITTRAP (status)(((status) & 0x7f) -1)


// Interrupt descriptor table (shared by all CPUs).
struct gatedesc idt[256];
extern uint vectors[];  // in vectors.S: array of 256 entry pointers
struct spinlock tickslock;
uint ticks;

void
tvinit(void)
{
  int i;

  for(i = 0; i < 256; i++)
    SETGATE(idt[i], 0, SEG_KCODE<<3, vectors[i], 0);
  SETGATE(idt[T_SYSCALL], 1, SEG_KCODE<<3, vectors[T_SYSCALL], DPL_USER);

  initlock(&tickslock, "time");
}

void
idtinit(void)
{
  lidt(idt, sizeof(idt));
}




int
reservarPagina(struct proc * proc,uint va){

  char *mem;
  uint a;
  a=PGROUNDDOWN(va); // redondea la direccion virtual a limite de pagina


  mem=kalloc(); // reserva una pagina fisica

  if(mem==0){ // no nos da mas memoria

    return -1;
  }

  memset(mem,0,PGSIZE); // inicializamos la pagina a 0


  if(mappages(proc->pgdir, (char*)a, PGSIZE, V2P(mem), PTE_W|PTE_U) < 0){ // intentamos mapear la pagina a memoria

    kfree(mem);
    return -1;
  }


  return 0;

}



//PAGEBREAK: 41
void
trap(struct trapframe *tf)
{
  if(tf->trapno == T_SYSCALL){
    if(myproc()->killed)
      exit(0);
    myproc()->tf = tf;
    syscall();
    if(myproc()->killed)
      exit(0);
    return;
  }

  uint dirErr;

  switch(tf->trapno){
  case T_IRQ0 + IRQ_TIMER:
    if(cpuid() == 0){
      acquire(&tickslock);
      ticks++;
      wakeup(&ticks);
      release(&tickslock);
    }
    lapiceoi();
    break;
  case T_IRQ0 + IRQ_IDE:
    ideintr();
    lapiceoi();
    break;
  case T_IRQ0 + IRQ_IDE+1:
    // Bochs generates spurious IDE1 interrupts.
    break;
  case T_IRQ0 + IRQ_KBD:
    kbdintr();
    lapiceoi();
    break;
  case T_IRQ0 + IRQ_COM1:
    uartintr();
    lapiceoi();
    break;
  case T_IRQ0 + 7:
  case T_IRQ0 + IRQ_SPURIOUS:
    cprintf("cpu%d: spurious interrupt at %x:%x\n",
            cpuid(), tf->cs, tf->eip);
    lapiceoi();
    break;

  case T_PGFLT: // si la pagina no esta presente la reservamos


    //*rcr2 nos informa de que direccion se ha intentado acceder

    
    dirErr=PGROUNDDOWN(rcr2());

    if(dirErr>=myproc()->sz){  // nos pasamos del tamaño de heap reservado
      
      myproc()->killed=1;
      break;
    }

    if(dirErr>=KERNBASE){ // si el error se da en una memoria perteneciente a la parte del OS en el proceso

      myproc()->killed=1;
      break;

    }

    if(dirErr==myproc()->inicio_pagina_guarda){  // si nos metemos en la pagina de guarda

      myproc()->killed=1;
      break;
    }
    
    if(reservarPagina(myproc(),rcr2())<0){  // hecha arriba ARRIBA 

      myproc()->killed=1;
        
    }

    break;





  //PAGEBREAK: 13
  default:
    if(myproc() == 0 || (tf->cs&3) == 0){
      // In kernel, it must be our mistake.
      cprintf("unexpected trap %d from cpu %d eip %x (cr2=0x%x)\n",
              tf->trapno, cpuid(), tf->eip, rcr2());
      panic("trap");
    }

    // In user space, assume process misbehaved.
    cprintf("pid %d %s: trap %d err %d on cpu %d "
            "eip 0x%x addr 0x%x--kill proc\n",
            //mi identificador de proceso, mi nombre, tf->trapno ejecutado esta trap, tf->err he provocado este error, estoy en la Cpu tal
            // este es el contador del programa y rcr2 es un registro interno del procesador qué nos dice cuando se produce 
            //una excepción cuál es la dirección de memoria que la ha provocado
            myproc()->pid, myproc()->name, tf->trapno,
            tf->err, cpuid(), tf->eip, rcr2());
    myproc()->killed = 1;
  }

  // Force process exit if it has been killed and is in user space.
  // (If it is still executing in the kernel, let it keep running
  // until it gets to the regular system call return.)
  if(myproc() && myproc()->killed && (tf->cs&3) == DPL_USER)
    exit(0);

  // Force process to give up CPU on clock tick.
  // If interrupts were on while locks held, would need to check nlock.
  if(myproc() && myproc()->state == RUNNING &&
     tf->trapno == T_IRQ0+IRQ_TIMER)
    yield();

  // Check if the process has been killed since we yielded
  if(myproc() && myproc()->killed && (tf->cs&3) == DPL_USER)
    exit(0);
}
