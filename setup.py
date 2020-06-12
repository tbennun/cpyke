from setuptools import setup, find_packages

with open("README.md", "r") as fp:
    long_description = fp.read()

setup(
    name='cpyke',
    version='1.0',
    url='https://github.com/tbennun/cpyke',
    author='Tal Ben-Nun',
    author_email='tbennun@gmail.com',
    description='Easy integrated Python scripting embedded in C++',
    long_description=long_description,
    long_description_content_type='text/markdown',
    classifiers=[
        "Programming Language :: Python :: 3",
        "License :: OSI Approved :: BSD License",
        "Operating System :: OS Independent",
    ],
    python_requires='>=3.5',
    packages=find_packages(
        exclude=["*.tests", "*.tests.*", "tests.*", "tests"]),
    install_requires=[],
    extras_require={'testing': ['coverage']},
)
