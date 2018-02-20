# coding: utf-8
from __future__ import division, print_function, absolute_import

APPNAME = 'SPTK'
VERSION = '3.11.0'

from waflib import Options
import sys
import os
import re
import waflib

subdirs = [
    'bin',
]

top = '.'
out = 'build'


def options(opt):
    opt.load('compiler_c')


def configure(conf):
    conf.load('compiler_c')

    conf.define('SPTK_VERSION', VERSION)
    conf.env['VERSION'] = VERSION

    if re.search('clang', conf.env.CC[0]):
        conf.env.append_unique(
            'CFLAGS',
            ['-O3', '-Wall', '-fno-common', '-Wstrict-prototypes'])
    elif re.search('gcc', conf.env.CC[0]):
        conf.env.append_unique(
            'CFLAGS',
            ['-O2', '-Wall', '-fno-common', '-Wstrict-prototypes'])
    elif re.search('cl.exe', conf.env.CC[0].lower()):
        conf.env.append_unique('CFLAGS', [''])
    else:
        raise RuntimeError("Not supported compiler: %s" % conf.env.CC[0])

    conf.env.HPREFIX = conf.env.PREFIX + '/include/SPTK'

    # check headers
    conf.check_cc(header_name="fcntl.h")
    conf.check_cc(header_name="limits.h")
    conf.check_cc(header_name="stdlib.h")
    conf.check_cc(header_name="string.h")

    conf.recurse(subdirs)

    print("""
SPTK has been configured as follows:

[Build information]
Package:                 {0}
build (compile on):      {1}
host endian:             {2}
Compiler:                {3}
Compiler version:        {4}
CFLAGS:                  {5}
""".format(
        APPNAME + '-' + VERSION,
        conf.env.DEST_CPU + '-' + conf.env.DEST_OS,
        sys.byteorder,
        conf.env.COMPILER_CC,
        '.'.join(conf.env.CC_VERSION),
        ' '.join(conf.env.CFLAGS)
    ))

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

    bld(source='SPTK.pc.in',
        prefix=bld.env['PREFIX'],
        exec_prefix='${prefix}',
        libdir=bld.env['LIBDIR'],
        libs=ls,
        includedir='${prefix}/include',
        PACKAGE=APPNAME,
        VERSION=VERSION)
