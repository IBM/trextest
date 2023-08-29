project('trex', ['c'],
  version: '0.1',
  default_options:
    ['warning_level=3',
     'c_std=gnu99',
     'buildtype=custom',
     'optimization=2',
     'debug=false',
     'default_library=static'])

# Check if we're on a supported OS & arch
supported_arches = ['x86_64', 'ppc64', 'aarch64']
arch = target_machine.cpu_family()
os = target_machine.system()

if os == 'linux'
  if arch not in supported_arches
    error('CPU architecture ' + arch + ' is not currently supported.')
  endif
elif os == 'aix'
  # Force 64-bit mode for everything
  arch = 'ppc64'
  add_global_arguments(['-mpowerpc64', '-maix64', '-Wl,-b64'], language: 'c')
  add_global_link_arguments(['-mpowerpc64', '-maix64', '-Wl,-b64'], language: 'c')
else
  error('Operating system ' + os + ' is not currently supported.')
endif

# Use intel syntax for x86-64 assembly
if arch == 'x86_64'
  add_global_arguments('-masm=intel', language: 'c')
endif

compiler = meson.get_compiler('c')
code = '''#include <stdio.h>
int main(void) { printf("test\n"); return 0; }
'''

if compiler.run(code, name: 'basic compile').returncode() != 0
  error('Couldn\'t compile and run basic C program!')
endif

if os == 'linux'
  if compiler.run(code, args: '-static', name: 'compile with -static').compiled()
    add_global_arguments('-static', language: 'c')
    add_global_link_arguments('-static', language: 'c')
  else
    warning('Compiler doesn\'t support -static, some tests may malfunction!')
  endif
endif

if get_option('timer') == 'thread'
  add_global_arguments('-DUSE_TIMER_THREAD', language: 'c')
endif

inc = include_directories('include', 'include/' + arch)

# Defines 'libs'
subdir('src')

# Standard dependencies for executables
deps = declare_dependency(include_directories: inc, link_with: libs)

subdir('demos')
subdir('tests')
