<pre>
              Simple Program UPS for Unix (V.1.0) 

                       June 17, 2006

                     ..:: Readme ::..
                     
                          /--/|   
       	                 /  / |   
      	                /__/  |   
                        |==|  |   
      	                |U |  |   
      	                |N |  |   
      	                |I |  |   
                        |X | /    
                        |==|/     
      	                `""`
</pre>

More information abou this program:

 * [https://www.gsp.com/cgi-bin/man.cgi?section=8&topic=upsdaemon](https://www.gsp.com/cgi-bin/man.cgi?section=8&topic=upsdaemon)
 * [https://www.freebsd.org/cgi/ports.cgi?query=upsdaemon&stype=all](https://www.freebsd.org/cgi/ports.cgi?query=upsdaemon&stype=all)

UpsDaemon:

   The upsdaemon is a simple program to monitor os (UPS) series APC-Pro.
   When the power fail, the UpsDaemon it effects shutdown the system.

Disclaimer:

   If it will be BSDs system I recommend that it compiles UpsDaemon 
   for ports.

    Ports in the FreeBSD:

       - cd /usr/ports/sysutils/upsdaemon
       - make install clean 
    
Builting:

   If exactly thus to want to compile without the use of ports,
   It continues.     

    Install: To build on Unix

       - mv Makefile.orig  Makefile   
       
       - make

       - make install

       - make clean
 
    
    Deinstall: To deinstalling on Unix

       - make deinstall


Ederson de Moura Corbari - ecorbari at protonmail.com
