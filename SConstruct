import fnmatch
import os
import argparse
from subprocess import Popen, PIPE
import subprocess

AddOption('--test',action='store_true',help='Run Unit Tests', default=False)
def get_all(folder,filetypes,filter_out_if_contains=[]):
	matches = []
	for root, dirnames, filenames in os.walk(folder):
  		for filename in filter(lambda fname: any(map(lambda extension: fname.endswith(extension),filetypes)),filenames):
    			matches.append((os.path.join(root, filename)))
        
        matches = filter(lambda file: not(any(map(lambda name_partial: name_partial in file,filter_out_if_contains))),matches)
	return matches

def add_alternative_dir_to_all(dir,files):
	return map(lambda a: dir + a, files)

env = Environment()

filetypes = ['.c','.cpp','.h','.hpp']

env.ParseConfig('pkg-config --cflags opencv')
env.Append(CPPFLAGS = '-O2 -std=c++0x -pthread -D ELPP_THREAD_SAFE -D ELPP_STACKTRACE_ON_CRASH')
env.Append(LINKFLAGS = '-O2 -std=c++0x')
env.Append(LIBS = ["-lpthread"])

src_directory = 'src'
build_dir = 'build'

test_directory = 'test'
test_build_dir = 'test_build'

def change_to_build_dir_and_glob(source_files,build_directory):
	build_files = map(lambda fname: fname.replace(src_directory,build_directory),source_files)
	glob_files = map(lambda a: Glob(a), build_files)
	return glob_files

OPENCV_FLAGS=subprocess.check_output(['pkg-config','--cflags','opencv'])
OPENCV_LIBS=subprocess.check_output(['pkg-config','--libs-only-l','opencv'])
OPENCV_LIBPATH=subprocess.check_output(['pkg-config','--libs-only-L','opencv'])
#env.Append(LIBPATH=OPENCV_FLAGS)

OPENCV_LINK_FLAGS = ' -rdynamic /usr/local/lib/libopencv_videostab.so.3.1.0 /usr/local/lib/libopencv_videoio.so.3.1.0 /usr/local/lib/libopencv_video.so.3.1.0 /usr/local/lib/libopencv_superres.so.3.1.0 /usr/local/lib/libopencv_stitching.so.3.1.0 /usr/local/lib/libopencv_shape.so.3.1.0 /usr/local/lib/libopencv_photo.so.3.1.0 /usr/local/lib/libopencv_objdetect.so.3.1.0 /usr/local/lib/libopencv_ml.so.3.1.0 /usr/local/lib/libopencv_imgproc.so.3.1.0 /usr/local/lib/libopencv_imgcodecs.so.3.1.0 /usr/local/lib/libopencv_highgui.so.3.1.0 /usr/local/lib/libopencv_flann.so.3.1.0 /usr/local/lib/libopencv_features2d.so.3.1.0 /usr/local/lib/libopencv_core.so.3.1.0 /usr/local/lib/libopencv_calib3d.so.3.1.0 /usr/local/lib/libopencv_features2d.so.3.1.0 /usr/local/lib/libopencv_ml.so.3.1.0 /usr/local/lib/libopencv_highgui.so.3.1.0 /usr/local/lib/libopencv_videoio.so.3.1.0 /usr/local/lib/libopencv_imgcodecs.so.3.1.0 /usr/local/lib/libopencv_flann.so.3.1.0 /usr/local/lib/libopencv_video.so.3.1.0 /usr/local/lib/libopencv_imgproc.so.3.1.0 /usr/local/lib/libopencv_core.so.3.1.0 -Wl,-rpath,/usr/local/lib '

env.Replace(LIBLINKPREFIX="")
env.Replace(LIBLINKSUFFIX="")
env.Append(LIBS=OPENCV_LINK_FLAGS.split())

if not GetOption('test'):
   VariantDir(build_dir,src_directory,duplicate=0)
   env.Program(target = 'main',source = change_to_build_dir_and_glob(get_all(src_directory,filetypes),build_dir))

else:
   VariantDir(test_build_dir,src_directory,duplicate=0)
   test = env.Program(target = 'test_main', source=change_to_build_dir_and_glob(get_all(src_directory,filetypes,['main.cpp']) + get_all(test_directory,filetypes),test_build_dir))  
   def finish(target,source,env):
       process = Popen(['./test_main'],stdout=PIPE)
       (output,err) = process.communicate()
       exit_code = process.wait()
       print(output)
   finish_command = Command('Run Test', [], finish)
   Depends(finish_command,test)
