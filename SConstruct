import fnmatch
import os

def get_all(folder,filetypes):
	matches = []
	for root, dirnames, filenames in os.walk(folder):
  		for filename in filter(lambda fname: any(map(lambda extension: fname.endswith(extension),filetypes)),filenames):
    			matches.append((os.path.join(root, filename)))
	return matches

def add_alternative_dir_to_all(dir,files):
	return map(lambda a: dir + a, files)

env = Environment()

src_directory = 'src'
filetypes = ['.cpp','.h','.hpp']
build_dir = 'build'

VariantDir(build_dir,src_directory,duplicate=0)

def get_files():
	source_files = get_all(src_directory,filetypes)
	build_files = map(lambda fname: fname.replace(src_directory,build_dir),source_files)
	glob_files = map(lambda a: Glob(a), build_files)
	return glob_files

env.Program(target = 'main',source = get_files())
