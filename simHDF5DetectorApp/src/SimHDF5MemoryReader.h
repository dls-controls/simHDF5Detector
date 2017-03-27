/*
 * SimHDF5MemoryReader.h
 *
 *  Created on: 28 Jan 2016
 *      Author: gnx91527
 */

#ifndef SIMHDF5DETECTORAPP_SRC_SIMHDF5MEMORYREADER_H_
#define SIMHDF5DETECTORAPP_SRC_SIMHDF5MEMORYREADER_H_

#include <hdf5.h>
#include <stdlib.h>
#include <string>
#include <map>
#include <vector>
#include <tr1/memory>
#include "NDArray.h"
#include "SimHDF5Reader.h"

class SimHDF5MemoryReader : public SimHDF5Reader
{
public:
  SimHDF5MemoryReader();
  virtual ~SimHDF5MemoryReader();
  void setFilename(const std::string &filename);
  std::string getFilename();
  bool validateFilename();
  int fileExists();
  void loadFile();
  void unloadFile();

  std::vector<std::string> getDatasetKeys();
  std::vector<int> getDatasetDimensions(const std::string& dname);
  std::vector<int> parseDatasetDimensions(const std::string& dname);
  NDDataType_t getDatasetType(const std::string& dname);
  NDDataType_t parseDatasetType(const std::string& dname);
  int dataTypeToBytes(NDDataType_t NDType);
  void prepareToReadDataset(const std::string& dname);
  void readFromDataset(const std::string& dname, int minX, int minY, int sizeX, int sizeY, int wdim, int hdim, void *data);
  void readFromDataset(const std::string& dname, int minX, int minY, int sizeX, int sizeY, int wdim, int hdim, int *indexes, void *data);
  void parseDataset(const std::string& dname, int minX, int minY, int sizeX, int sizeY, int wdim, int hdim, int *indexes, void *data);
  void cleanupDataset();
  void process(hid_t loc_id, const char *name, H5G_obj_t type);

private:
  hid_t file;
  std::string filename;
  std::string cname;
  bool fileLoaded;
  hid_t dset_id;
  hid_t dspace_id;
  int ndims;
  hsize_t *dims;
  hid_t type_id;
  hid_t ntype_id;
  bool reading;
  bool inMemory;
  void *rawPtr;
  std::vector<int> memDims;
  NDDataType_t memDatatype;

  class HDF5RawImage
  {
  public:
    HDF5RawImage(int width, int height, int dataSize)
    {
      this->width = width;
      this->height = height;
      this->dataSize = dataSize;
      this->rawPtr = malloc(width*height*dataSize);
    }

    int getAllocatedBytes()
    {
      return this->width*this->height*this->dataSize;
    }

    void *getRawPtr()
    {
      return this->rawPtr;
    }

  private:
    int width;
    int height;
    int dataSize;
    void *rawPtr;
  };

  class HDF5MemDataset
  {
  public:
    HDF5MemDataset(const std::string& name, std::vector<int> dimensions, NDDataType_t datatype, std::vector<std::tr1::shared_ptr<HDF5RawImage> > rawImages)
    {
      this->name = name;
      this->dimensions = dimensions;
      this->datatype = datatype;
      this->rawImages = rawImages;
    };

    std::vector<int> getDimensions()
    {
      return this->dimensions;
    }

    NDDataType_t getDataType()
    {
      return this->datatype;
    }

    std::tr1::shared_ptr<HDF5RawImage> getImage(int index)
    {
      return rawImages[index%rawImages.size()];
    }

    virtual ~HDF5MemDataset(){};

  private:
    std::string name;
    std::vector<int> dimensions;
    NDDataType_t datatype;
    std::vector<std::tr1::shared_ptr<HDF5RawImage> > rawImages;
  };

  std::map<std::string, std::tr1::shared_ptr<HDF5MemDataset> > datasets;

};

#endif /* SIMHDF5DETECTORAPP_SRC_SIMHDF5MEMORYREADER_H_ */
