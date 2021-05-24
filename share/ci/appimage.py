import common as c
from config import *
import os
import sys
import subprocess as sub

if len(sys.argv) > 1 and sys.argv[1] == 'glibc_version':  # subcommand
    sub.run('ldd --version | head -n 1 | grep -Po "\\d\\.\\d\\d"', shell=True)
    exit(0)

artifact_name = '{}-{}.AppImage'.format(app_name, app_version)
if len(sys.argv) > 1 and sys.argv[1] == 'artifact_name':  # subcommand
    c.print(artifact_name)
    exit(0)
artifact_path = os.path.abspath(artifact_name)

c.print('>> Making appimage')

def get_bin(url):
    file = os.path.abspath(os.path.basename(url))
    c.download(url, file)
    c.run('chmod a+x {}'.format(file))
    return file

linuxdeploy_bin = get_bin('https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage')
get_bin('https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/continuous/linuxdeploy-plugin-qt-x86_64.AppImage')

os.chdir(build_dir)

install_dir = os.path.abspath('appdir')
c.recreate_dir(install_dir)
c.run('make INSTALL_ROOT={0} DESTDIR={0} install'.format(install_dir))

if c.is_inside_docker():
    c.run('{}  --appimage-extract'.format(linuxdeploy_bin))
    linuxdeploy_bin = os.path.abspath('squashfs-root/AppRun')

os.environ['VERSION'] = app_version
os.environ['QMAKE'] = os.path.abspath(qt_dir + '/bin/qmake') # for icu internal dependency

c.run('{} --appdir {} -d {}/usr/share/applications/*.desktop --plugin qt --output appimage'.format(
    linuxdeploy_bin, install_dir, install_dir))

c.run('mv {}-{}*.AppImage "{}"'.format(app_name, app_version, artifact_path))
