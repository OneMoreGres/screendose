from os import getenv, path

app_name = 'ScreenDose'
app_version = 'test'
with open(path.dirname(__file__) + '/../../version', 'rt') as f:
    app_version = f.readline().strip()

target_name = 'screendose'
with open(path.dirname(__file__) + '/../../version', 'rt') as f:
    app_version = f.readline().strip()
qt_version = '5.14.0'
qt_modules = ['qtbase', 'qttools', 'icu', 'qttranslations']
qt_dir = path.abspath('qt')

build_dir = path.abspath('build')
pro_file = path.abspath(path.dirname(__file__) + '/../../screendose.pro')
ts_files_dir = path.abspath(path.dirname(__file__) + '/../../translation')

os_name = getenv('OS', 'linux')
app_version += {'linux': '', 'macos': '-experimental',
                'win32': '-experimental', 'win64': '-experimental'}[os_name]
bitness = '32' if os_name == 'win32' else '64'
msvc_version = getenv('MSVC_VERSION', '2017/Community')
# artifact_ext = 'zip' if os_name.startswith('win') else 'tar.gz'
# artifact_name = '{}-{}-{}.{}'.format(app_name,
#                                      app_version, os_name, artifact_ext)

# echo 'print(app_name)' | cat ../1/action-test/share/ci/config.py - | python -
# env_script = 'true'
# if os_name.startswith('win'):
#     msvc_version = getenv('msvc_version', '2017/Community')
#     env_script = 'C:/Program Files (x86)/Microsoft Visual Studio/{}/VC/'
#     'Auxiliary/Build/vcvars{}.bat'.format(msvc_version, os_name[-2:])
