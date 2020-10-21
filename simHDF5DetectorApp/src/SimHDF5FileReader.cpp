/*
 * SimHDF5FileReader.cpp
 *
 *  Created on: 14 Apr 2015
 *      Author: gnx91527
 */

#include "SimHDF5FileReader.h"
#include <iostream>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>

/** C function called when inspecting the HDF5 for datasets.
  * \param[in] loc_id Internal ID of the HDF5 object.
  * \param[in] name pointer to the name of the HF5 object.
  * \param[in] opdata void pointer to the object that started the inspection.
  *
  * C function callback invoked by inspecting the HDF5 file.  The pointer will point
  * to the SimHDF5FileReader object that started the inspection and can be used to
  * call the appropriate method.
  */
herr_t file_info(hid_t loc_id, const char *name, void *opdata)
{
  H5G_stat_t statbuf;
  // Cast void pointer into our SimHDF5FileReader class
  SimHDF5FileReader *ptr = (SimHDF5FileReader *)opdata;
  // Get the HDF5 object information for this object ID
  H5Gget_objinfo(loc_id, name, false, &statbuf);
  // Process this object
  ptr->process(loc_id, name, statbuf.type);
  return 0;
}

/** Constructor.
  *
  * Sets default values for all member variables.
  */
SimHDF5FileReader::SimHDF5FileReader() :
  SimHDF5Reader(),
  file(0),
  filename(""),
  fileLoaded(false),
  dset_id(0),
  dspace_id(0),
  ndims(0),
  dims(0),
  type_id(0),
  ntype_id(0),
  reading(false),
  inMemory(false),
  rawPtr(0),
  memDatatype(NDUInt8)
{

}

/** Destructor.
  *
  */
SimHDF5FileReader::~SimHDF5FileReader()
{

}

/** Set the filename of the HDF5 to read.
  * \param[in] filename including full path of the HDF5 to read.
  *
  */
void SimHDF5FileReader::setFilename(const std::string &filename)
{
  this->filename = filename;
}

/** Get the filename of the HDF5 to read.
  * \return filename including full path of the HDF5 to read.
  *
  */
std::string SimHDF5FileReader::getFilename()
{
  return filename;
}

/** Validate the filename.
  *
  * This method checks that the file exists.  If it does then an
  * attempt is made to open the file using the HDF library.  If
  * the file is successfully opened then the file is considered
  * valid.
  */
bool SimHDF5FileReader::validateFilename()
{
  bool validated = true;
  hid_t fid = 0;

  // First check for the existence of the file
  if(!fileExists()){
    validated = false;
  }

  if (validated){
    // Now attempt to open the file
    fid = H5Fopen(filename.c_str(), H5F_ACC_RDONLY, H5P_DEFAULT);
    if (fid < 0){
      validated = false;
    } else {
      // Close the file again
      H5Fclose(fid);
    }
  }
  return validated;
}

/** Validate the filename.
  * \return integer value of whether the file exists.
  *
  * This method checks that the file stored in filename exists.
  */
int SimHDF5FileReader::fileExists()
{
  struct stat buffer;
  return (stat (filename.c_str(), &buffer) == 0);
}

/** Load the filename and inspect it.
  *
  * The file specified by filename is opened and then inspected
  * for datasets.
  */
void SimHDF5FileReader::loadFile()
{
  if (fileLoaded){
    // If there is already an open file then we need to unload it first
    unloadFile();
  }
  // Open the file
  file = H5Fopen(filename.c_str(), H5F_ACC_RDONLY, H5P_DEFAULT);
  fileLoaded = true;
  // Iterate through the file structure to obtain all datasets
  H5Giterate(file, "/", NULL, file_info, this);
}

/** Unload currently loaded file and clear resources.
  *
  */
void SimHDF5FileReader::unloadFile()
{
  if (fileLoaded){
    // First empty the dataset containers
    datasets.clear();
    // Now force a close of the file, clearing out all references etc
    H5Fclose(this->file);
    this->file = -1;
  }
}

/** Return an array of dataset keys found within the file.
  * \return vector of string dataset key values.
  *
  */
std::vector<std::string> SimHDF5FileReader::getDatasetKeys()
{
  std::vector<std::string> keys;
  std::map<std::string, std::tr1::shared_ptr<HDF5Dataset> >::iterator iter;
  for (iter = datasets.begin(); iter != datasets.end(); ++iter){
    keys.push_back(iter->first);
  }
  return keys;
}

/** Return the specified dataset dimensions.
  * \param[in] dname Name of the dataset
  * \return vector of integer dimensions.
  *
  * Returns the dimensions of the specified dataset as a vector of
  * integer values.
  */
std::vector<int> SimHDF5FileReader::getDatasetDimensions(const std::string& dname)
{
  if (reading && inMemory){
    return memDims;
  }
  std::vector<int> dimensions;
  hid_t dset = H5Dopen(this->file, dname.c_str(), H5P_DEFAULT);
  hid_t dspace = H5Dget_space(dset);
  const int ndims = H5Sget_simple_extent_ndims(dspace);
  hsize_t dims[ndims];
  H5Sget_simple_extent_dims(dspace, dims, NULL);
  for (int i = 0; i < ndims; i++){
    dimensions.push_back((int)dims[i]);
  }
  H5Sclose(dspace);
  H5Dclose(dset);
  return dimensions;
}

/** Return the specified dataset type.
  * \param[in] dname Name of the dataset
  * \return data type of the dataset.
  *
  * Returns the data type as a NDDataType_t type.
  */
NDDataType_t SimHDF5FileReader::getDatasetType(const std::string& dname)
{
  NDDataType_t NDType = NDUInt8;

  if (inMemory){
    return memDatatype;
  }

  hid_t dset = H5Dopen(this->file, dname.c_str(), H5P_DEFAULT);
  hid_t type = H5Dget_type(dset);
  hid_t ntype = H5Tget_native_type(type, H5T_DIR_ASCEND);
  if (H5Tequal(ntype, H5T_NATIVE_INT8)){
    NDType = NDInt8;
    //std::cout << "Data type: NDInt8" << std::endl;
  }
  if (H5Tequal(ntype, H5T_NATIVE_UINT8)){
    NDType = NDUInt8;
    //std::cout << "Data type: NDUInt8" << std::endl;
  }
  if (H5Tequal(ntype, H5T_NATIVE_INT16)){
    NDType = NDInt16;
    //std::cout << "Data type: NDInt16" << std::endl;
  }
  if (H5Tequal(ntype, H5T_NATIVE_UINT16)){
    NDType = NDUInt16;
    //std::cout << "Data type: NDUInt16" << std::endl;
  }
  if (H5Tequal(ntype, H5T_NATIVE_INT32)){
    NDType = NDInt32;
    //std::cout << "Data type: NDInt32" << std::endl;
  }
  if (H5Tequal(ntype, H5T_NATIVE_UINT32)){
    NDType = NDUInt32;
    //std::cout << "Data type: NDUInt32" << std::endl;
  }
  if (H5Tequal(ntype, H5T_NATIVE_FLOAT)){
    NDType = NDFloat32;
    //std::cout << "Data type: NDFloat32" << std::endl;
  }
  if (H5Tequal(ntype, H5T_NATIVE_DOUBLE)){
    NDType = NDFloat64;
    //std::cout << "Data type: NDFloat64" << std::endl;
  }
  H5Tclose(ntype);
  H5Tclose(type);
  H5Dclose(dset);
  return NDType;
}

/** Prepare information required to read out dataset data.
  * \param[in] dname Name of the dataset
  *
  * Allocates the required resources ready to read out the data for
  * the specified dataset.
  */
void SimHDF5FileReader::prepareToReadDataset(const std::string& dname)
{
  if (!reading){
    dset_id = H5Dopen(this->file, dname.c_str(), H5P_DEFAULT);
    dspace_id = H5Dget_space(dset_id);
    ndims = H5Sget_simple_extent_ndims(dspace_id);
    dims = (hsize_t *)malloc(sizeof(hsize_t) * ndims);
    H5Sget_simple_extent_dims(dspace_id, dims, NULL);
    type_id = H5Dget_type(dset_id);
    ntype_id = H5Tget_native_type(type_id, H5T_DIR_ASCEND);
    if (inMemory){
      // We need to allocate the total memory required for the dataset
      int totalBytes = 1;
      hsize_t offset[ndims]; // Hyperslab offset in the file
      for (int index = 0; index < ndims; index++){
        totalBytes = totalBytes * dims[index];
        offset[index] = 0;
      }
      switch (getDatasetType(dname)){
        case NDInt8:
        case NDUInt8:
          totalBytes = totalBytes * 1;
          break;
        case NDInt16:
        case NDUInt16:
          totalBytes = totalBytes * 2;
          break;
        case NDInt32:
        case NDUInt32:
        case NDFloat32:
          totalBytes = totalBytes * 4;
          break;
        case NDFloat64:
          totalBytes = totalBytes * 8;
          break;
      }
      // Perform the allocation
      printf("Allocating %d bytes for dataset storage\n", totalBytes);
      rawPtr = malloc(totalBytes);
      // Select the hyperslab
      herr_t status = H5Sselect_hyperslab(dspace_id, H5S_SELECT_SET, offset, NULL, dims, NULL);
      // Define the memory dataspace.
      hid_t memspace = H5Screate_simple(ndims,dims,NULL);
      // Select the memory hyperslab
      status = H5Sselect_hyperslab(memspace, H5S_SELECT_SET, offset, NULL, dims, NULL);
      // Read data from hyperslab in the file into the hyperslab in memory and to the data pointer
      status = H5Dread(dset_id, ntype_id, memspace, dspace_id, H5P_DEFAULT, rawPtr);
      // Cleanup
      H5Sclose(memspace);

      std::vector<int> dimensions;
      hid_t dset = H5Dopen(this->file, dname.c_str(), H5P_DEFAULT);
      hid_t dspace = H5Dget_space(dset);
      const int ndims = H5Sget_simple_extent_ndims(dspace);
      hsize_t dims[ndims];
      H5Sget_simple_extent_dims(dspace, dims, NULL);
      for (int i = 0; i < ndims; i++){
        dimensions.push_back((int)dims[i]);
      }
      H5Sclose(dspace);
      H5Dclose(dset);
      memDims = dimensions;



      dset = H5Dopen(this->file, dname.c_str(), H5P_DEFAULT);
      hid_t type = H5Dget_type(dset);
      hid_t ntype = H5Tget_native_type(type, H5T_DIR_ASCEND);
      if (H5Tequal(ntype, H5T_NATIVE_INT8)){
        memDatatype = NDInt8;
        //std::cout << "Data type: NDInt8" << std::endl;
      }
      if (H5Tequal(ntype, H5T_NATIVE_UINT8)){
        memDatatype = NDUInt8;
        //std::cout << "Data type: NDUInt8" << std::endl;
      }
      if (H5Tequal(ntype, H5T_NATIVE_INT16)){
        memDatatype = NDInt16;
        //std::cout << "Data type: NDInt16" << std::endl;
      }
      if (H5Tequal(ntype, H5T_NATIVE_UINT16)){
        memDatatype = NDUInt16;
        //std::cout << "Data type: NDUInt16" << std::endl;
      }
      if (H5Tequal(ntype, H5T_NATIVE_INT32)){
        memDatatype = NDInt32;
        //std::cout << "Data type: NDInt32" << std::endl;
      }
      if (H5Tequal(ntype, H5T_NATIVE_UINT32)){
        memDatatype = NDUInt32;
        //std::cout << "Data type: NDUInt32" << std::endl;
      }
      if (H5Tequal(ntype, H5T_NATIVE_FLOAT)){
        memDatatype = NDFloat32;
        //std::cout << "Data type: NDFloat32" << std::endl;
      }
      if (H5Tequal(ntype, H5T_NATIVE_DOUBLE)){
        memDatatype = NDFloat64;
        //std::cout << "Data type: NDFloat64" << std::endl;
      }
      H5Tclose(ntype);
      H5Tclose(type);
      H5Dclose(dset);




      H5Fclose(this->file);
      this->file = -1;
    }
    reading = true;
  }
}

void SimHDF5FileReader::readFromDataset(const std::string& dname, int minX, int minY, int sizeX, int sizeY, int wdim, int hdim, void *data)
{
  int indexes[6] = {0,0,0,0,0,0};
  readFromDataset(dname, minX, minY, sizeX, sizeY, wdim, hdim, indexes, data);
}

/** Prepare information required to read out dataset data.
  * \param[in] dname Name of the dataset
  * \param[in] minX offset of data in x dimension
  * \param[in] minY offset of data in y dimension
  * \param[in] sizeX ROI of data in x dimension
  * \param[in] sizeY ROI of data in y dimension
  * \param[in] wdim specified dimension number for x dimension
  * \param[in] hdim specified dimension number for y dimension
  * \param[in] indexes index values for additional dimensions
  * \param[out] data pointer to buffer for storing data
  *
  * Fills the data buffer with the data required according to the supplied
  * indexes, offsets and ROI parameters.
  */
void SimHDF5FileReader::readFromDataset(const std::string& dname, int minX, int minY, int sizeX, int sizeY, int wdim, int hdim, int *indexes, void *data)
{
  hid_t memspace;    // Memory space ID
  hsize_t dimsm[2];  // Memory space dimensions
  herr_t status;

  hsize_t count[ndims];  // Size of the hyperslab in the file
  hsize_t offset[ndims]; // Hyperslab offset in the file
  hsize_t count_out[2];  // Size of the hyperslab in memory
  hsize_t offset_out[2]; // Hyperslab offset in memory

  if (inMemory){
    int totalBytes = 1;
    switch (getDatasetType(dname)){
      case NDInt8:
      case NDUInt8:
        totalBytes = totalBytes * 1;
        break;
      case NDInt16:
      case NDUInt16:
        totalBytes = totalBytes * 2;
        break;
      case NDInt32:
      case NDUInt32:
      case NDFloat32:
        totalBytes = totalBytes * 4;
        break;
      case NDFloat64:
        totalBytes = totalBytes * 8;
        break;
    }
      void *ptr = (void *)&(((char *)rawPtr)[0]);
      memcpy(data, ptr, sizeX*sizeY*totalBytes);
      return;
  }

  // Define hyperslab in the dataset.
  // First initialise offsets to zero in all dimensions
  // Also initialise size to 1 in all dimensions
  int ofsindex = 0;
  for (int index = 0; index < ndims; index++){
    if (index == wdim || index == hdim){
      // This is a frame dimension, so set the offset to minX, minY
      if (index == wdim){
        offset[index] = minX;
      } else if (index == hdim){
        offset[index] = minY;
      }
    } else {
      // Set the offset to the specified index
      offset[index] = indexes[ofsindex];
      ofsindex++;
    }
    count[index]  = 1;
  }
  // Now set the count dimensions for the chosen width, height to the real
  // dimension sizes for the chosen width, height
  count[wdim] = sizeX;
  count[hdim] = sizeY;
  // Select the hyperslab
  status = H5Sselect_hyperslab(dspace_id, H5S_SELECT_SET, offset, NULL, count, NULL);

  // Define the memory dataspace.
  dimsm[1] = sizeX;
  dimsm[0] = sizeY;
  int ndimsout = 2;
  memspace = H5Screate_simple(ndimsout,dimsm,NULL);

  // Define memory hyperslab.  This is only 2 dimensional no matter how many dims there are
  offset_out[0] = 0;
  offset_out[1] = 0;
  count_out[1]  = sizeX;
  count_out[0]  = sizeY;
  // Select the memory hyperslab
  status = H5Sselect_hyperslab(memspace, H5S_SELECT_SET, offset_out, NULL, count_out, NULL);

  // Read data from hyperslab in the file into the hyperslab in memory and to the data pointer
  status = H5Dread(dset_id, ntype_id, memspace, dspace_id, H5P_DEFAULT, data);

  H5Sclose(memspace);
}

/** Cleanup all resources after completion of reading out current dataset.
  *
  */
void SimHDF5FileReader::cleanupDataset()
{
  if (reading){
    H5Tclose(ntype_id);
    H5Tclose(type_id);
    H5Sclose(dspace_id);
    H5Dclose(dset_id);
    free(dims);
    reading = false;
  }
}

/** Process an HDF5 object and store the datasets.
  * \param[in] loc_id Internal ID of the HDF5 object.
  * \param[in] name pointer to the name of the HF5 object.
  * \param[in] type HDF5 object type.
  *
  * This method is called by the C function callback <b>file_info</b> invoked by
  * inspecting the HDF5 file.  Any datasets are stored in a map of dataset objects
  * to allow the driver to quickly retrieve information relating to each dataset.
  */
void SimHDF5FileReader::process(hid_t loc_id, const char *name, H5G_obj_t type)
{
  std::string sname(name);
  std::string oldname = cname;
  cname = cname + "/" + sname;
  if (type == H5G_DATASET){
    if (getDatasetDimensions(cname).size() > 2){
      datasets[cname] = std::tr1::shared_ptr<HDF5Dataset>(new HDF5Dataset(name, loc_id));
    }
  }
  if (type == H5G_GROUP){
    H5Giterate(loc_id, name, NULL, file_info, this);
  }
  cname = oldname;
}
