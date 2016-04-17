# Define a common build environment
common_env = Environment()

# Our release build is derived from the common build environment...
release_env = common_env.Clone()

# ... and adds a RELEASE preprocessor symbol ...
release_env.Append(CPPDEFINES=['RELEASE'])

# ... and release builds end up in the "build/release" dir
release_env.VariantDir('build/release', 'main', duplicate=0)

# Define the location of the third_party
#release_env.Append(third_party_dir=['/Users/dtrimarchi/third_party'])

# We define our debug build environment in a similar fashion...
#debug_env = common_env.Clone()
#debug_env.Append(CPPDEFINES=['DEBUG'])
#debug_env.VariantDir('build/debug', 'src')

# Now that all build environment have been defined, let's iterate over
# them and invoke the lower level SConscript files.
#for mode, env in dict(release=release_env, 
#                   debug=debug_env).iteritems():
# print 'Mode= ', mode,'  env = ', env
# env.SConscript('build/%s/SConscript' % mode, {'env': env})
 
mode = 'release'
env = release_env
env.SConscript('build/%s/SConscript' % mode,{'env': env})