TOP=../..

include $(TOP)/configure/CONFIG

# -------------------------------
# Build an Diamond Support Module
# -------------------------------

LIBRARY_IOC += simHDF5Detector

# include files
INCLUDE += SimHDF5Detector.h
INCLUDE += SimHDF5FileReader.h

# installed into <top>/dbd
DBD += simHDF5Support.dbd

# The following are compiled and added to the support library
simHDF5Detector_SRCS += SimHDF5Detector.cpp
simHDF5Detector_SRCS += SimHDF5Reader.cpp
simHDF5Detector_SRCS += SimHDF5FileReader.cpp
simHDF5Detector_SRCS += SimHDF5MemoryReader.cpp

# We need to link against the EPICS Base libraries
simHDF5Detector_LIBS += asyn
simHDF5Detector_LIBS += ADBase
simHDF5Detector_LIBS += $(EPICS_BASE_IOC_LIBS)

USR_INCLUDES += $(HDF5)/include

include $(TOP)/configure/RULES
