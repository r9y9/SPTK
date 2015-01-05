APPNAME = 'SPTK'
VERSION = '3.8.2'

from waflib import Options
import sys
import os
import re
import waflib

subdirs = [
    'bin',
]

python_bindings = 'python'

top = '.'
out = 'build'

def options(opt):
    opt.load('compiler_c python')
    opt.add_option("--python",
                   action="store_true", dest="enable_python", default=False,
                   help="whether compile python bindings")

def configure(conf):
    conf.load('compiler_c python')

    conf.env['PYTHON_BINDINGS'] = conf.options.enable_python

    if conf.options.enable_python:
        conf.load('swig')
        if conf.check_swig_version() < (1, 2, 27):
            conf.fatal('this swig version is too old')

    conf.load('python')
    conf.check_python_version((2,4,2))
    conf.check_python_headers()

    conf.define('SPTK_VERSION', VERSION)
    conf.env['VERSION'] = VERSION

    if re.search('clang', conf.env.CC[0]):
        conf.env.append_unique(
            'CFLAGS',
            ['-O3', '-Wall', '-g'])
    elif re.search('gcc', conf.env.CC[0]):
        conf.env.append_unique(
            'CFLAGS',
            ['-O2', '-Wall', '-g'])

    conf.env.HPREFIX = conf.env.PREFIX + '/include/SPTK'

    # check headers
    conf.check_cc(header_name = "fcntl.h")
    conf.check_cc(header_name = "limits.h")
    conf.check_cc(header_name = "stdlib.h")
    conf.check_cc(header_name = "string.h")
    conf.check_cc(header_name = "strings.h")
    conf.check_cc(header_name = "sys/ioctl.h")

    conf.recurse(subdirs)
    if conf.options.enable_python:
        conf.recurse(python_bindings)

    print """
SPTK has been configured as follows:

[Build information]
Package:                 %s
build (compile on):      %s
host endian:             %s
Compiler:                %s
Compiler version:        %s
CFLAGS:                  %s
Generate python bindings %s
""" % (
        APPNAME + '-' + VERSION,
        conf.env.DEST_CPU + '-' + conf.env.DEST_OS,
        sys.byteorder,
        conf.env.COMPILER_CC,
        '.'.join(conf.env.CC_VERSION),
        ' '.join(conf.env.CFLAGS),
        conf.options.enable_python
        )

    conf.write_config_header('src/SPTK-config.h')

def build(bld):
    bld.recurse(subdirs)

    libs = []
    for tasks in bld.get_build_iterator():
        if tasks == []:
            break
        for task in tasks:
            if isinstance(task.generator, waflib.TaskGen.task_gen) and 'cshlib' in task.generator.features:
                libs.append(task.generator.target)
    ls = ''
    for l in set(libs):
        ls = ls + ' -l' + l
    ls += ' -lm'

    if bld.env["PYTHON_BINDINGS"]:
        bld.recurse(python_bindings)

    bld(source = 'SPTK.pc.in',
        prefix = bld.env['PREFIX'],
        exec_prefix = '${prefix}',
        libdir = bld.env['LIBDIR'],
        libs = ls,
        includedir = '${prefix}/include',
        PACKAGE = APPNAME,
        VERSION = VERSION)
