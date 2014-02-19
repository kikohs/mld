/*
 * SPARKSEE: a library for out-of-core graph-based data storage and query processing.
 *
 * See the file LICENSE.txt for redistribution information.
 *
 * Copyright (c) 2005-2010 Sparsity Technologies, Barcelona (Spain)
 */

#ifndef _SPARKSEE_CSVWRITER_H_
#define _SPARKSEE_CSVWRITER_H_

/**
 * \addtogroup io
 * @{
 *
 * \file CSVWriter.h
 * \brief It contains the definition of CSVWriter class.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */

#include "io/RowWriter.h"

/*
 * Forward declaration of sparksee_core classes.
 */
namespace sparksee_core
{
    class FileWriter;
    class Locale;
}


BEGIN_SPARKSEE_IO_NAMESPACE


/**
 * \brief CSVWriter interface.
 *
 * A very simple CSV writer implementing RowWriter.
 *
 * It works as any other RowWriter, but open must be called once before
 * the first write operation.
 * 
 * It uses the format RFC 4180: http://tools.ietf.org/html/rfc4180
 *
 * You can use your own separators and quote characters.
 * By default the separator is the comma (,) and the quote character
 * is the double quotes (") and autoquote is enabled.
 *
 * See the CSVReader locale documentation or the SPARKSEE User Manual.
 *
 * Check out the 'Data export' section in the SPARKSEE User Manual for more
 * details on this.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */
class SPARKSEE_EXPORT CSVWriter
: public RowWriter
{
private:
    
    /**
     * Output file path.
     */
    std::wstring file;
    /**
     * \brief The locale of the file.
     */
    sparksee_core::Locale* locale;
    /**
     * Character used to separate the values.
     */
    sparksee::gdb::uchar_t separator;
    /**
     * Character used to quote strings.
     */
    sparksee::gdb::uchar_t quote;    
    /**
     * Autoquote fields or not.
     */
    sparksee::gdb::bool_t autoquote;
    /**
     * Force quote given columns.
     */
    std::vector<sparksee::gdb::bool_t> * force;
    /**
     * Output stream.
     */
    sparksee_core::FileWriter *fileWriter;

    /**
     * Default field separator character.
     */
    static const sparksee::gdb::uchar_t defaultSeparator;
    /**
     * Default field quote character.
     */
    static const sparksee::gdb::uchar_t defaultQuote;

public:
    
    /**
     * Creates a new instance.
     */
    CSVWriter();
    
    /**
     * Sets the character used to separate fields in the file.
     * \param sep [in] Separator character.
     */
    void SetSeparator(const std::wstring & sep)
    throw (sparksee::gdb::Error);

    /**
     * Sets the character used to quote fields.
     * \param quotes [in] Quote character.
     */
    void SetQuotes(const std::wstring & quotes)
    throw (sparksee::gdb::Error);

    /**
     * Sets on/off the automatic quote mode.
     * If there are forced quotes, setting autoquotes on will clear them.
     * If the autoquotes is set to off and no forced quotes are provided,
     * there will not be any quote.
     * \param autoquotes [in] If TRUE it enables the automatic quote mode,
     *                        if FALSE it disables it.
     */
    void SetAutoQuotes(sparksee::gdb::bool_t autoquotes);

    /**
     * Disables the automatic quote mode and forces to be quoted those
     * positions set to TRUE in the given vector.
     * \param forcequotes [in] A booleanList with the position for each
     *     column that must be quoted set to true.
     */
    void SetForcedQuotes(sparksee::gdb::BooleanList & forcequotes);

    /**
     * \brief Sets the locale that will be used to write the file.
     * \param localeStr [in] The locale string for the file encoding.
     */
    void SetLocale(const std::wstring &localeStr);

    /**
     * \brief Opens the output file path.
     * \param f [in] Output file path.
     * \exception IOException If bad things happen opening the file.
     */
    void Open(const std::wstring & f)
    throw(sparksee::gdb::IOException);
    
    /**
     * Writes the next row.
     * \param row [in] Row of data.
     * \exception IOException If bad things happen during the write.
     */
    void Write(sparksee::gdb::StringList & row)
    throw(sparksee::gdb::IOException, sparksee::gdb::Error);
    
    /**
     * Closes the writer.
     * \exception IOException If the close fails.
     */
    void Close()
    throw(sparksee::gdb::IOException, sparksee::gdb::Error);
    
    /**
     * \brief Destructor.
     */
    virtual ~CSVWriter();
    
private:
    
    /**
     * Writes the string at the given position and quotes it if necessary.
     * \param str [in] String to be written.
     * \param pos [in] Position of the string in range [0..N). It is 
     * necessary to know if the string must be quoted or not.
     */
    void WriteString(const std::wstring & str, sparksee::gdb::int32_t pos);

    /**
     * \brief Non implemented to avoid use of copy constructor.
     */
    CSVWriter(const CSVWriter & csvw);
    
    /**
     * \brief Non implemented to avoid use of assignment operator.
     */
    CSVWriter & operator =(const CSVWriter & csvw);
    
};


END_SPARKSEE_IO_NAMESPACE

/** @} */
#endif // _SPARKSEE_CSVWRITER_H_
