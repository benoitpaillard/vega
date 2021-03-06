/*
 * CSVAssertionParser.h
 *
 *  Created on: Jan 14, 2015
 *      Author: devel
 */

#ifndef COMMANDLINE_CSVASSERTIONPARSER_H_
#define COMMANDLINE_CSVASSERTIONPARSER_H_

#include "../Abstract/SolverInterfaces.h"
#include <vector>
#include <memory>

namespace vega {

class ConfigurationParameters;
class Assertion;

namespace result {

enum LineItems {
	UNUSED,
	RESULT_NAME,
	NODE,
	NUM_ORD,
	TIME,
	DX,
	DY,
	DZ,
	DRX,
	DRY,
	DRZ
};

class CSVResultReader final: public vega::ResultReader {
public:
	CSVResultReader() = default;
	CSVResultReader(const CSVResultReader& that) = delete;
	void add_assertions(const ConfigurationParameters& configuration,
			Model& model) override;
};

}
} /* namespace vega */

#endif /* COMMANDLINE_CSVASSERTIONPARSER_H_ */
