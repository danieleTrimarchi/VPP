import thirdPartyCompile

#from ipOptCompile import IpOptCompile 
#ipOpt= IpOptCompile()
# ipOpt.get()
 
# from BoostCompile import BoostCompile 
# boost= BoostCompile()
# boost.get()

# from CppUnitCompile import CppUnitCompile
# cppUnit = CppUnitCompile()
# cppUnit.get()
# 
# from EigenCompile import EigenCompile
# eigen = EigenCompile()
# eigen.get()

# from NlOptCompile import NlOptCompile
# nlOpt= NlOptCompile()
# nlOpt.get()

#from QtCompile import QtCompile
#qt= QtCompile()
#qt.get(False)
#qt.__package__()

from OpenCascadeCompile import TclCompile
tcl = TclCompile()
#tcl.__download__()
tcl.__compile__()