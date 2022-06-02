#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{

  int status;

  // recogemos el int y lo pasamos a la funcion exit de proc.c ( paso de parametro de la pila de usuario a kernel)
  if(argint(0,&status)<0){

	  return -1;
  }

  status=status<<8;
  exit(status);
  return 0;

}

int
sys_wait(void)
{
  int * status;

  // recogemos el puntero a int y lo pasamos a la funcion wait de proc.c( paso de parametro de la pila de usuario a kernel)
	if(argptr(0,(void**)&status,sizeof(int*))<0){

	  return -1;
   
  }

  return wait(status);
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
	return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

/*
int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
	return -1;

  addr = myproc()->sz;

  if(growproc(n) < 0)
	return -1;

  return addr;
}*/

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0){ // recogemos el argumento con el tamaño
	  return -1;
  }
  addr = myproc()->sz;

  if(n<0){ // si el tamaño es menor que el actual decrementamos ekl tamaño del proceso + quitar pags en growproc + growproc actualiza tabla de paginas

    if(growproc(n) < 0){  //? growproc devuelve 0 si todo va bien o -1 si hay error
	    return -1;
    }
    return addr;
  }

  if(n==0){ // si es igual devolvemos el tamaño que tiene myproc()->sz

    return addr;
  }

  if(myproc()->sz+n>=KERNBASE){ // si nos pasamos del tam

    return -1;
  }
  myproc()->sz+=n;

  return addr;
}


int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
	return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
	if(myproc()->killed){
	  release(&tickslock);
	  return -1;
	}
	sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}


int
sys_date(void){

  struct rtcdate* r;

  // argptr accede al parametro 0 de la pila del usuarioy lo introduce en la variable r indicando tamaño de la variable

  if(argptr(0,(void**)&r,sizeof(struct rtcdate))<0){

	return -1;

  }

  // llamada al sistema

  cmostime(r);

  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}
