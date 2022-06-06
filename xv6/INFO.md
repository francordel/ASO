<h1>XV6 GUÍA</h1>



<h2>P2</h2></br> 


<h3>DUP2</h3>

* Modificar **[sys_file.c]** funcion dup2

</br>

<h3>EXIT WAIT</h3>

* Cambiar todos exit y wait por exit(0) wait(0)

* sys_exit y sys_wait en **[sysproc.c]**

* cambiar entrada de las funciones exit y wait ->  **[user.h]**

* poner status en proceso **[proc.h]**

* wait(int * status) y exit(int status) en **[proc.c]** 

* En **[sh.c]** poner un print en la llamada main para saber codigo salida

</br>

---

<h2>P3</h2></br> 

<h3>RESERVA PEREZOSA DE MEMORIA Y EJ2</h3>

* Modificamos sys_sbrk() en **[sysproc.c]** para que no reserve memoria
* Ahora se generará un trap en **[trap.c]** porque no se ha reservado memoria por lo que cuando falte reservamos en la funcion trap :
    * Cuando CASE PGFAULT -> comprobamos que la direccion donde falla es válida y llamamos a reservar memoria
    * Este reservar memoria está en **[trap.c]** , creamos RESERVARPAGINA inspirado en allocuvm()

* En copyuvm en **[vm.c]** modificamos los panic por continue para que se sigan copiando las páginas aunque no tengan espacio reservado
* En **[exec.c]** introducimos un nuevo campo para guardar la posición de la guarda y posteriormente tratar en caso de que se pida esa dirección.

<h3>EXEC</h3>

* En **[exec.c]** cambiar el numero de tamaño resevado para la pila.

<h3>FREEMEM</h3>

* Creamos la syscall en **[sysfile.c]** y la declaramos en los distintos sitios **[syscall.h]**,**[defs.h]** ... , para saber todas -> Buscar dup2
* Macros de F_PAGES Y F_BYTES declarado en **[mmu.h]**
* **[mmu.h]** añadido a tfreemtest 
* Creamos la funcion freemem en **[kalloc.c]**



<br>

---

<h2>P4</h2></br>


<h3>BLOQUE DOBLEMENTE ENLAZADO</h3><br>

* **[fs.h]** 
    * Cambiar NDIRECT a uno menos
    * En inode actualizar -> uint addrs[NDIRECT+2]
    * Añadir BDI y calcular su tamaño ( 2 veces el BSI )
    * Actualizar el tamaño total del nodo [BSI+BDI+BD]
    * Implementar bmap con DOBLEMENTE_ENLANZADO -> dar bloque dentro de este nodo

* **[file.h]**
    * Cambiar addr[NDIRECT+2] en la estructura del inode

<br>

<h3>BORRADO BLOQUE DOBLEMENTE ENLAZADO</h3><br>

* **[fs.c]**
    * Modificar itrunc para borrar los bloques doblemente enlazados
        1. Liberar el bloque principal
        2. Recorrer los indirectos y liberar los doblemente enlazados de dentro
        3. Liberar los propios indirectos

</br>

---

*DUDAS*

</br>

*TODO*

1. DOCUMENTACION
