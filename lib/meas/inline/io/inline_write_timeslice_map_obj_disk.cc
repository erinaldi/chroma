/*! \file
 * \brief Inline task to time-sliced map object
 */

#include "chromabase.h"
#include "singleton.h"
#include "funcmap.h"

#include "qdp_map_obj_disk.h"
#include "qdp_disk_map_slice.h"

#include "meas/inline/abs_inline_measurement_factory.h"
#include "meas/inline/io/inline_write_timeslice_map_obj_disk.h"
#include "meas/inline/io/named_objmap.h"

#include "util/info/proginfo.h"
#include "util/ferm/subset_ev_pair.h"
#include "util/ferm/subset_vectors.h"

namespace Chroma 
{ 
  namespace InlineWriteTimeSliceMapObjDiskEnv 
  { 
    namespace WriteMapObjCallEnv
    { 
      struct DumbDisambiguator {};

      //! Write object function map
      /*! \ingroup inlineio */
      typedef SingletonHolder< 
	FunctionMap<DumbDisambiguator,
		    void,
		    std::string,
		    TYPELIST_1(const Params&),
		    void (*)(const Params& named_obj),
		    StringFunctionMapError> >
      TheWriteMapObjFuncMap;


      namespace 
      { 
	static bool registered = false;

	void writeMapObjEVPairLCV(const Params& params)
	{
	  // Input object
	  QDP::MapObject<int,EVPair<LatticeColorVector> >& input_obj = 
	    *(TheNamedObjMap::Instance().getData< Handle< QDP::MapObject<int,EVPair<LatticeColorVector> > > >(params.named_obj.input_id));
	  std::vector<int> keys = input_obj.keys();

	  const int decay_dir = Nd-1;

	  XMLBufferWriter file_xml;

	  push(file_xml, "MODMetaData");
	  write(file_xml, "id", string("eigenVecsTimeSlice"));
	  write(file_xml, "lattSize", QDP::Layout::lattSize());
	  write(file_xml, "decay_dir", decay_dir);
	  write(file_xml, "num_vecs", keys.size());
	  proginfo(file_xml);    // Print out basic program info
	  write(file_xml, "Weights", getEigenValues(input_obj, keys.size()));
	  pop(file_xml);

	  // Create the entry
	  QDP::MapObjectDisk<int,TimeSliceIO<LatticeColorVector> > output_obj(params.named_obj.output_file, file_xml.str());

	  // Copy the key/value-s
	  output_obj.openWrite();

	  int Lt = Layout::lattSize()[decay_dir];

	  for(int i=0; i < keys.size(); i++) 
	  {
	    EVPair<LatticeColorVector> tmpvec; input_obj.lookup(keys[i], tmpvec);

	    for(int t=0; t < Lt; t++) 
	    {
	      output_obj.insert(keys[i], TimeSliceIO<LatticeColorVector>(tmpvec.eigenVector,t));
	    }
	  }

	  output_obj.openRead();
	}


	bool registerAll(void) 
	{
	  bool success = true; 
	  if (! registered ) 
	  { 
	    success &= TheWriteMapObjFuncMap::Instance().registerFunction("KeyTintValTEVPairLatticeColorVector",
									  writeMapObjEVPairLCV);

	    registered = true;
	  }
	  return success;
	}
      }
    }


    namespace
    {
      AbsInlineMeasurement* createMeasurement(XMLReader& xml_in, 
					      const std::string& path) 
      {
	return new InlineMeas(Params(xml_in, path));
      }

      //! Local registration flag
      bool registered = false;

      const std::string name = "WRITE_TIMESLICE_MAP_OBJECT_DISK";
    }

    //! Register all the factories
    bool registerAll() 
    {
      bool success = true; 
      if (! registered)
      {
	success &= TheInlineMeasurementFactory::Instance().registerObject(name, createMeasurement);
	success &= WriteMapObjCallEnv::registerAll();
	registered = true;
      }
      return success;
    }


    //! Object buffer
    void read(XMLReader& xml, const string& path, Params::NamedObject_t& input)
    {
      XMLReader inputtop(xml, path);

      read(inputtop, "object_type", input.object_type);
      read(inputtop, "input_id", input.input_id);
      read(inputtop, "output_file", input.output_file);
    }

    // Param stuff
    Params::Params() { frequency = 0; }

    Params::Params(XMLReader& xml_in, const std::string& path) 
    {
      try 
      {
	XMLReader paramtop(xml_in, path);

	if (paramtop.count("Frequency") == 1)
	  read(paramtop, "Frequency", frequency);
	else
	  frequency = 1;

	// Parameters for source construction
	read(paramtop, "NamedObject", named_obj);
      }
      catch(const std::string& e) 
      {
	QDPIO::cerr << __func__ << ": caught Exception reading XML: " << e << endl;
	QDP_abort(1);
      }
    }


    void 
    InlineMeas::operator()(unsigned long update_no, XMLWriter& xml_out) 
    {
      START_CODE();

      registerAll();

      push(xml_out, "write_timeslice_map_object_disk");
      write(xml_out, "update_no", update_no);

      QDPIO::cout << name << ": map object copy" << endl;
      StopWatch swatch;

      // Write the object
      // ONLY named object format is supported in this task
      // Other tasks could support other disk formats
      QDPIO::cout << "Attempt to copy input object name = " << params.named_obj.input_id << endl;

      write(xml_out, "object_type", params.named_obj.object_type);
      write(xml_out, "input_id", params.named_obj.input_id);
      write(xml_out, "output_file", params.named_obj.output_file);

      try
      {
	swatch.reset();
	swatch.start();

	// Write the object
	WriteMapObjCallEnv::TheWriteMapObjFuncMap::Instance().callFunction(params.named_obj.object_type, params);

	swatch.stop();

	QDPIO::cout << "Object successfully copied: time= " 
		    << swatch.getTimeInSeconds() 
		    << " secs" << endl;
      }
      catch( std::bad_cast ) 
      {
	QDPIO::cerr << name 
		    << ": cast error for input_id= " << params.named_obj.input_id 
		    << " with type= " << params.named_obj.object_type 
		    << endl;
	QDP_abort(1);
      }
      catch (const string& e) 
      {
	QDPIO::cerr << name << ": error message: " << e << endl;
	QDP_abort(1);
      }
      catch(const char* e) 
      { 
	QDPIO::cout << name << ": Caught const char * exception: " << e << endl;
	QDP_abort(1);
      }
    
      QDPIO::cout << name << ": ran successfully" << endl;

      pop(xml_out);  // read_named_obj

      END_CODE();
    } 

  }
}