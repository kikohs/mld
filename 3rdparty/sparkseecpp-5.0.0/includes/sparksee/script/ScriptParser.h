/*
 * SPARKSEE: a library for out-of-core graph-based data storage and query processing.
 *
 * See the file LICENSE.txt for redistribution information.
 *
 * Copyright (c) 2005-2010 Sparsity Technologies, Barcelona (Spain)
 */


#ifndef _SPARKSEE_SCRIPTPARSER_H_
#define _SPARKSEE_SCRIPTPARSER_H_


/**
 * \addtogroup script
 * @{
 *
 * \file ScriptParser.h
 * \brief It contains the declaration of ScriptParser class.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */


#include "gdb/common.h"
#include "gdb/Exception.h"


/*
 * Forward declaration of sparksee_core classes.
 */
namespace sparksee_core
{
    class FileReader;
}

/*
 * Forward declaration.
 */
namespace sparksee 
{
    namespace gdb 
    {
        class Sparksee;
        class Database;
        class Session;
        class Graph;
    }
}


BEGIN_SPARKSEE_SCRIPT_NAMESPACE


/**
 * \brief ScriptParser state.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */
enum ScriptParserState
{
    /**
     * \brief Initial and 'there is no problem for the moment' state.
     */
    Parsing,
    /**
     * \brief Finished successfully state.
     */
    Parsed,
    /**
     * \brief Error case.
     */
    SyntaxError,
    /**
     * \brief Error case.
     */
    ParserFailure,
    /**
     * \brief Error case.
     */
    StackOverflow,
    /**
     * \brief Error case.
     */
    TokenizerError,
    /**
     * \brief Error case when executing generated code.
     */
    ExecutionError
};


/**
 * \brief Easy STL printing operator redefinition.
 *
 * It allows to do: ... << sparksee::script::SyntaxError << ...
 */
std::wostream & operator << (std::wostream & wostrm, 
                             const enum ScriptParserState & state);


/**
 * \brief Extra data to be shared between the Parser (Parser.cpp) 
 * and the ScriptParser.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */
class ScriptParserData
{
private:
    
    /**
     * \brief Sparksee instance.
     */
    sparksee::gdb::Sparksee * sparksee;
    /**
     * \brief Database instance.
     */
    sparksee::gdb::Database * db;
    /**
     * \brief Session instance.
     */
    sparksee::gdb::Session * sess;
    /**
     * \brief Graph instance.
     */
    sparksee::gdb::Graph * graph;
    /**
     * \brief State.
     */
    ScriptParserState state;
    /**
     * \brief Output stream.
     */
    std::wostream * ostrm;
    /**
     * \brief Error output stream.
     */
    std::wostream * estrm;
    /**
     * \brief If TRUE parsed script will be executed,
     * if FALSE it will be just parsed.
     */
    sparksee::gdb::bool_t exec;
    

    /**
     * \brief The current timestamp format
     */
    std::wstring timestampFormat;

    /**
     * \brief Non implemented to avoid use of copy constructor.
     */
    ScriptParserData(const ScriptParserData &spd);
    
    /**
     * \brief Non implemented to avoid use of assignment operator.
     */
    ScriptParserData & operator =(const ScriptParserData &spd);

public:
    
    /**
     * \brief Creates a new instance.
     *
     * Initial state: Parsing.
     * \param execute [in] If TRUE parsed script will be executed,
     * if FALSE it will be just parsed.
     * \param outStrm [in] Output stream. If NULL no output is done.
     * \param errStrm [in] Error output stream. If NULL no error output is done.
     */
    ScriptParserData(sparksee::gdb::bool_t execute,
                     std::wostream * outStrm, std::wostream * errStrm);
    
    /**
     * \brief Destructor.
     *
     * Closes the storage if it is opened.
     */
    virtual ~ScriptParserData();
    
    /**
     * \brief Gets if the parsed script must be executed or not.
     * \return If TRUE parsed script will be executed,
     * if FALSE it will be just parsed.
     */
    sparksee::gdb::bool_t Execute() const
    {
        return exec;
    }
    
    /**
     * \brief Gets the output stream.
     * \return A reference to the output stream.
     */
    std::wostream * GetOutputStream()
    {
        return ostrm;
    }
    
    /**
     * \brief Gets the error output stream.
     * \return A reference to the output stream.
     */
    std::wostream * GetErrorStream()
    {
        return estrm;
    }
    
    /**
     * \brief Opens the storage.
     *
     * It initializes Sparksee, Database, Session and Graph instances.
     *
     * \param path [in] Path of the Sparksee database file.
     * \param alias [in] Alias of the Sparksee database.
     * \param create [in] If TRUE, it opens an existing Sparksee database, 
     * if FALSE it creates a new one one. Also, when opening it validates 
     * the given alias matches the real one.
     * \return TRUE if ok, FALSE in case of ERROR.
     */
    sparksee::gdb::bool_t OpenStorage(const std::wstring & path, 
                                 const std::wstring & alias, 
                                 sparksee::gdb::bool_t open);
    
    /**
     * \brief Closes the storage.
     *
     * It closes Sparksee, Database, Session and Graph instances.
     */
    sparksee::gdb::bool_t CloseStorage();

    /**
     * \brief Gets if the storage is opened.
     * \return TRUE if the storage is opened, FALSE otherwise.
     */
    sparksee::gdb::bool_t StorageIsOpened();
    
    /**
     * \brief Gets the Graph.
     * \pre Storage must be opened.
     */
    sparksee::gdb::Graph * GetGraph()
    {
        return graph;
    }
    
    /**
     * \brief Gets the state of the parser.
     * \return The state of the parser.
     */
    ScriptParserState GetState() const
    {
        return state;
    }
    
    /**
     * \brief Sets the state of the parser.
     * \param st [in] The state of the parser.
     */
    void SetState(ScriptParserState st)
    {
        state = st;
    }
    
    /**
     * \brief Gets if the current state is not an error state.
     *
     * See ScriptParserState.
     *
     * Error states: all but Parsing and Parsed
     */
    sparksee::gdb::bool_t IsNoErrorState() const
    {
        return ((Parsing == state) 
                || (Parsed == state));
    }


    /**
     * \brief Gets the current timestamp format.
     * \return The current timestamp format string.
     */
    const std::wstring & GetTimestampFormat() const
    {
        return timestampFormat;
    }

    /**
     * \brief Sets the current timestamp format.
     * \param format [in] The new timestamp format string
     */
    void SetTimestampFormat(const std::wstring &format)
    {
        timestampFormat = format;
    }
};


/**
 * \brief ScriptParser.
 *
 * The ScriptParser can create schemas and load data from a set of commands
 * in a sparksee script.
 *
 * A SPARKSEE script contains an ordered list of commands. ScriptParser will execute 
 * each one of them in order. Commands may create schemas, 
 * define nodes and edges, and load data into a previous defined SPARKSEE schema. 
 *
 * Check out the 'Scripting' chapter in the SPARKSEE User Manual for a comprehensive
 * explanation on the grammar of the SPARKSEE commands and how they work.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */
class SPARKSEE_EXPORT ScriptParser
{
private:
    
    /**
     * \brief Output stream.
     *
     * If NULL, standard output will be used.
     */
    std::wostream * outStrm;
    
    /**
     * \brief Error stream.
     *
     * If NULL, standard error output will be used.
     */
    std::wostream * errStrm;
    
    /**
     * \brief Non implemented to avoid use of copy constructor.
     */
    ScriptParser(const ScriptParser &sp);
    
    /**
     * \brief Non implemented to avoid use of assignment operator.
     */
    ScriptParser & operator =(const ScriptParser &sp);

public:

    /**
     * \brief Constructor.
     */
    ScriptParser();
    
    /**
     * \brief Destructor.
     */
    virtual ~ScriptParser();
    
    /**
     * \brief Sets the output log.
     *
     * If not set, output log corresponds to standard output.
     * \param path [in] Path of the output log.
     * \exception IOException If bad things happen opening the file.
     */
    void SetOutputLog(const std::wstring & path)
    throw(sparksee::gdb::IOException);
    
    /**
     * \brief Sets the error log.
     *
     * If not set, error log corresponds to standard error output.
     * \param path [in] Path of the error log.
     * \exception IOException If bad things happen opening the file.
     */
    void SetErrorLog(const std::wstring & path)
    throw(sparksee::gdb::IOException);

    /**
     * \brief Parser the given input stream.
     * \param fileReader [in] Input file reader.
     * \param execute [in] If TRUE the script is executed, if FALSE
     * it is just parsed.
     * \return TRUE if ok, FALSE in case of error.
     */
    sparksee::gdb::bool_t Parse(sparksee_core::FileReader *fileReader, sparksee::gdb::bool_t execute);
    
    /**
     * \brief Parses the given input file.
     * \param path [in] Input file path.
     * \param execute [in] If TRUE the script is executed, if FALSE
     * it is just parsed.
     * \param localeStr [in] The locale string for reading the input file. See CSVReader.
     * \return TRUE if ok, FALSE in case of error.
     * \exception IOException If bad things happen opening the file.
     */
    sparksee::gdb::bool_t Parse(const std::wstring & path, sparksee::gdb::bool_t execute, const std::wstring &localeStr)
    throw(sparksee::gdb::IOException);

    /**
     * \brief Writes an script with the schema definition for the given database.
     * \param path [in] Filename of the script to be writen.
     * \param db [in] Database.
     * \exception IOException If bad things happen opening or writing the file.
     */
    static void GenerateSchemaScript(const std::wstring & path, sparksee::gdb::Database & db)
    throw(sparksee::gdb::IOException);
};


END_SPARKSEE_SCRIPT_NAMESPACE


/** @} */
#endif // _SPARKSEE_SCRIPTPARSER_H_
