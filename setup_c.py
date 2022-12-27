from setuptools import setup
from Cython.Build import cythonize

setup(
    name='ld_multi',
    ext_modules=cythonize("ld_func.py"),
    zip_safe=False,
)