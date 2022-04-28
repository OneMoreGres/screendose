from os import getenv, path

app_name = 'ScreenDose'
app_version = 'test'
with open(path.dirname(__file__) + '/../../version', 'rt') as f:
    app_version = f.readline().strip()

target_name = 'screendose'
with open(path.dirname(__file__) + '/../../version', 'rt') as f:
    app_version = f.readline().strip()
qt_version = '6.2.3'
qt_modules = ['qtbase', 'qttools', 'icu', 'qttranslations']
qt_dir = path.abspath('qt')

build_dir = path.abspath('build')
pro_file = path.abspath(path.dirname(__file__) + '/../../screendose.pro')
ts_files_dir = path.abspath(path.dirname(__file__) + '/../../translations')

os_name = getenv('OS', 'linux')
app_version += {'linux': '', 'macos': '-experimental',
                'win32': '-experimental', 'win64': '-experimental'}[os_name]
bitness = '32' if os_name == 'win32' else '64'
msvc_version = getenv('MSVC_VERSION', 'C:/Program Files (x86)/Microsoft Visual Studio/2019/Community')

build_type = 'release' # 'debug'
