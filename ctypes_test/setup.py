from setuptools import setup, Extension

setup(
    ext_modules=[Extension('run_execution_on_graph', ['clib.cpp'],),],
)

