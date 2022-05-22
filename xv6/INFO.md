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


<h3>BLOQUE DOBLEMENTE ENLAZADO</h3><br>

* **[fs.h]** 
    * Cambiar NDIRECT a uno menos
    * En inode actualizar -> uint addrs[NDIRECT+2]
    * Añadir BDI y calcular su tamaño ( 2 veces el BSI )
    * Actualizar el tamaño total del nodo [BSI+BDI+BD]
    <br>
    * Implementar bmap con DOBLEMENTE_ENLANZADO -> dar bloque dentro de este nodo

* **[file.h]**
    * Cambiar addr[NDIRECT+2] en la estructura del inode

</br>

---

*DUDAS*

1. EXITWAIT ES CORRECTO¿¿¿¿ PREGUNTAR PROFESOR
2. SH DEVUELVE 0 SIEMPRE , CORRECTO ya que le pasamos siempre exit(0)?
3. BP->DATA **[fs.c]** que es data? a = (uint*)bp->data;

</br>

*TODO*

1. Tutoria EXITWAIT
2. BIG FILES ( COMPROBAR EJ1 Y HACER EL 2)
3. LAZY PAGE ALLOCATION
4. DOCUMENTACION