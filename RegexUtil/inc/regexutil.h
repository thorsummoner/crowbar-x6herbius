/** \file regexutil.h
 * \brief Defines useful regular expression strings.
 */

/**
 * \defgroup RegexUtil RegexUtil library
 * @{
 */

#ifndef REGEXUTIL_H
#define REGEXUTIL_H

#include "regexutil_global.h"
#include <QString>

namespace NRegexUtil
{
    /**
     * @brief Regex to match command arguments.
     *
     * Arguments are either space-delimited or enclosed in unescaped quotes.
     * <b>
     * @verbatim "[^"\\]*(?:\\.[^"\\]*)*"|\S+ @endverbatim
     * </b><br/>
     * For example, when parsing the following string:<br/>
     * <b>this is a \" test\" "command \"string\""</b><br/>
     * The matches are:<br/>
     * <ul><li>this</li>
     * <li>is</li>
     * <li>a</li>
     * <li>\"</li>
     * <li>test\"</li>
     * <li>"command \"string\""</li></ul>
     */
    static const QString RegexMatchCommandArgs = QString("\"[^\"\\\\]*(?:\\\\.[^\"\\\\]*)*\"|\\S+");
    
    /**
     * @brief Regex to match command arguments but also spaces between arguments.
     *
     * <b>
     * @verbatim "[^"\\]*(?:\\.[^"\\]*)*"|\S+|\s+ @endverbatim
     * </b><br/>
     * Same as the non-strict regex except spaces between the arguments are also matched.
     * This allows checking whether a user has finished typing the first argument in a given
     * string (ie. the command name) and is going to type any subsequent arguments for that command.
     * As soon as the number of matches is greater than 1, the user has stopped typing the command name.
     */
    static const QString RegexMatchCommandArgsWs = QString("\"[^\"\\\\]*(?:\\\\.[^\"\\\\]*)*\"|\\S+|\\s+");
    
    /**
     * @brief Regex to match pipe characters surrounded by whitespace.
     *
     * Will match unescaped pipe characters, regardless of how much whitespace they are (or are not) surrounded by.<br/>
     * <b>
     * @verbatim \s*(?<!\\)\|\s* @endverbatim
     * </b><br/>
     * Searches for 0 or more whitespace characters, followed by a pipe which is not preceded by a backslash (using negative
     * lookbehind), followed by 0 or more whitespace characters.
     * @note Will still match pipe characters that are quoted.
     */
    static const QString RegexMatchDelimPipe = QString("\\s*(?<!\\\\)\\|\\s*");
    
    /**
     * @brief Regex to match semicolons surrounded by whitespace.
     *
     * <b>
     * @verbatim \s*(?<!\\)\;\s* @endverbatim
     * </b><br/>
     * Similar construction to the pipe regex.
     * @note Will still match semicolon characters that are quoted.
     */
    static const QString RegexMatchDelimSemicolon = QString("\\s*(?<!\\\\)\\;\\s*");
    
    /**
     * @brief Regex to match the last argument in a command string.
     *
     * <b>
     * @verbatim \S*$ @endverbatim
     * </b><br/>
     * Matches any non-whitespace character any number of times at the end of the string.
     * If there are no non-whitespace characters (but there is whitespace), does not match.
     */
    static const QString RegexMatchLastArgNonWs = QString("\\S*$");
    
    /**
     * @brief Regex to match the last argument in a command string.
     *
     * <b>
     * @verbatim \w*$ @endverbatim
     * </b><br/>
     * In contrast to the above (which will match any non-whitespace character), this regex
     * will match only letters, numbers and underscores.
     */
    static const QString RegexMatchLastArgWord = QString("\\w*$");
    
    static const QString testString = "Test string";
}

// It appears that without a .cpp file there are compile errors.
// Until I learn why, a redundant class is defined here and implemented in the .cpp file to make compiled work.
namespace NULL_REQUIRED
{
    /**
     * @brief Temporary class that's here in order to generate a .lib file.
     */
    class Null_Required
    {
        /**
         * @brief Constructor.
         */
        Null_Required();
    };
}

#endif // REGEXUTIL_H

/**@}*/
