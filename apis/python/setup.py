import setuptools

with open('README.md') as f:
    long_description = f.read()

umo_module = setuptools.Extension(
    'umo',
    include_dirs = ['../../include'],
    extra_objects = ['cpp_build/libumo.a'],
    sources = ['umo.cpp']
)

setuptools.setup (
    name = 'umo',
    author = 'Gabriel Gouvine',
    author_email = 'gabriel.gouvine_umo@m4x.org',
    url = "https://github.com/Coloquinte/umo",
    version = '0.0.1',
    description = 'Modeler for mathematical programming solvers',
    test_suite = "test.TestUmoApi",
    ext_modules = [umo_module],
    classifiers=[
        "Operating System :: POSIX :: Linux",
        "License :: OSI Approved :: MIT License",
        "Intended Audience :: Science/Research",
        "Intended Audience :: Developers",
        "Development Status :: 2 - Pre-Alpha",
    ],
    long_description=long_description,
    long_description_content_type='text/markdown'
)
