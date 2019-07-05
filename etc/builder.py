from iocbuilder import Device, AutoSubstitution, Architecture, SetSimulation
from iocbuilder.arginfo import *
from iocbuilder.modules.asyn import Asyn, AsynPort, AsynIP

from iocbuilder.modules.ADCore import ADCore, ADBaseTemplate, makeTemplateInstance, includesTemplates

@includesTemplates(ADBaseTemplate)
class SimHDF5DetectorTemplate(AutoSubstitution):
    TemplateFile= "simHDF5Detector.template"

class SimHDF5Detector(AsynPort):
    """Create an SimHDF5Detector detector"""
    Dependencies = (ADCore,)
    # This tells xmlbuilder to use PORT instead of name as the row ID
    UniqueName = "PORT"
    _SpecificTemplate = SimHDF5DetectorTemplate
    def __init__(self, PORT, MEMORY = 0, **args):
        # Init the superclass (AsynPort)
        self.__super.__init__(PORT)
        # Update the attributes of self from the commandline args
        self.__dict__.update(locals())
        # Make an instance of our template
        makeTemplateInstance(self._SpecificTemplate, locals(), args)

    # __init__ arguments
    ArgInfo = ADBaseTemplate.ArgInfo + _SpecificTemplate.ArgInfo + makeArgInfo(__init__,
        PORT = Simple('Port name for the detector', str),
        MEMORY = Simple('Max memory to allocate, should be maxw*maxh*nbuffer for driver and all attached plugins', int))

    # Device attributes
    LibFileList = ['simHDF5Detector']
    DbdFileList = ['simHDF5Support']

    def Initialise(self):
        print '# SimHDF5DetectorConfig(portName, maxBuffers, maxMemory )'
        print 'SimHDF5DetectorConfig( %(PORT)10s, 0, %(MEMORY)9d )' % self.__dict__


