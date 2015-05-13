from iocbuilder import Device, AutoSubstitution, Architecture, SetSimulation
from iocbuilder.arginfo import *
from iocbuilder.modules.asyn import Asyn, AsynPort, AsynIP

from iocbuilder.modules.areaDetector import AreaDetector, _NDPluginBase, _ADBase, _ADBaseTemplate, simDetector

class _SimHDF5Detector(AutoSubstitution):
    TemplateFile= "simHDF5Detector.template"
    SubstitutionOverwrites = [_ADBaseTemplate]

class SimHDF5Detector(_ADBase):
    """Create an SimHDF5Detector detector"""
    _BaseTemplate = _ADBaseTemplate
    _SpecificTemplate = _SimHDF5Detector
    AutoInstantiate = True
    def __init__(self, DRIVER="CAM.SIM", BUFFERS=50, MEMORY=-1, **args):
        self.__super.__init__(**args)
        self.__dict__.update(locals())
        
    # __init__ arguments
    ArgInfo = _ADBase.ArgInfo + _BaseTemplate.ArgInfo + makeArgInfo(__init__,
        DRIVER = Simple('Name of the driver port', str),
        BUFFERS = Simple('Maximum number of NDArray buffers to be created for plugin callbacks', int),
        MEMORY  = Simple('Max memory to allocate, should be maxw*maxh*nbuffer for driver and all attached plugins', int))
    LibFileList = ['simHDF5Detector']
    DbdFileList = ['simHDF5Support']
    SysLibFileList = []
    MakefileStringList = []

    def Initialise(self):
        print '# SimHDF5DetectorConfig(portName, maxBuffers, maxMemory )'
        print 'SimHDF5DetectorConfig( %(PORT)10s, %(BUFFERS)10d, %(MEMORY)9d )' % self.__dict__



