#Remember the macports command:  port contents packageName

THIRD_PARTY_INCS=[
			'/Users/daniele/third_party/eigen-3.2.7',
			'/Users/daniele/third_party/SuiteSparse-4.4.6/UMFPACK/Include',
			'/Users/daniele/third_party/SuiteSparse-4.4.6/SuiteSparse_config',
			'/Users/daniele/third_party/SuiteSparse-4.4.6/AMD/Include',
			'/Users/daniele/third_party/boost_1_59_0',
			'/Users/daniele/third_party/nlopt-2.4.2/api',
			'/opt/local/include',												# macPorts and nlopt include path
			'/opt/local/share/plplot5.11.1/examples/c',							#include path for plplot example
			'/opt/local/include/plplot'
		]



THIRD_PARTY_LIBS=[
			'goto2_penrynp-r1.13',
			'amd',
			'cholmod',
			'suitesparseconfig',
			'colamd',
			'umfpack',
			'plplotcxx',  #libs required by plplot
			'plplot',
			'nlopt'
		]
			
					
THIRD_PARTY_LIBPATH=[
			'/Users/daniele/third_party/SuiteSparse-4.4.6/AMD/Lib',
			'/Users/daniele/third_party/GotoBLAS2',
			'/Users/daniele/third_party/SuiteSparse-4.4.6/CHOLMOD/Lib',
			'/Users/daniele/third_party/SuiteSparse-4.4.6/SuiteSparse_config',
			'/Users/daniele/third_party/SuiteSparse-4.4.6/COLAMD/Lib',
			'/Users/daniele/third_party/SuiteSparse-4.4.6/UMFPACK/Lib',
			'/opt/local/lib' # macPorts and lnopt lib path
		]


#This must be a way to define the path where the executable searches for the dyLib, but for the moment it does not seems to work
env = DefaultEnvironment()


env.Program('VPP',
		['main.cpp','Plotter.cpp'],
		CPPPATH = THIRD_PARTY_INCS,
		LIBS=THIRD_PARTY_LIBS,
		LIBPATH=THIRD_PARTY_LIBPATH
	)
