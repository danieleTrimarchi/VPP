import thirdPartyCompile as tp

# from ipOptCompile import IpOptCompile 
# ipOpt= IpOptCompile()
# ipOpt.get()
# 
# from BoostCompile import BoostCompile 
# boost= BoostCompile()
# boost.get()
# 
# from CppUnitCompile import CppUnitCompile
# cppUnit = CppUnitCompile()
# cppUnit.get()
# 
from EigenCompile import EigenCompile
eigen = EigenCompile()
eigen.__compile__()
eigen.__package__()
eigen.__test__()

# from NlOptCompile import NlOptCompile
# nlOpt= NlOptCompile()
# nlOpt.__compile__()
# nlOpt.__package__()
# nlOpt.__test__()