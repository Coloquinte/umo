from distutils.core import setup, Extension

umo_module = Extension(
    'umo',
    include_dirs = ['../../include'],
    libraries = ['umo'],
    library_dirs = ['../../build'],
    sources = ['umo.c']
)

setup (
    name = 'umo',
    author = 'Gabriel Gouvine',
    author_email = 'gabriel.gouvine_umo@m4x.org',
    version = '0.0.1',
    description = 'Universal modeler',
    ext_modules = [umo_module]
)
