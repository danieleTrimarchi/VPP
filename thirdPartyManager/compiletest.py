import thirdPartyCompile as tp

# from ipOptCompile import IpOptCompile 
# ipOpt= IpOptCompile()
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

from QtCompile import QtCompile
qt= QtCompile()
#qt.__compile__()
#qt.__package__()
qt.__test__()