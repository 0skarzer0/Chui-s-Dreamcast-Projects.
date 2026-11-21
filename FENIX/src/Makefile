# target=win32
# SDL=/win32

ifndef target
error:
	@echo "ERROR: Debe compilar con make target=N"
	@echo "       (N puede ser MAC, linux, GP32, DC o WIN32)"
endif

ifeq ($(target) ,GP32)
all:
	cd fxi/gp32; make

clean:
	cd fxi/gp32; make clean

else
ifeq ($(target) ,DC)

all:
	cd fsl; make -f Makefile.dc

clean:
	cd fsl; make -f Makefile.dc clean

else

all:
#	cd map; make
#	cd fpg; make
	cd fxc; make
	cd fxi; make 
	cd dll; make

clean:
#	(cd map; make clean)
#	(cd fpg; make clean)
	(cd fxc; make clean)
	(cd fxi; make clean)
	rm -f core *.dcb *.exe *.dll bin/fxi bin/fxc bin/*.exe

dist:
	make clean
	(cd .. ; tar zcvf fenix-src.tgz fenix)
	make debug=false ; mv bin dist ; make clean ; mv dist bin
	(cd .. ; tar zcvf fenix-linux-i386.tgz fenix \
	     --exclude fenix/src \
	     --exclude fenix/SDL)
	make clean
	(PATH=/win32/bin:$$PATH make target=win32 debug=false SDL=/win32 ; \
	 mv bin/* . ; cp /win32/lib/*.dll .)
	(cd .. ; zip -r fenix-win32.zip fenix/* ; \
	 zip -d fenix-win32.zip fenix/src/* fenix/src/*/* \
		fenix/SDL/*/*/* fenix/SDL/*/* fenix/SDL/* fenix/SDL)

backup:	clean
	tar zcvf ../divc.tgz .

endif
endif
