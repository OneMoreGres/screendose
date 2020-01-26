import os
import subprocess as sub
import urllib.request
from shutil import which
import zipfile
import tarfile
import functools
import shutil
import multiprocessing
import platform


print = functools.partial(print, flush=True)


def run(cmd, capture_output=False, silent=False):
    print('>> Running', cmd)
    if capture_output:
        result = sub.run(cmd, check=True, shell=True, universal_newlines=True,
                         stdout=sub.PIPE, stderr=sub.STDOUT)
        if not silent:
            print(result.stdout)
    else:
        if not silent:
            result = sub.run(cmd, check=True, shell=True)
        else:
            result = sub.run(cmd, check=True, shell=True,
                             stdout=sub.DEVNULL, stderr=sub.DEVNULL)
    return result


def download(url, out, force=False):
    print('>> Downloading', url, 'as', out)
    if not force and os.path.exists(out):
        print('>>', out, 'already exists')
        return
    out_path = os.path.dirname(out)
    if len(out_path) > 0:
        os.makedirs(out_path, exist_ok=True)
    urllib.request.urlretrieve(url, out)


def extract(src, dest):
    abs_path = os.path.abspath(src)
    print('>> Extracting', abs_path, 'to', dest)
    if len(dest) > 0:
        os.makedirs(dest, exist_ok=True)

    if src.endswith('.tar') or src.endswith('.tar', 0, src.rfind('.')):
        if which('tar'):
            sub.run('tar xf "{}" --keep-newer-files -C "{}"'.format(abs_path, dest),
                    check=True, shell=True)
        return

    if which('7z'):
        sub.run('7z x "{}" -o"{}"'.format(abs_path, dest),
                check=True, shell=True, input=b'S\n')
        return

    if which('cmake'):
        out = run('cmake -E tar t "{}"'.format(abs_path),
                  capture_output=True, silent=True)
        files = out.stdout.split('\n')
        already_exist = True
        for file in files:
            if not os.path.exists(os.path.join(dest, file)):
                already_exist = False
                break
        if already_exist:
            print('>> All files already exist')
            return
        sub.run('cmake -E tar xvf "{}"'.format(abs_path),
                check=True, shell=True, cwd=dest)
        return

    raise RuntimeError('No archiver to extract {} file'.format(src))


def get_folder_files(path):
    result = []
    for root, _, files in os.walk(path):
        for file in files:
            result.append(os.path.join(root, file))
    return result


def archive(files, out):
    print('>> Archiving', files, 'into', out)
    if out.endswith('.zip'):
        arc = zipfile.ZipFile(out, 'w', zipfile.ZIP_DEFLATED)
        for f in files:
            arc.write(f)
        arc.close()
        return

    if out.endswith('.tar.gz'):
        arc = tarfile.open(out, 'w|gz')
        for f in files:
            arc.add(f)
        arc.close()
        return

    raise RuntimeError('No archiver to create {} file'.format(out))


def symlink(src, dest):
    print('>> Creating symlink', src, '=>', dest)
    norm_src = os.path.normcase(src)
    norm_dest = os.path.normcase(dest)
    if os.path.lexists(norm_dest):
        os.remove(norm_dest)
    os.symlink(norm_src, norm_dest,
               target_is_directory=os.path.isdir(norm_src))


def recreate_dir(path):
    shutil.rmtree(path, ignore_errors=True)
    os.mkdir(path)


def add_to_path(entry, prepend=True):
    path_separator = ';' if platform.system() == "Windows" else ':'
    os.environ['PATH'] = entry + path_separator + os.environ['PATH']


def get_cpp_env_cmd(bitness='64', msvc_version=''):
    """Return environment setup command for running cpp compiler for current platform"""
    if platform.system() != "Windows":
        return 'true'

    msvc_path = 'C:/Program Files (x86)/Microsoft Visual Studio'
    if len(msvc_version) == 0:
        with os.scandir(msvc_path) as ver_it:
            version = next(ver_it, '')
            with os.scandir(msvc_path + '/' + version) as ed_it:
                msvc_version = version + '/' + next(ed_it, '')

    env_script = msvc_path + '/{}/VC/Auxiliary/Build/vcvars{}.bat'.format(
        msvc_version, bitness)
    return '"' + env_script + '"'


def get_make_cmd():
    """Return `make` command for current platform"""
    if platform.system() == "Windows":
        os.environ['CL'] = '/MP'
        return 'nmake'
    return 'make -j{}'.format(multiprocessing.cpu_count())


def is_inside_docker():
    """ Return True if running in a Docker container """
    with open('/proc/1/cgroup', 'rt') as f:
        return 'docker' in f.read()
