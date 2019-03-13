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

#from OpenCascadeCompile import TclCompile
#tcl = TclCompile()
#tcl.get(False)

from OpenCascadeCompile import FreeTypeCompile
ftc = FreeTypeCompile()
#ftc.__compile__()
ftc.__package__()


