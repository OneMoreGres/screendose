import common as c
from config import *
import os
import sys

artifact_name = '{}-{}-{}.zip'.format(app_name, app_version, os_name)
if len(sys.argv) > 1 and sys.argv[1] == 'artifact_name':  # subcommand
    c.print(artifact_name)
    exit(0)
artifact_path = os.path.abspath(artifact_name)

c.print('>> Making win deploy')

pwd = os.getcwd()
os.chdir(build_dir)

env_cmd = c.get_msvc_env_cmd(bitness=bitness, msvc_version=msvc_version)
c.apply_cmd_env(env_cmd)
make_cmd = c.get_make_cmd()
install_dir = os.path.abspath(app_name)
install_cmd = '{0} INSTALL_ROOT="{1}" DESTDIR="{1}" install'.format(
    make_cmd, install_dir)
deploy_cmd = '{}/bin/windeployqt.exe "{}"'.format(qt_dir, install_dir)

c.run(install_cmd)
c.run(deploy_cmd)

c.archive(c.get_folder_files(os.path.relpath(install_dir)), artifact_path)
