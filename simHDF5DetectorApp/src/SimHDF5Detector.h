/*
 * SimHDF5Detector.h
 *
 *  Created on: 13 Apr 2015
 *      Author: gnx91527
 */

#ifndef ADSIMAPP_SRC_SimHDF5Detector_H_
#define ADSIMAPP_SRC_SimHDF5Detector_H_

#include <epicsEvent.h>
#include "ADDriver.h"
#include "SimHDF5FileReader.h"
#include "SimHDF5MemoryReader.h"
#include "SimHDF5Reader.h"

#define str_ADSim_Filename        "ADSim_Filename"
#define str_ADSim_FileValid       "ADSim_FileValid"
#define str_ADSim_NumOfDsets      "ADSim_NumOfDsets"
#define str_ADSim_DsetIndex       "ADSim_DsetIndex"
#define str_ADSim_DsetName        "ADSim_DsetName"
#define str_ADSim_DsetNumDims     "ADSim_DsetNumDims"
#define str_ADSim_DsetDim1        "ADSim_DsetDim1"
#define str_ADSim_DsetDim2        "ADSim_DsetDim2"
#define str_ADSim_DsetDim3        "ADSim_DsetDim3"
#define str_ADSim_DsetDim4        "ADSim_DsetDim4"
#define str_ADSim_DsetDim5        "ADSim_DsetDim5"
#define str_ADSim_DsetDim6        "ADSim_DsetDim6"
#define str_ADSim_XDim            "ADSim_XDim"
#define str_ADSim_YDim            "ADSim_YDim"
#define str_ADSim_DsetPath        "ADSim_DsetPath"

class SimHDF5Detector : public ADDriver
{
public:
  SimHDF5Detector(const char *portName,
              int maxBuffers,
              size_t maxMemory,
              int priority,
              int stackSize);
  virtual ~SimHDF5Detector();

  void acqTask();
  virtual asynStatus writeInt32(asynUser *pasynUser, epicsInt32 value);
  virtual asynStatus writeOctet(asynUser *pasynUser, const char *value, size_t nChars, size_t *nActual);

protected:
  int ADSim_Filename;         // Filename of HDF5 file to use for data source
  #define FIRST_ADSIM_DETECTOR_PARAM ADSim_Filename
  int ADSim_FileValid;        // Is the specified filename a valid file
  int ADSim_NumOfDsets;       // Number of datasets found within HDF5 file
  int ADSim_DsetIndex;        // Index of currently selected dataset
  int ADSim_DsetName;         // Name of currently selected dataset
  int ADSim_DsetNumDims;      // Number of dimensions for the currently selected dataset
  int ADSim_DsetDim1;         // Size of dimension 1 for the currently selected dataset
  int ADSim_DsetDim2;         // Size of dimension 2 for the currently selected dataset
  int ADSim_DsetDim3;         // Size of dimension 3 for the currently selected dataset
  int ADSim_DsetDim4;         // Size of dimension 4 for the currently selected dataset
  int ADSim_DsetDim5;         // Size of dimension 5 for the currently selected dataset
  int ADSim_DsetDim6;         // Size of dimension 6 for the currently selected dataset
  int ADSim_XDim;             // Selected dimension to represent the image X
  int ADSim_YDim;             // Selected dimension to represent the image Y
  int ADSim_DsetPath;         // Path of currently selected dataset
  #define LAST_ADSIM_DETECTOR_PARAM ADSim_DsetPath

private:

  asynStatus readImage(int index);
  asynStatus loadFile();
  asynStatus readDatasetInfo();
  asynStatus updateSourceImage();
  asynStatus verifySizes();
  asynStatus setArraySizes();

  std::tr1::shared_ptr<SimHDF5Reader> fileReader;      // Filereader used for importing HDF5 datasets
  bool validFile;                                      // Is the current file valid?
  epicsEventId startEventId;                           // Event used to signal acquisition start
  epicsEventId stopEventId;                            // Event used to signal acquisition stop
  NDArray *pRaw;                                       // Pointer to NDArrays ready to process

};

#define NUM_ADSIM_DETECTOR_PARAMS ((int)(&LAST_ADSIM_DETECTOR_PARAM - &FIRST_ADSIM_DETECTOR_PARAM + 1))

#endif /* ADSIMAPP_SRC_SimHDF5Detector_H_ */
