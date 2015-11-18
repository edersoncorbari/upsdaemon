
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

        http://www.ederbs.org/projects/upsdaemon/


UpsDaemon:

   The upsdaemon is a simple program to monitor os (UPS) series APC-Pro.
   When the power fail, the UpsDaemon it effects shutdown the system.

Disclaimer:

   If it will be BSDs system I recommend that it compiles UpsDaemon 
   for ports.

    Ports In the FreeBSD:

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


Ederson de Moura - ederbsd@ederbs.org

~:)
