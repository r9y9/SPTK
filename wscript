APPNAME = 'SPTK'
VERSION = '3.6.0'

from waflib import Options
import sys
import os
import re
import waflib

subdirs = [
    'bin',
    'lib',
]

top = '.'
out = 'build'

def options(opt):
    opt.load('compiler_cxx')
    opt.add_option("--showenv",
                   action="store_true", dest="showenv", default=False,
                   help="show environmental variables")
    
    opt.recurse(subdirs)
    
def configure(conf):
    conf.load('compiler_cxx')
    
    conf.define('SPTK_VERSION', VERSION)
    conf.env['VERSION'] = VERSION

    ver = conf.env.CC_VERSION
    conf.env.append_unique(
        'CXXFLAGS',
        ['-O2', '-Wall', '-g', '-lm'])

    conf.env.HPREFIX = conf.env.PREFIX + '/include/SPTK'

    # check headers
    conf.check_cxx(header_name = 'fcntl.h')
    conf.check_cxx(header_name = 'limits.h')
    conf.check_cxx(header_name = 'stdlib.h')
    conf.check_cxx(header_name = 'string.h')
    conf.check_cxx(header_name = 'strings.h')
    conf.check_cxx(header_name = 'sys/ioctl.h')

    conf.recurse(subdirs)

    print """
SPTK has been configured as follows:

[Build information]
Package:                 %s
build (compile on):      %s
host endian:             %s
Compiler:                %s
Compiler version:        %s
CXXFLAGS:                %s
""" % (
        APPNAME + '-' + VERSION,
        conf.env.DEST_CPU + '-' + conf.env.DEST_OS,
        sys.byteorder,
        conf.env.COMPILER_CXX,
        '.'.join(conf.env.CC_VERSION),
        ' '.join(conf.env.CXXFLAGS)
        )

    if conf.options.showenv:
        print conf.env
    conf.write_config_header('bin/SPTK-config.h')
            
def build(bld):
    bld.install_files('${PREFIX}/include/SPTK', [
        'bin/SPTK-config.h',
    ])
    bld.recurse(subdirs)

    libs = []
    for tasks in bld.get_build_iterator():
        if tasks == []:
            break
        for task in tasks:
            if isinstance(task.generator, waflib.TaskGen.task_gen) and 'cxxshlib' in task.generator.features:
                libs.append(task.generator.target)
    ls = ''
    for l in set(libs):
        ls = ls + ' -l' + l

    bld(source = 'SPTK.pc.in',
        prefix = bld.env['PREFIX'],
        exec_prefix = '${prefix}',
        libdir = bld.env['LIBDIR'],
        libs = ls,
        includedir = '${prefix}/include',
        PACKAGE = APPNAME,
        VERSION = VERSION)
