# Build subfolders
Import('releaseEnv')

for subdir in ['main']:
    env.SConscript('%s/SConscript' % subdir, {'releaseEnv': releaseEnv})