project(
    'prcrun',
    ['cpp'],
    version : '1.0',
    license : 'GPL-2.0',
    default_options : [
        'cpp_std=c++11',
    ],
)

cpp_args = [
    '-Wall',
    '-fno-rtti',
    '-fno-exceptions',
    '-O2',
]

app_src = [
    'main.cpp',
]

executable(
    'prcrun',
    cpp_args : cpp_args,
    sources : app_src,
    install : false,
)


