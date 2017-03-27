/*
 * SimHDF5Reader.h
 *
 *  Created on: 28 Jan 2016
 *      Author: gnx91527
 */

#ifndef SIMHDF5DETECTORAPP_SRC_SIMHDF5READER_H_
#define SIMHDF5DETECTORAPP_SRC_SIMHDF5READER_H_

#include <hdf5.h>
#include <string>
#include <map>
#include <vector>
#include <tr1/memory>
#include "NDArray.h"

class SimHDF5Reader
{
public:
  SimHDF5Reader ();
  virtual ~SimHDF5Reader ();

  virtual void setFilename(const std::string &filename) = 0;
  virtual std::string getFilename() = 0;
  virtual bool validateFilename() = 0;
  virtual int fileExists() = 0;
  virtual void loadFile() = 0;
  virtual void unloadFile() = 0;
  virtual std::vector<std::string> getDatasetKeys() = 0;
  virtual std::vector<int> getDatasetDimensions(const std::string& dname) = 0;
  virtual NDDataType_t getDatasetType(const std::string& dname) = 0;
  virtual void prepareToReadDataset(const std::string& dname) = 0;
  virtual void readFromDataset(const std::string& dname, int minX, int minY, int sizeX, int sizeY, int wdim, int hdim, void *data) = 0;
  virtual void readFromDataset(const std::string& dname, int minX, int minY, int sizeX, int sizeY, int wdim, int hdim, int *indexes, void *data) = 0;
  virtual void cleanupDataset() = 0;
};

#endif /* SIMHDF5DETECTORAPP_SRC_SIMHDF5READER_H_ */
