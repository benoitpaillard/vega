/*
 * Copyright (C) Alneos, s. a r. l. (contact@alneos.fr)
 * This file is part of Vega.
 *
 *   Vega is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   Vega is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with Vega.  If not, see <http://www.gnu.org/licenses/>.
 *
 * NastranTokenizer.h
 *
 *  Created on: Sep 1, 2013
 *      Author: devel
 */

#ifndef NASTRANTOKENIZER_H_
#define NASTRANTOKENIZER_H_

#include <string>
#include <fstream>
#include <vector>
#include <iostream>
#include <limits>
#include "../Abstract/ConfigurationParameters.h"
#include "../Abstract/SolverInterfaces.h"

namespace vega {

namespace nastran {

//TODO implements iterator
class NastranTokenizer : public vega::Tokenizer {
public:
    enum class LineType {
        FREE_FORMAT,
        SHORT_FORMAT,
        LONG_FORMAT
    };
    enum class CommentType : int {
        BEAMSECTCOLS,
        BEAMSECTS,
        COMP,
        COMPS,
        CSURF,
        DEQUATIONS,
        GROUP,
        LOADCOL,
        LOADCOLS,
        LOADSTEP,
        MAT,
        MATS,
        OBJECTIVES,
        OPTICONSTRAINTS,
        PROP,
        SYSTCOL,
        VECTORCOL
    };
private:
    static const int SFSIZE = 8; /**< Short field size **/
    static const int LFSIZE = 16;/**< Long field size **/
    static const std::string HM_COMMENT_START;
    static const std::map<std::string, CommentType> commentTypeByString;

    unsigned int currentField;   /**< Current position of the Tokenizer, i.e, the next field to be interpreted **/
    std::vector<std::string> currentLineVector;
    std::string currentLine = "";

    NastranTokenizer::LineType getLineType(const std::string& line); /**< Determine the LineType of the line.**/
    void replaceTabs(std::string &line, bool longFormat); /**< Replace all tabulation by the needed number of space. **/

    void splitFixedFormat(std::string& line, bool longFormat, bool firstLine);

    bool readLineSkipComment(std::string& line, bool firstLine);
    void splitFreeFormat(std::string line, bool firstLine);
    void parseBulkSectionLine(std::string line);
    void parseParameters();

    /**
     * Return the next symbol to be interpreted, as a string (trimmed + uppercase), and advances to next field
     * Return a void string if it's the end of the line.
     */
    //string nextBulkString();
    std::string nextSymbolString();

public:
    enum class SymbolType {
        SYMBOL_KEYWORD = 0,
        SYMBOL_FIELD = 1,
        SYMBOL_EOF = 2
    };
    enum class SectionType {
        SECTION_EXECUTIVE,
        SECTION_BULK
    };


    SectionType currentSection;
    SymbolType nextSymbolType;
    std::map<std::pair<CommentType, int>, std::string> labelByCommentTypeAndId;

    NastranTokenizer(std::istream& stream, vega::LogLevel logLevel = vega::LogLevel::INFO,
            const std::string fileName = "UNKNOWN",
            const vega::ConfigurationParameters::TranslationMode translationMode = vega::ConfigurationParameters::TranslationMode::BEST_EFFORT);
    virtual ~NastranTokenizer() = default;
    NastranTokenizer(const NastranTokenizer& that) = delete;

    /**
     * Set the Tokenizer into BULK mode, which allows three formats: free,
     * short and long.
     * If a line was already read, it re-reads it in BULK mode.
     */
    void bulkSection();
    /*
     * space separated section
     */
    void executiveControlSection();

    /**
     * Try to interpret the next symbol in the Nastran file as an integer
     *  and advances to next symbol.
     *
     *  @param returnDefaultIfNotFoundOrBlank
     *      if true returns default if the field is not found or blank
     *      if false throws exception
     *  @param defaultValue
     *      default value to use instead
     * @return
     */
    int nextInt(bool returnDefaultIfNotFoundOrBlank = false, int defaultValue = Globals::UNAVAILABLE_INT);
    std::list<int> nextInts();
    std::list<double> nextDoubles();
    bool isNextInt();
    bool isNextTHRU();
    bool isNextBY();
    bool isNextDouble();
    bool isNextEmpty(int n = 1);
    bool isEmptyUntilNextKeyword();
    std::string remainingTextUntilNextKeyword();
    /**
     * Returns the next symbol the Nastran file as a String
     *  and advances to next symbol.
     *
     *  @param returnDefaultIfNotFoundOrBlank
     *      if true returns default if the field is not found or blank
     *      if false throws exception
     *  @param defaultValue
     *      default value to use instead
     * @return
     */
    std::string nextString(bool returnDefaultIfNotFoundOrBlank = false, std::string defaultValue = "");
    /**
     * Try to interpret the next symbol in the Nastran file as a double
     *  and advances to next symbol.
     * @return
     */
    double nextDouble(bool returnDefaultIfNotFoundOrBlank = false, double defaultValue =
            Globals::UNAVAILABLE_DOUBLE);

    /**
     * Skip at most n fields. It stops if end of line is reached.
     * @param fieldNum
     */
    void skip(int fieldNum);

    //TODO: Delete this command: useless for BULK, dangerous for EXECUTIVE ?
    /**
     * Skip to the next not empty field.
     */
    void skipToNotEmpty();

    /**
     * Skip to the next keyword, dismissing the current content.
     * Does nothing if the next field is a keyword
     */
    void skipToNextKeyword();
    /**
     * Return a vector containing the full data line with unparsed arguments.
     */
    std::vector<std::string> currentDataLine() const;

    std::string currentRawDataLine() const override;
    /**
     * Advances to next data line, discarding the current content.
     */
    void nextLine();

};

} /* namespace nastran */

} /* namespace vega */

#endif /* NASTRANTOKENIZER_H_ */
