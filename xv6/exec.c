#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "defs.h"
#include "x86.h"
#include "elf.h"

int
exec(char *path, char **argv)
{
  char *s, *last;
  int i, off;
  uint argc, sz, sp, ustack[3+MAXARG+1];
  uint gp; //! NUEVO , USADO PARA PASAR INICIO DE LA PÁGINA DE GUARDA
  struct elfhdr elf;
  struct inode *ip;
  struct proghdr ph;
  pde_t *pgdir, *oldpgdir;
  struct proc *curproc = myproc();

  begin_op();

  if((ip = namei(path)) == 0){
    end_op();
    cprintf("exec: fail\n");
    return -1;
  }
  ilock(ip);
  pgdir = 0;

  // Check ELF header
  if(readi(ip, (char*)&elf, 0, sizeof(elf)) != sizeof(elf))
    goto bad;
  if(elf.magic != ELF_MAGIC)
    goto bad;

  if((pgdir = setupkvm()) == 0)
    goto bad;

  // Load program into memory.
  sz = 0;
  for(i=0, off=elf.phoff; i<elf.phnum; i++, off+=sizeof(ph)){
    if(readi(ip, (char*)&ph, off, sizeof(ph)) != sizeof(ph))
      goto bad;
    if(ph.type != ELF_PROG_LOAD || ph.memsz == 0)
      continue;
    if(ph.memsz < ph.filesz)
      goto bad;
    if(ph.vaddr + ph.memsz < ph.vaddr)
      goto bad;
    if((sz = allocuvm(pgdir, sz, ph.vaddr + ph.memsz)) == 0)
      goto bad;
    if(ph.vaddr % PGSIZE != 0)
      goto bad;
    if(loaduvm(pgdir, (char*)ph.vaddr, ip, ph.off, ph.filesz) < 0)
      goto bad;
  }
  iunlockput(ip);
  end_op();
  ip = 0;

  // Allocate two pages at the next page boundary.
  // Make the first inaccessible.  Use the second as the user stack.
 

  sz = PGROUNDUP(sz);     //* sz tiene la última de datos y codigo

  uint bloque_datos_y_codigo= sz/PGSIZE; //* número de paginas que ocupan datos y codigo


  //? ESTO RESERVA ESPACIO PARA LA PILA Y LA PÁGINA DE GUARDA , ENCIMA DE CODIGO Y DATOS
  if((sz = allocuvm(pgdir, sz, sz+(bloque_datos_y_codigo+1)*PGSIZE)) == 0) //* antes habia (sz+2*PGSIZE ) subimos el inicio del heap , creando espacio entre medias para pila + pagina de guarda
    
  //! BLOQUE DE DATOS Y CODIGO + 1 = PAGINA DE GUARDA + nº paginas de dato y codigo
  //! sz+(bloque_datos_y_codigo+1)*PGSIZE = aumentar sz hasta ese tamaño

    goto bad;
  clearpteu(pgdir, (char*)(sz - (bloque_datos_y_codigo+1)*PGSIZE)); // COGE EL INICIO DE LA PAGINA DE GUARDA =  FINAL PILA - PAGINAS DE LA PILA - PÁGINA GUARDA
  sp = sz; //* sp contiene el final de la pila / comienzo del heap
  //! NUEVO
  //gp=sp-PGSIZE*2; //* Ya que al crear proceso el final de la pila es igual al principio del heap , por lo que 2 paginas menos es el inicio 
                  //*de la página de guarda

  gp=sp-((bloque_datos_y_codigo+1)*PGSIZE);

  curproc->inicio_pagina_guarda=gp;



  // Push argument strings, prepare rest of stack in ustack.
  for(argc = 0; argv[argc]; argc++) {
    if(argc >= MAXARG)
      goto bad;
    sp = (sp - (strlen(argv[argc]) + 1)) & ~3;
    if(copyout(pgdir, sp, argv[argc], strlen(argv[argc]) + 1) < 0)
      goto bad;
    ustack[3+argc] = sp;
  }
  ustack[3+argc] = 0;

  ustack[0] = 0xffffffff;  // fake return PC
  ustack[1] = argc;
  ustack[2] = sp - (argc+1)*4;  // argv pointer

  sp -= (3+argc+1) * 4;
  if(copyout(pgdir, sp, ustack, (3+argc+1)*4) < 0)
    goto bad;

  // Save program name for debugging.
  for(last=s=path; *s; s++)
    if(*s == '/')
      last = s+1;
  safestrcpy(curproc->name, last, sizeof(curproc->name));

  // Commit to the user image.
  oldpgdir = curproc->pgdir;
  curproc->pgdir = pgdir;
  curproc->sz = sz;
  curproc->tf->eip = elf.entry;  // main
  curproc->tf->esp = sp;
  switchuvm(curproc);
  freevm(oldpgdir);
  return 0;

 bad:
  if(pgdir)
    freevm(pgdir);
  if(ip){
    iunlockput(ip);
    end_op();
  }
  return -1;
}
