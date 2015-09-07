/*!

  \file nmc_reader.cc

  \brief NMC_* classes

*/

#include "nmc_reader.hh"

//---------------------------------------------------------------------

void NMC_Wind_Reader::load( std::string filename )
{
	Py_Initialize();

//////////////////////////// TESTING STRINGS ////////////////////////////
    std::cout<<"show import pathes:\n";
    PyRun_SimpleString("import sys; print(sys.path)");
    std::cout<<"\nshow current working directory:\n";
    PyRun_SimpleString("import os; print(os.getcwd())");
    std::cout<<"\n";
    //////////////////////

	PyObject *pName;
	std::string module_name = remove_file_extension( filename );
	pName = PyUnicode_FromString( module_name.c_str() ); // new
	assert( pName );

	// Load the module object
	pModule = PyImport_Import( pName ); // new
	Py_DECREF( pName );                 // no need for pName anymore
	if ( !pModule )
		std::cout<< (std::string("module not found: ") + module_name ).c_str() <<"\n";
		//throw( ( std::string("module not found: ") + module_name ).c_str() );

	// Never fails if module OK
	PyObject* pDict = PyModule_GetDict( pModule ); // borrowed

/////////////////////////// path adjusting for test ///////////////
    //PyRun_SimpleString("import os; os.chdir('/home/gleb/Documents/wrkdir/')");
    PyRun_SimpleString("import os; os.chdir('./../python/siku/')");
    //PyRun_SimpleString("import os; os.chdir('./../../')");



	  // access to the siku field. Ura!
	PyObject* NMCWind = PyDict_GetItemString( pDict, "NMCWind" ); // borrowed
	PyObject* NMCVar = PyDict_GetItemString( pDict, "NMCVar" ); // borrowed

    //Py_INCREF( pVar );           // incremented to store in class

    PyObject* pTuple = PyTuple_New( 2 ); //new
    PyObject* a1 = PyUnicode_FromString( "u2014.nc" ); //new
    PyObject* a2 = PyUnicode_FromString( "uwnd" ); //new
    PyTuple_SetItem( pTuple, 0,  a1);
    PyTuple_SetItem( pTuple, 1,  a2);

    PyObject* uw = PyObject_CallObject( NMCVar, pTuple ); //new

    a1 = PyUnicode_FromString( "v2014.nc" ); //new
    a2 = PyUnicode_FromString( "vwnd" ); //new
    PyTuple_SetItem( pTuple, 0,  a1);
    PyTuple_SetItem( pTuple, 1,  a2);

    PyObject* vw = PyObject_CallObject( NMCVar, pTuple ); //new

    Py_DECREF( pTuple );
    pTuple = PyTuple_New( 3 ); //new

    PyTuple_SetItem( pTuple, 0,  uw );
    PyTuple_SetItem( pTuple, 1,  vw );
    PyTuple_SetItem( pTuple, 2,  Py_None );

    PyObject* pWind = PyObject_CallObject( NMCWind, pTuple ); //new

    Py_DECREF( pTuple );

//////////////////////////////

	PyObject* Lat = PyObject_GetAttrString( pWind, "lat" ); //new
	size_t lat_s = PyList_Size( Lat );
	//size_t lat_s = PyByteArray_GET_SIZE( Lat );

	PyObject* Lon = PyObject_GetAttrString( pWind, "lon" ); //new
	size_t lon_s = PyList_Size( Lon );
	//size_t lon_s = PyByteArray_GET_SIZE( Lon );

	init_grid( lat_s, lon_s );

	for( size_t i=0; i < lat_s; ++i )
	{
		pTemp = PyList_GetItem( Lat, i); //borrowed
		double cur_lat = PyFloat_AsDouble( pTemp );
		lat_indexer[ cur_lat ] = i;
	}

	for( size_t i=0; i < lon_s; ++i )
	{
		pTemp = PyList_GetItem( Lon, i); //borrowed
		double cur_lon = PyFloat_AsDouble( pTemp );
		lat_indexer[ cur_lon ] = i;
	}

    pTemp = PyObject_GetAttrString( pWind, "wind" ); //new

    for( size_t i=0; i < lat_s; ++i )
    {
        PyObject* pLine  = PyList_GetItem( pTemp, i ); //borrowed
        for( size_t j=0; j < lon_s; ++j )
        {
            pTuple = PyList_GetItem( pLine, j ); //borrowed
            double ew = PyFloat_AsDouble( PyTuple_GetItem( pTuple, 0 ) );
            double nw = PyFloat_AsDouble( PyTuple_GetItem( pTuple, 1 ) );

            double cur_lat = PyFloat_AsDouble( PyList_GetItem( Lat, i) );
            double cur_lon = PyFloat_AsDouble( PyList_GetItem( Lon, j) );

            GridNode<UVWind> tempW( cur_lat, cur_lon, UVWind( ew, nw ) );
            set_node( tempW, i,  j );
        }
    }

	Py_DECREF( Lat );
	Py_DECREF( Lon );
    Py_DECREF( pTemp );

    std::cout<<"get wind at 10, 20 internally:\n";
    std::cout<<grid[10][20].value.uwind<<"\n";

    std::cout<<"get wind at 10, 20 externally:\n";
    std::cout<<get_value( 10, 20 )->uwind<<"\n";

    std::cout<<"get latitude at 10 20 internally:\n";
    std::cout<<grid[10][20].lat<<"\n";
    std::cout<<"get longitude at 10 20 externally:\n";
    std::cout<<get_node( 10, 20 )->lon<<"\n";

    std::cout<<"get wind by lat-lon 65, 50:\n";
    std::cout<<get_value_by_lat_lon( 65, 50 )->uwind<<"\n";

	//pTemp = PyObject_GetAttrString(pVar, "x");

	//double result = PyFloat_AsDouble(pTemp);
	//std::cout<<result;

	Py_Finalize();
/*
	  // access to the siku.callback

	  // new ref
	  pSiku_callback = PyObject_GetAttrString( pSiku, "callback" );
	  assert ( pSiku_callback );

	  if ( !PyObject_IsInstance( pSiku_callback, pSiku ) )
	    fatal( 1, "Cannot read siku.callback" );

	  // access to the diagnostics (always defined)
	  pSiku_diagnostics = PyObject_GetAttrString( pSiku, "diagnostics" );
	  assert( pSiku_diagnostics );

	  if ( !PyObject_IsInstance( pSiku_diagnostics, pSiku ) )
	    fatal( 1, "Cannot read siku.diagnostics" );
*/
}


//REMOVE AFTER AUXUTILS INCLUDE!!!!
std::string remove_file_extension( const std::string& filename )
{
  size_t lastdot = filename.find_last_of( "." );
  if ( lastdot == std::string::npos )
    return filename;

  return filename.substr( 0, lastdot );
}
//AAAAA! REMOVE IT -^
