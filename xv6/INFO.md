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

1. EXITWAIT ES CORRECTO¿¿¿¿ PREGUNTAR PROFESOR
2. SH DEVUELVE 0 SIEMPRE , CORRECTO ya que le pasamos siempre exit(0)?

</br>

*TODO*

1. Tutoria EXITWAIT
2. LAZY PAGE ALLOCATION
4. DOCUMENTACION
