.c.obj:
	$(CC) /nologo /Fo$(@D)\  $(CFLAGS) /c $<
	
CC	= cl
CL	= link
CPU	= win32

SPTKCFG	= /I ..\include
SYSCFG	= /O2 /Ot /GL /FD /EHsc /MT /W3 /nologo /c /Zi /TC /D "NDEBUG" /D "HAVE_STRING_H" /D "WIN32" /D "_WINDOWS" /D "_CONSOLE" /D "_MBCS" /D "_CRT_SECURE_NO_WARNINGS"

!IFNDEF DOUBLE
SYSCFG  = $(SYSCFG) /D "FORMAT=\"float\""
!ELSE
SYSCFG  = $(SYSCFG) /D "FORMAT=\"double\""
!ENDIF

CFLAGS	= $(SYSCFG) $(SPTKCFG)

CORES = agexp.obj \
	cholesky.obj \
	fileio.obj \
	fillz.obj \
	getfp.obj \
	getmem.obj \
	gexp.obj \
	glog.obj \
	invert.obj \
	matrix.obj \
	movem.obj \
	mseq.obj \
	theq.obj \
	toeplitz.obj

MODULES = ..\bin\acep\_acep.obj \
	..\bin\acorr\_acorr.obj \
	..\bin\agcep\_agcep.obj \
	..\bin\amcep\_amcep.obj \
	..\bin\average\_average.obj \
	..\bin\b2mc\_b2mc.obj \
	..\bin\c2acr\_c2acr.obj \
	..\bin\c2ir\_c2ir.obj \
	..\bin\c2sp\_c2sp.obj \
	..\bin\clip\_clip.obj \
	..\bin\dct\_dct.obj \
	..\bin\df2\_df2.obj \
	..\bin\dfs\_dfs.obj \
	..\bin\fft\_fft.obj \
	..\bin\fft2\_fft2.obj \
	..\bin\fftcep\_fftcep.obj \
	..\bin\fftr\_fftr.obj \
	..\bin\fftr2\_fftr2.obj \
	..\bin\freqt\_freqt.obj \
	..\bin\gc2gc\_gc2gc.obj \
	..\bin\gcep\_gcep.obj \
	..\bin\glsadf\_glsadf.obj \
	..\bin\gmm\_gmm.obj \
	..\bin\gnorm\_gnorm.obj \
	..\bin\grpdelay\_grpdelay.obj \
	..\bin\histogram\_histogram.obj \
	..\bin\ifft\_ifft.obj \
	..\bin\ifft2\_ifft2.obj \
	..\bin\ifftr\_ifftr.obj \
	..\bin\ignorm\_ignorm.obj \
	..\bin\imsvq\_imsvq.obj \
	..\bin\ivq\_ivq.obj \
	..\bin\lbg\_lbg.obj \
	..\bin\levdur\_levdur.obj \
	..\bin\lmadf\_lmadf.obj \
	..\bin\lpc\_lpc.obj \
	..\bin\lpc2c\_lpc2c.obj \
	..\bin\lpc2lsp\_lpc2lsp.obj \
	..\bin\lpc2par\_lpc2par.obj \
	..\bin\lsp2lpc\_lsp2lpc.obj \
	..\bin\lspcheck\_lspcheck.obj \
	..\bin\lspdf\_lspdf.obj \
	..\bin\ltcdf\_ltcdf.obj \
	..\bin\mc2b\_mc2b.obj \
	..\bin\mcep\_mcep.obj \
	..\bin\mfcc\_mfcc.obj \
	..\bin\mgc2mgc\_mgc2mgc.obj \
	..\bin\mgc2sp\_mgc2sp.obj \
	..\bin\mgcep\_mgcep.obj \
	..\bin\mglsadf\_mglsadf.obj \
	..\bin\mlpg\_mlpg.obj \
	..\bin\mlsadf\_mlsadf.obj \
	..\bin\msvq\_msvq.obj \
	..\bin\norm0\_norm0.obj \
	..\bin\nrand\_nrand.obj \
	..\bin\par2lpc\_par2lpc.obj \
	..\bin\phase\_phase.obj \
	..\bin\poledf\_poledf.obj \
	..\bin\reverse\_reverse.obj \
	..\bin\rmse\_rmse.obj \
	..\bin\root_pol\_root_pol.obj \
	..\bin\smcep\_smcep.obj \
	..\bin\transpose/_transpose.obj \
	..\bin\uels\_uels.obj \
	..\bin\ulaw\_ulaw.obj \
	..\bin\vq\_vq.obj \
	..\bin\window\_window.obj \
	..\bin\zcross\_zcross.obj \
	..\bin\zerodf\_zerodf.obj

SPTK.lib: $(CORES) $(MODULES)
	lib /OUT:SPTK.lib $(CORES) $(MODULES)
	lib /list SPTK.lib

clean:
	del *.lib
	del $(CORES) 
	del $(MODULES)
