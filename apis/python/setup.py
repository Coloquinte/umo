from setuptools import setup, Extension

umo_module = Extension(
    'umo',
    include_dirs = ['../../include'],
    extra_objects = ['../../build/libumo.a'],
    sources = ['umo.cpp']
)

setup (
    name = 'umo',
    author = 'Gabriel Gouvine',
    author_email = 'gabriel.gouvine_umo@m4x.org',
    version = '0.0.1',
    description = 'Universal modeler',
    test_suite = "test.TestUmoApi",
    ext_modules = [umo_module],
    classifiers=[
        "Operating System :: POSIX :: Linux",
        "License :: OSI Approved :: MIT License",
        "Intended Audience :: Science/Research",
        "Intended Audience :: Developers"
    ]
)
