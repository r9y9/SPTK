CC		= cl
CL		= link
CPU		= win32

SPTKCFG	= /I..\include
SYSCFG	= /O2 /Ot /GL /FD /EHsc /MT /W3 /nologo /c /Zi /TC /D "NDEBUG" /D "HAVE_MEMSET" /D "HAVE_STRING_H" /D "WIN32" /D "_WINDOWS" /D "_CONSOLE" /D "_MBCS" /D "_CRT_SECURE_NO_WARNINGS"

!IFNDEF DOUBLE
SYSCFG	= $(SYSCFG) /D "FORMAT=\"float\""
!ELSE
SYSCFG	= $(SYSCFG) /D "FORMAT=\"double\""
!ENDIF

CFLAGS	= $(SYSCFG) $(SPTKCFG)

SPTKLIB	= ..\lib\SPTK.lib
SYSLIB	= kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib
LIBS	=  /NOLOGO /SUBSYSTEM:CONSOLE /MACHINE:X86 $(SPTKLIB) $(SYSLIB)

all: acep.exe acorr.exe agcep.exe amcep.exe \
	average.exe b2mc.exe bcp.exe bcut.exe c2acr.exe \
	c2ir.exe c2sp.exe cat2.exe cdist.exe clip.exe da.exe dct.exe\
	decimate.exe delay.exe delta.exe df2.exe dfs.exe dmp.exe ds.exe dtw.exe echo2.exe \
	excite.exe extract.exe fd.exe fdrw.exe fft.exe fft2.exe fftcep.exe fftr.exe fftr2.exe \
	fig.exe frame.exe freqt.exe gc2gc.exe gcep.exe glsadf.exe gmm.exe gmmp.exe gnorm.exe \
	grpdelay.exe histogram.exe idct.exe ifft.exe ifft2.exe ifftr.exe \
	ignorm.exe impulse.exe imsvq.exe interpolate.exe ivq.exe \
	lbg.exe levdur.exe linear_intpl.exe lmadf.exe lpc.exe lpc2c.exe lpc2lsp.exe \
	lpc2par.exe lsp2lpc.exe lspcheck.exe lspdf.exe ltcdf.exe mc2b.exe mcep.exe \
	merge.exe mfcc.exe mgc2mgc.exe mgc2sp.exe mgcep.exe mglsadf.exe minmax.exe mlpg.exe \
	mlsacheck.exe mlsadf.exe msvq.exe nan.exe norm0.exe nrand.exe pca.exe pcas.exe \
	par2lpc.exe phase.exe pitch.exe poledf.exe psgr.exe \
	ramp.exe rawtowav.exe reverse.exe rmse.exe root_pol.exe sin.exe smcep.exe snr.exe \
	sopr.exe spec.exe step.exe swab.exe symmetrize.exe train.exe transpose.exe uels.exe ulaw.exe \
	us.exe vc.exe vopr.exe vq.exe vstat.exe vsum.exe window.exe x2x.exe \
	zcross.exe zerodf.exe 

acep.exe : acep\acep.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

acorr.exe : acorr\acorr.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

agcep.exe : agcep\agcep.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

amcep.exe : amcep\amcep.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

average.exe : average\average.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

b2mc.exe : b2mc\b2mc.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

bcp.exe : bcp\bcp.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

bcut.exe : bcut\bcut.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

c2acr.exe : c2acr\c2acr.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

c2ir.exe : c2ir\c2ir.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

c2sp.exe : c2sp\c2sp.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

cat2.exe : cat2\cat2.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

cdist.exe : cdist\cdist.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

clip.exe : clip\clip.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

da.exe : da\dawrite.obj da\winplay.obj
	$(CC) $(CFLAGS) /c da\dawrite.c
	$(CC) $(CFLAGS) /c da\winplay.c
	$(CL) /LTCG /OUT:$@ $(LIBS) winmm.lib dawrite.obj winplay.obj

dct.exe : dct\dct.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj
    
decimate.exe : decimate\decimate.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

delay.exe : delay\delay.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

delta.exe : delta\delta.obj
        $(CC) $(CFLAGS) /c $(@B)\$(@B).c
        $(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

df2.exe : df2\df2.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

dfs.exe : dfs\dfs.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

dmp.exe : dmp\dmp.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

ds.exe : ds\ds.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

dtw.exe : dtw\dtw.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

echo2.exe : echo2\echo2.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

excite.exe : excite\excite.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

extract.exe : extract\extract.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

fd.exe : fd\fd.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

fdrw.exe : fig+fdrw\fdrw.obj fig+fdrw\plot.obj fig+fdrw\plsub.obj
	$(CC) $(CFLAGS) /c fig+fdrw\fig.c
	$(CC) $(CFLAGS) /c fig+fdrw\plot.c
	$(CC) $(CFLAGS) /c fig+fdrw\plsub.c
	$(CL) /LTCG /OUT:$@ $(LIBS) fdrw.obj plot.obj plsub.obj

fft.exe : fft\fft.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

fft2.exe : fft2\fft2.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

fftcep.exe : fftcep\fftcep.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

fftr.exe : fftr\fftr.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

fftr2.exe : fftr2\fftr2.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

fig.exe : fig+fdrw\fig.obj fig+fdrw\fig0.obj fig+fdrw\fig1.obj \
	fig+fdrw\plot.obj fig+fdrw\plsub.obj
	$(CC) $(CFLAGS) /c fig+fdrw\fig.c
	$(CC) $(CFLAGS) /c fig+fdrw\fig0.c
	$(CC) $(CFLAGS) /c fig+fdrw\fig1.c
	$(CC) $(CFLAGS) /c fig+fdrw\plot.c
	$(CC) $(CFLAGS) /c fig+fdrw\plsub.c
	$(CL) /LTCG /OUT:$@ $(LIBS) fig.obj fig0.obj fig1.obj plot.obj plsub.obj

frame.exe : frame\frame.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

freqt.exe : freqt\freqt.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

gc2gc.exe : gc2gc\gc2gc.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

gcep.exe : gcep\gcep.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

glsadf.exe : glsadf\glsadf.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

gmm.exe : gmm\gmm.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

gmmp.exe : gmm\gmmp.obj
	$(CC) $(CFLAGS) /c gmm\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

gnorm.exe : gnorm\gnorm.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

grpdelay.exe : grpdelay\grpdelay.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

histogram.exe : histogram\histogram.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

idct.exe : idct\idct.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

ifft.exe : ifft\ifft.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

ifft2.exe : ifft2\ifft2.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

ifftr.exe : ifftr\ifftr.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

ignorm.exe : ignorm\ignorm.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

impulse.exe : impulse\impulse.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

imsvq.exe : imsvq\imsvq.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

interpolate.exe : interpolate\interpolate.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

ivq.exe : ivq\ivq.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

lbg.exe : lbg\lbg.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

levdur.exe : levdur\levdur.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

linear_intpl.exe : linear_intpl\linear_intpl.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

lmadf.exe : lmadf\lmadf.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

lpc.exe : lpc\lpc.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

lpc2c.exe : lpc2c\lpc2c.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

lpc2lsp.exe : lpc2lsp\lpc2lsp.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

lpc2par.exe : lpc2par\lpc2par.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

lsp2lpc.exe : lsp2lpc\lsp2lpc.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

lspcheck.exe : lspcheck\lspcheck.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

lspdf.exe : lspdf\lspdf.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

ltcdf.exe : ltcdf\ltcdf.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

mc2b.exe : mc2b\mc2b.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

mcep.exe : mcep\mcep.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

merge.exe : merge\merge.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

mfcc.exe : mfcc\mfcc.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

mgc2mgc.exe : mgc2mgc\mgc2mgc.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

mgc2sp.exe : mgc2sp\mgc2sp.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

mgcep.exe : mgcep\mgcep.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

mglsadf.exe : mglsadf\mglsadf.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

minmax.exe : minmax\minmax.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

mlpg.exe : mlpg\mlpg.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

mlsacheck.exe : mlsacheck\mlsacheck.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

mlsadf.exe : mlsadf\mlsadf.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

msvq.exe : msvq\msvq.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

nan.exe : nan\nan.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

norm0.exe : norm0\norm0.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

nrand.exe : nrand\nrand.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

par2lpc.exe : par2lpc\par2lpc.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

pca.exe : pca\pca.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

pcas.exe : pca\pcas.obj
	$(CC) $(CFLAGS) /c pca\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

phase.exe : phase\phase.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

pitch.exe : pitch\pitch.obj pitch\snack\jkGetF0.obj pitch\snack\sigproc.obj \
	pitch\swipe\swipe.obj pitch\swipe\vector.obj
	$(CC) $(CFLAGS) /c pitch\pitch.c
	$(CC) $(CFLAGS) /c pitch\snack\jkGetF0.c
	$(CC) $(CFLAGS) /c pitch\snack\sigproc.c
	$(CC) $(CFLAGS) /c pitch\swipe\swipe.c
	$(CC) $(CFLAGS) /c pitch\swipe\vector.c
	$(CL) /LTCG /OUT:$@ $(LIBS) pitch.obj jkGetF0.obj sigproc.obj swipe.obj vector.obj

poledf.exe : poledf\poledf.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

psgr.exe : psgr\psgr.obj psgr\dict.obj psgr\plot.obj psgr\eps.obj
	$(CC) $(CFLAGS) /c psgr\psgr.c
	$(CC) $(CFLAGS) /c psgr\dict.c
	$(CC) $(CFLAGS) /c psgr\plot.c
	$(CL) /LTCG /OUT:$@ $(LIBS) psgr.obj dict.obj plot.obj eps.obj

ramp.exe : ramp\ramp.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

rawtowav.exe : rawtowav\rawtowav.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

reverse.exe : reverse\reverse.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

rmse.exe : rmse\rmse.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

root_pol.exe : root_pol\root_pol.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

sin.exe : sin\sin.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

smcep.exe : smcep\smcep.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

snr.exe : snr\snr.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

sopr.exe : sopr\sopr.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

spec.exe : spec\spec.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

step.exe : step\step.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

swab.exe : swab\swab.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

symmetrize.exe : symmetrize\symmetrize.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

train.exe : train\train.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

transpose.exe : transpose\transpose.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

uels.exe : uels\uels.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

ulaw.exe : ulaw\ulaw.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

us.exe : us\us.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

vc.exe : vc\vc.obj vc\_vc.obj vc\hts_engine_API\HTS_misc.obj vc\hts_engine_API\HTS_pstream.obj \
	vc\hts_engine_API\HTS_sstream.obj
	$(CC) $(CFLAGS) /c vc\vc.c
	$(CC) $(CFLAGS) /c vc\_vc.c
	$(CC) $(CFLAGS) /c vc\hts_engine_API\HTS_misc.c
	$(CC) $(CFLAGS) /c vc\hts_engine_API\HTS_pstream.c
	$(CC) $(CFLAGS) /c vc\hts_engine_API\HTS_sstream.c
	$(CL) /LTCG /OUT:$@ $(LIBS) vc.obj _vc.obj HTS_misc.obj HTS_pstream.obj HTS_sstream.obj

vopr.exe : vopr\vopr.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

vq.exe : vq\vq.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

vstat.exe : vstat\vstat.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

vsum.exe : vsum\vsum.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

window.exe : window\window.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

x2x.exe : x2x\x2x.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

xgr.exe : xgr\xgr.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

zcross.exe : zcross\zcross.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

zerodf.exe : zerodf\zerodf.obj
	$(CC) $(CFLAGS) /c $(@B)\$(@B).c
	$(CL) /LTCG /OUT:$@ $(LIBS) $(@B).obj

clean:	
	del *.exe
	del *.obj
