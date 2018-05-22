#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#include "../src/libnfporb.hpp"

#include <boost/filesystem.hpp>
#include <string>
#include <vector>


using namespace std;
using namespace boost::filesystem;
using namespace libnfporb;

void find_files(const path & dir_path,         // in this directory,
                const std::string & file_name, // search for this name,
                vector<path> &paths_found)          // placing path here if found
{
  if (!exists(dir_path)) 
    return;

  directory_iterator end_itr; // default construction yields past-the-end

  for (directory_iterator itr(dir_path); itr != end_itr; ++itr) {
      if (is_directory(itr->status())) {
          find_files(itr->path(), file_name, paths_found);
      } else if (itr->path().filename() == file_name) { 
          paths_found.push_back(dir_path);
        }
    }
}

void read_wkt_nfp(const string& filename, nfp_t& p) {
	std::ifstream t(filename);
        /*
	std::string str;
	t.seekg(0, std::ios::end);
	str.reserve(t.tellg());
	t.seekg(0, std::ios::beg);

	str.assign((std::istreambuf_iterator<char>(t)),
                    std::istreambuf_iterator<char>());

	str.pop_back();
        */
        
        std::string line;
        while (std::getline(t, line)) {
            polygon_t::ring_type ring;
            bg::read_wkt(line, ring);

            p.push_back(ring);
        }
        
}

TEST_CASE( "Compare with golden results", "[golden]" ) {

  vector<path> test_dirs;
  find_files("../data", "golden.wkt", test_dirs);
    
  for(path testdir : test_dirs) {
    string name = testdir.filename().generic_string();

    SECTION( name.c_str() ) {
      	polygon_t pA;
	polygon_t pB;
	nfp_t nfpGolden;
	//read polygons from wkt files
	read_wkt_polygon((testdir / "A.wkt").native(), pA);
	read_wkt_polygon((testdir / "B.wkt").native(), pB);
	read_wkt_nfp((testdir / "golden.wkt").native(), nfpGolden);

        //generate NFP of polygon A and polygon B and check the polygons for validity.
	nfp_t nfp = generateNFP(pA, pB, true);
        

        REQUIRE( nfp == nfpGolden );
    }
  }

  
}
