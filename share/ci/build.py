import common as c
from config import *
import os

c.print('>> Building {} on {}'.format(app_name, os_name))

c.add_to_path(os.path.abspath(qt_dir + '/bin'))
os.environ['VERSION'] = app_version
c.recreate_dir(build_dir)

env_cmd = c.get_cpp_env_cmd(bitness=bitness, msvc_version=msvc_version)
qmake_cmd = 'qmake "{}"'.format(pro_file)
make_cmd = c.get_make_cmd()

os.chdir(build_dir)
c.run('{} && {} && {}'.format(env_cmd, qmake_cmd, make_cmd))
os.chdir('..')
