<h1>XV6 GUÍA</h1>



<h2>P2</h2></br> 


<h3>DUP2<h/3>

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

*DUDAS*

1. EXITWAIT ES CORRECTO¿¿¿¿ PREGUNTAR PROFESOR
2. SH DEVUELVE 0 SIEMPRE , CORRECTO ya que le pasamos siempre exit(0)?

</br>

*TODO*

1. Tutoria EXITWAIT
2. BIG FILES
3. LAZY PAGE ALLOCATION
4. DOCUMENTACION