noinst_DATA =  doom_icon.c heretic_icon.c hexen_icon.c strife_icon.c setup_icon.c

if HAVE_PYTHON

doom_icon.c: doom.png convert-icon
	./convert-icon doom.png doom_icon.c

heretic_icon.c: heretic.png convert-icon
	./convert-icon heretic.png heretic_icon.c

hexen_icon.c: hexen.png convert-icon
	./convert-icon hexen.png hexen_icon.c

strife_icon.c: strife.png convert-icon
	./convert-icon strife.png strife_icon.c

setup_icon.c: setup.png convert-icon
	./convert-icon setup.png setup_icon.c

endif
