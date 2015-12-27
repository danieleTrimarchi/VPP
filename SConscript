# Build subfolders
Import('env')

for subdir in ['main']:
    env.SConscript('%s/SConscript' % subdir, {'env': env})