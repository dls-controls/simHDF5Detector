/*
 * SimHDF5FileReader.h
 *
 *  Created on: 14 Apr 2015
 *      Author: gnx91527
 */

#ifndef ADSIMAPP_SRC_SimHDF5FileReader_H_
#define ADSIMAPP_SRC_SimHDF5FileReader_H_

#include <hdf5.h>
#include <string>
#include <map>
#include <vector>
#include <tr1/memory>
#include "NDArray.h"

class SimHDF5FileReader
{
public:
  SimHDF5FileReader();
  virtual ~SimHDF5FileReader();
  void setFilename(const std::string &filename);
  std::string getFilename();
  bool validateFilename();
  int fileExists();
  void loadFile();
  void unloadFile();

  std::vector<std::string> getDatasetKeys();
  std::vector<int> getDatasetDimensions(const std::string& dname);
  NDDataType_t getDatasetType(const std::string& dname);
  void readFromDataset(const std::string& dname, int wdim, int hdim, void *data);
  void readFromDataset(const std::string& dname, int wdim, int hdim, int *indexes, void *data);
  void process(hid_t loc_id, const char *name, H5G_obj_t type);

private:
  hid_t file;
  std::string filename;
  std::string cname;
  bool fileLoaded;

  class HDF5Dataset
  {
  public:
    HDF5Dataset(const std::string& name, hid_t id)
    {
      this->name = name;
      this->id = id;
    };

    hid_t getID()
    {
      return this->id;
    }

    virtual ~HDF5Dataset(){};

  private:
    std::string name;
    hid_t id;
  };

  std::map<std::string, std::tr1::shared_ptr<HDF5Dataset> > datasets;

};

#endif /* ADSIMAPP_SRC_SimHDF5FileReader_H_ */
