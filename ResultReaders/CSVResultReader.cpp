/*
 * CSVAssertionParser.cpp
 *
 *  Created on: Jan 14, 2015
 *      Author: devel
 */

#define BOOST_SPIRIT_DEBUG
#define BOOST_SPIRIT_USE_PHOENIX_V3
//#define BOOST_NO_SFINAE

#ifdef _WIN32
//#define BOOST_RESULT_OF_USE_TR1_WITH_DECLTYPE_FALLBACK
#define BOOST_RESULT_OF_USE_TR1
//windows fix error C2516: 'boost::phoenix::evaluator::impl<const Expr &,boost::phoenix::vector2<boost::mpl::true_,boost::phoenix::is_nullary>,
//boost::proto::envns_::empty_env>::result_type' : is not a legal base class is_nullary.hpp
#define BOOST_PHOENIX_NO_SPECIALIZE_CUSTOM_TERMINAL
#endif

#include <string>
#include <vector>
#include <memory>
#include <cassert>
#include <fstream>
#include <iostream>
#include <unordered_map>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_function.hpp>
#include <boost/spirit/include/phoenix_statement.hpp>
#include <boost/spirit/include/phoenix_bind.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/support_line_pos_iterator.hpp>
#include "CSVResultReader.h"
#include "../Abstract/ConfigurationParameters.h"
#include "../Abstract/Model.h"

#include <boost/fusion/container/vector.hpp>
#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/define_struct.hpp>
#include <boost/optional.hpp>
#include <boost/variant.hpp>

namespace qi = boost::spirit::qi;
namespace spirit = boost::spirit;
namespace phx = boost::phoenix;

using namespace vega;

using stream_iterator_type = spirit::line_pos_iterator<spirit::istream_iterator>;

namespace vega {
namespace result {

using namespace std;

struct CsvGrammar: qi::grammar<stream_iterator_type, void(), qi::locals<vector<LineItems>>,
		qi::blank_type> {
	CsvGrammar(Model& _model, const ConfigurationParameters& _configuration) :
			CsvGrammar::base_type(start), model(_model), configuration(_configuration) {
		using namespace qi;

		static const char colsep = ',';

		start = omit[*comment >> header[_a = qi::_1] >> eol] >> line(_a) % eol;
		header = header_cell % colsep;
		header_cell = lit("RESULTAT")[_val = LineItems::RESULT_NAME] | lit("NUME_ORDRE")[_val =
				LineItems::NUM_ORD] | lit("INST")[_val = LineItems::TIME] | lit("NOEUD")[_val =
				LineItems::NODE] | lit("DX")[_val = LineItems::DX] | lit("DY")[_val = LineItems::DY]
				| lit("DZ")[_val = LineItems::DZ] | lit("DRX")[_val = LineItems::DRX]
				| lit("DRY")[_val = LineItems::DRY] | lit("DRZ")[_val = LineItems::DRZ]
				| column[_val = LineItems::UNUSED];
		line = (column % colsep)[phx::bind(&CsvGrammar::convertFunc, this, qi::_1, qi::_r1)];
		column = quoted | *~char_(",\n");
		quoted = '"' >> *("\"\"" | ~char_("\"\n")) >> '"';
		comment = ("#" >> *~char_("\n") > qi::eol) | qi::eol;
		if (model.configuration.logLevel >= LogLevel::TRACE) {
            BOOST_SPIRIT_DEBUG_NODES((header)(column)(quoted)(comment)(line));
		}
	}

	void convertFunc(const vector<string>& columns, const vector<LineItems>& positions) {
		if (columns.size() != positions.size()) {
			return;
		}
		int i = 0;
		string result_name;
		int result_number = -1;
		string node_name;
		int nodeId = Node::UNAVAILABLE_NODE;
		int num_step;
		double time = 0;
		for (LineItems position : positions) {
			switch (position) {
			case LineItems::RESULT_NAME:
				result_name = columns[i];
				if (result_name.find("RESU") == 0) {
					result_number = stoi(result_name.substr(4));
				} else {
					cerr << "Can't parse result name" << result_name << endl;
					return;
				}
				break;
			case LineItems::NODE:
				node_name = columns[i];
				nodeId = stoi(node_name.substr(1));
				break;
			case LineItems::NUM_ORD:
				num_step = stoi(columns[i]);
				break;
			case LineItems::TIME:
				time = atof(columns[i].c_str());
				break;
			default:
				//nothing to do
				break;
			}
			i++;
		}
		UNUSEDV(num_step);
		i = 0;
		assert(result_number != -1);
		const auto& objectiveSet = model.getOrCreateObjectiveSet(result_number, ObjectiveSet::Type::ASSERTION);
		shared_ptr<Analysis> analysis = model.analyses.find(result_number);
		if (analysis != nullptr) {
            analysis->add(objectiveSet);
		}
		for (LineItems position : positions) {
			auto it = dofPosition_by_lineItemEnum.find(position);
			if (it != dofPosition_by_lineItemEnum.end()) {
				double value = atof(columns[i].c_str());
				assert(nodeId != Node::UNAVAILABLE_NODE);
				const auto& nda = make_shared<NodalDisplacementAssertion>(model, objectiveSet, configuration.testTolerance, nodeId,
						DOF::findByPosition(it->second), value, time);
				model.add(nda);
			}
			i++;
		}
	}

	qi::rule<stream_iterator_type, void(), qi::locals<vector<LineItems>>, qi::blank_type> start;
	qi::rule<stream_iterator_type, vector<LineItems>(), qi::blank_type> header;
	qi::rule<stream_iterator_type, LineItems(), qi::blank_type> header_cell;
	qi::rule<stream_iterator_type, void(vector<LineItems>), qi::blank_type> line;
	qi::rule<stream_iterator_type, string(), qi::blank_type> column;
	qi::rule<stream_iterator_type, string()> quoted;
	qi::rule<stream_iterator_type, qi::blank_type> empty;
	qi::rule<stream_iterator_type> comment;
	Model& model;
	const ConfigurationParameters configuration;
	//visual studio 2013 refuses to compile initializer list
	unordered_map<LineItems, dof_int, std::hash<dof_int>> dofPosition_by_lineItemEnum = {
        {LineItems::DX, 0}, //
        {LineItems::DY, 1}, //
        {LineItems::DZ, 2}, //
        {LineItems::DRX, 3}, //
        {LineItems::DRY, 4}, //
        {LineItems::DRZ, 5}
    };
};

void CSVResultReader::add_assertions(const ConfigurationParameters& configuration,
		Model& model) {

	if (!configuration.resultFile.empty()) {
		ifstream in(configuration.resultFile.string());
		in.unsetf(ios::skipws);
		spirit::istream_iterator begin(in);
		const spirit::istream_iterator end;

		stream_iterator_type begin1(begin);
		stream_iterator_type end1(end);
		CsvGrammar grammar(model, configuration);

		bool ok = qi::phrase_parse(begin1, end1, grammar, qi::blank);

		if (ok) {
			cout << "Parse OK\n";
		} else {
			cout << "Parse failed\n";
		}

		if (begin1 != end1)
			cout << "Remaining unparsed: '" << string(begin1, end1) << endl;
	}
}

}
} /* namespace vega */
