#!/usr/bin/env python
#
# This is the qct setup script.
#
# './setup.py install', or
# './setup.py --help' for more options

import sys, os
if not hasattr(sys, 'version_info') or sys.version_info < (2, 4, 0, 'final'):
    raise SystemExit, "Qct requires python 2.4 or later."

extra = {}
try:
    # to generate qct MSI installer, you run python setup.py bdist_msi
    from setuptools import setup
    if os.name in ['nt']:
        # the msi will automatically install the qct.py plugin into hgext
        extra['data_files'] = [('lib/site-packages/hgext', ['hgext/qct.py']),
                ('mercurial/hgrc.d', ['qct.rc']),
                ('share/qct', ['doc/qct.1.html', 'README', 'README.mercurial'])]
        extra['scripts'] = ['win32/qct_postinstall.py']
    else:
        extra['scripts'] = ['qct']
except ImportError:
    from distutils.core import setup
    extra['scripts'] = ['qct']

from distutils.command.build import build
from distutils.spawn import find_executable, spawn
from qctlib.version import qct_version

try:
    import py2app
    extra['app'] = ['qct']
except ImportError:
    pass

try:
    # to generate qct.exe, you need python 2.5, Qt 4.2, PyQt4, and py2exe
    # then execute:  python setup.py py2exe
    import py2exe
    extra['console'] = ['qct']
except ImportError:
    pass

opts = {
   "py2exe" : {
       "excludes" : "pywin,pywin.dialogs,pywin.dialogs.list",
       "includes" : "sip"
       # ",PyQt4._qt"
   }
}

class QctBuild(build):
    def compile_ui(self, ui_file, py_file):
        # Search for pyuic4 in python bin dir, then in the $Path.
        try:
            from PyQt4 import uic
            fp = open(py_file, 'w')
            uic.compileUi(ui_file, fp)
            fp.close()
        except Exception, e:
            print 'Unable to compile user interface', e
            return

    def run(self):
        if not os.path.exists('qctlib/ui_dialog.py'):
            self.compile_ui('qctlib/dialog.ui', 'qctlib/ui_dialog.py')
        if not os.path.exists('qctlib/ui_preferences.py'):
            self.compile_ui('qctlib/preferences.ui', 'qctlib/ui_preferences.py')
        if not os.path.exists('qctlib/ui_select.py'):
            self.compile_ui('qctlib/select.ui', 'qctlib/ui_select.py')
        build.run(self)

setup(name='qct',
      version=qct_version,
      download_url='http://qct.sourceforce.net/qct-' + qct_version + '.tar.gz',
      author='Steve Borho',
      author_email='steve@borho.org',
      url='http://qct.sourceforge.net',
      description='Commit Tool',
      long_description='''
      Qct Qt4/PyQt based commit tool
      Primary goals:

      1) Cross-Platform (Linux, Windows-Native, MacOS)
      2) Be VCS agnostic
      3) Good keyboard navigation, keep the typical work-flow simple
      4) Universal change selection

      Currently supports Mercurial, Bazaar, CVS, and Monotone repositories.''',
      license='GNU GPL2',
      packages=['qctlib', 'qctlib/vcs'],
      cmdclass = { 'build' : QctBuild },  # define custom build class
      classifiers=['Development Status :: 4 - Beta',
                   'License :: OSI Approved :: GNU General Public License (GPL)',
                   'Intended Audience :: Developers',
                   'Intended Audience :: Science/Research',
                   'Intended Audience :: System Administrators',
                   'Natural Language :: English',
                   'Environment :: Console',
                   'Environment :: MacOS X',
                   'Operating System :: OS Independent',
                   'Operating System :: Microsoft :: Windows',
                   'Operating System :: POSIX',
                   'Operating System :: Unix',
                   'Programming Language :: Python',
                   'Topic :: Software Development',
                   'Topic :: System :: Systems Administration',
                   'Topic :: Software Development :: Version Control'],
      platforms='All',
      options=opts,
      **extra)
