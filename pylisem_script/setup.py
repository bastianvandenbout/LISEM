from setuptools import setup
import os

def package_files(directory):
    paths = []
    for (path, directories, filenames) in os.walk(directory):
        for filename in filenames:
            paths.append(os.path.join('..', path, filename))
    return paths
extra_files = package_files('lisem/lisem/bin/')
extra_files.append('lisem_python.pyd')
extra_files.append('_version.pyi')

setup(
  name = 'lisem',         
  packages = ['lisem'],  
  version = '0.1.0',
  license='gpl-3.0', 
  description = 'Lisem Integrated Spatial Earth modeller',
  author = 'Bastian van den Bout',
  author_email = 'b.vandenbout@utwente.nl',
  url = 'https://blog.utwente.nl/lisem/',
  keywords = ['Geo', 'Spatial', 'Science', 'Physically-based', 'Modelling', 'Flood', 'Landslide', 'Hydrology', 'Flow'], 
  install_requires=[
          'numpy',
          'ipython',
		  'ipywidgets>=7.1.0',
		  'widgetsnbextension>=3.1.0',
		  'notebook>=5.3.0'
      ],
  setup_requires=['wheel'],
  include_package_data = True,
  package_data={'': extra_files},
  classifiers=[
    'Development Status :: 3 - Alpha', 
    'Intended Audience :: Developers',  
    'License :: OSI Approved :: MIT License',  
    'Programming Language :: Python :: 3.8',  
    'Programming Language :: Python :: 3.9',
	'Operating System :: Microsoft :: Windows'
  ],
)