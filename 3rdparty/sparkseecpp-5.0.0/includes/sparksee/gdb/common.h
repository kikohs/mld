/*
 * SPARKSEE: a library for out-of-core graph-based data storage and query processing.
 *
 * See the file LICENSE.txt for redistribution information.
 *
 * Copyright (c) 2005-2010 Sparsity Technologies, Barcelona (Spain)
 */

#ifndef _SPARKSEE_COMMON_H_
#define _SPARKSEE_COMMON_H_

/**
 * \addtogroup gdb
 * @{
 *
 * \file common.h
 * \brief It contains common includes and definitions as well as set basic
 * data types and enumerations.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */


#ifdef _SPARKSEE_DEBUG
#  include <assert.h>
#  define SPARKSEE_DEBUG
#  include <assert.h>
#  define SPARKSEE_ASSERT(_cond) assert(_cond)
#  define SPARKSEE_PERMANENT_ASSERT(_cond) SPARKSEE_ASSERT(_cond)
#else // !_SPARKSEE_DEBUG
#  define SPARKSEE_ASSERT(_cond)
#  define SPARKSEE_PERMANENT_ASSERT(_cond) if (!(_cond)) { abort(); }
#endif // _SPARKSEE_DEBUG

/*
 * Common system includes.
 */
#include <stdlib.h>
#include <string>
#include <vector>
#include <iostream>


/**
 * \brief Begining macro for the sparksee namespace.
 */
#define BEGIN_SPARKSEE_NAMESPACE namespace sparksee {
/**
 * \brief Ending macro for the sparksee namespace.
 */
#define END_SPARKSEE_NAMESPACE }
/**
 * \brief Begining macro for the sparksee::gdb namespace.
 */
#define BEGIN_SPARKSEE_GDB_NAMESPACE BEGIN_SPARKSEE_NAMESPACE namespace gdb {
/**
 * \brief Ending macro for the sparksee::gdb namespace.
 */
#define END_SPARKSEE_GDB_NAMESPACE END_SPARKSEE_NAMESPACE }
/**
 * \brief Begining macro for the sparksee::io namespace.
 */
#define BEGIN_SPARKSEE_IO_NAMESPACE BEGIN_SPARKSEE_NAMESPACE namespace io {
/**
 * \brief Ending macro for the sparksee::io namespace.
 */
#define END_SPARKSEE_IO_NAMESPACE END_SPARKSEE_NAMESPACE }
/**
 * \brief Begining macro for the sparksee::script namespace.
 */
#define BEGIN_SPARKSEE_SCRIPT_NAMESPACE BEGIN_SPARKSEE_NAMESPACE namespace script {
/**
 * \brief Ending macro for the sparksee::script namespace.
 */
#define END_SPARKSEE_SCRIPT_NAMESPACE END_SPARKSEE_NAMESPACE }
/**
 * \brief Begining macro for the sparksee::algorithms namespace.
 */
#define BEGIN_SPARKSEE_ALGORITHMS_NAMESPACE BEGIN_SPARKSEE_NAMESPACE namespace algorithms {
/**
 * \brief Ending macro for the sparksee::algorithms namespace.
 */
#define END_SPARKSEE_ALGORITHMS_NAMESPACE END_SPARKSEE_NAMESPACE }
/**
 * \brief Begining macro for the sparksee::test namespace.
 */
#define BEGIN_SPARKSEE_TEST_NAMESPACE BEGIN_SPARKSEE_NAMESPACE namespace test {
/**
 * \brief Ending macro for the sparksee::test namespace.
 */
#define END_SPARKSEE_TEST_NAMESPACE END_SPARKSEE_NAMESPACE }

#ifdef WIN32
#  pragma warning( disable : 4290 ) // disable warning: Windows accepts throw specifier but does not implement it, resulting in the C4290 warning.
#  pragma warning( disable : 4251 ) // disable warning: SPARKSEE gdb has a lot non client usable private variables. TODO: It might be interesting to check more thoroughly this issue. 
#  ifdef sparksee_EXPORTS // automatically defined by cmake when linking a dynamic library
#    define SPARKSEE_EXPORT __declspec(dllexport)
#  else
#    define SPARKSEE_EXPORT __declspec(dllimport)
#  endif
#else
#  define SPARKSEE_EXPORT
#endif


BEGIN_SPARKSEE_GDB_NAMESPACE    

/**
 * \brief Byte type.
 */
typedef unsigned char byte_t;
/**
 * \brief Boolean type.
 */
typedef bool bool_t;
/**
 * \brief Character type.
 */
typedef char char_t;
/**
 * \brief Unicode character type.
 */
typedef wchar_t uchar_t;
/**
 * \brief 32-bit signed integer type.
 */
typedef signed int int32_t;
/**
 * \brief 64-bit signed integer type.
 */
typedef signed long long int64_t;
/**
 * \brief 64-bit double type.
 */
typedef double double64_t;
/**
 * \brief Graph node or edge type type.
 */
typedef int32_t type_t;
/**
 * \brief Graph attribute type.
 */
typedef int32_t attr_t;
/**
 * \brief Graph OID type.
 */
typedef int64_t oid_t;
/**
 * \brief Color codified as RGB 32-bit int.
 *
 * Bits 24-31 are alpha, 16-23 are red, 8-15 are green, 0-7 are blue.
 */
typedef int32_t ColorRGB;


/**
 * \brief Object type enumeration.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */
enum ObjectType
{
    /**
     * \brief Node object type.
     */
    Node,
    /**
     * \brief Edge object type.
     */
    Edge
};


/**
 * \brief Data type (domain) enumeration.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */
enum DataType
{
    /**
     * \brief Boolean data type.
     */
    Boolean,
    
    /**
     * \brief 32-bit signed integer data type.
     */
    Integer,
    
    /**
     * \brief 64-bit signed integer data type.
     */
    Long,
    
    /**
     * \brief 64-bit signed double data type.
     */
    Double,
    
    /**
     * \brief Distance from Epoch (UTC) time in milliseconds precision.
     *
     * It just works properly with timestamps in the range
     * ['1970-01-01 00:00:01' UTC, '2038-01-19 03:14:07' UTC].
     */
    Timestamp,
    
    /**
     * \brief Unicode string data type.
     *
     * 2048 characters maximum length.
     */
    String,
    
    /**
     * \brief Large unicode character object (CLOB) data type.
     * \see TextStream
     */
    Text,
    
    /**
     * \brief Object identifier (OID) data type.
     */
    OID
};

/**
 * \brief Easy STL printing operator redefinition.
 *
 * It allows to do: ... << sparksee::gdb::String << ...
 */
std::wostream & operator << (std::wostream & wostrm, 
                             const enum DataType & dt);
const uchar_t* DataTypeToString(const enum DataType & dt);

/**
 * \brief Condition operators enumeration.
 *
 * It is mainly used in the attribute-based graph select operations.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */
enum Condition
{
    /**
     * \brief Equal condition (==).
     *
     * Null values can be used together with this condition to retrieve all objects
     * having a null value for an attribute.
     */
    Equal,
    
    /**
     * \brief Greater or equal condition (>=).
     *
     * Null values cannot be used together with this condition.
     */
    GreaterEqual,
    
    /**
     * \brief Greater than condition (>).
     *
     * Null values cannot be used together with this condition.
     */
    GreaterThan,
    
    /**
     * \brief Less or equal condition (<=).
     *
     * Null values cannot be used together with this condition.
     */
    LessEqual,
    
    /**
     * \brief Less than condition (<).
     *
     * Null values cannot be used together with this condition.
     */
    LessThan,
    
    /**
     * \brief Not equal condition (!=).
     *
     * Null values can be used together with this condition to retrieve all objects
     * having a non-null value for an attribute.
     */
    NotEqual,
    
    /**
     * \brief Substring condition.
     *
     * Null values cannot be used together with this condition.
     *
     * This condition can just be used together with String values. It allows for searching
     * substrings (case sensitive). Ex:
     * <pre>
     * 'AAABBBCCCD' Like 'BBB'   returns TRUE
     * 'AAABBBCCCD' Like 'bbb'   returns FALSE
     * 'AAABBBCCCD' Like 'E'     returns FALSE
     * </pre>
     */
    Like,
    
    /**
     * \brief Substring (no case sensitive) condition.
     *
     * Null values cannot be used together with this condition.
     *
     * This condition can just be used together with String values. It allows for searching
     * substrings (no case sensitive). Ex:
     * <pre>
     * 'AAABBBCCCD' LikeNoCase 'BBB'   returns TRUE
     * 'AAABBBCCCD' LikeNoCase 'bbb'   returns TRUE
     * 'AAABBBCCCD' LikeNoCase 'E'     returns FALSE
     * </pre>
     */
    LikeNoCase,
    
    /**
     * \brief In range operator condition ([x,y]).
     *
     * Null values cannot be used together with this condition.
     */
    Between,
    
    /**
     * \brief Regular expression condition.
     *
     * Null values cannot be used together with this condition.
     *
     * This condition can just be used together with String values. 
     *
     * Regular expression format conforms most of the POSIX
     * Extended Regular Expressions so it is case sensitive.
     *
     * See the 'Regular expressions' section in the 'SPARKSEE User Manual' for
     * details.
     */
    RegExp
};


/**
 * \brief Order enumeration.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */
enum Order
{
    /**
     * \brief From lower to higher.
     */
    Ascendent,

    /**
     * \brief From higher to lower.
     */
    Descendent
};


/**
 * \brief Edges direction enumeration.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */
enum EdgesDirection
{
    /**
     * \brief In-going edges.
     */
    Ingoing,
    
    /**
     * \brief Out-going edges.
     */
    Outgoing,
    
    /**
     * \brief In-going or out-going edges.
     */
    Any
};


/**
 * \brief Attribute kind enumeration.
 *
 * It determines the indexing-capabilities of an attribute.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */
enum AttributeKind
{
    /**
     * \brief Basic attribute (non indexed attribute).
     */
    Basic,
    /**
     * \brief Indexed attribute.
     */
    Indexed,
    /**
     * \brief Unique attribute (indexed + unique restriction).
     *
     * Unique restriction sets two objects cannot have the same value for
     * an attribute but NULL.
     */
    Unique
};


/**
 * \brief Easy STL printing operator redefinition.
 *
 * It allows to do: ... << sparksee::gdb::Basic << ...
 */
std::wostream & operator << (std::wostream & wostrm, 
                             const enum AttributeKind & ak);
const uchar_t* KindToString(const enum AttributeKind & ak);

/**
 * \brief Log level enumeration.
 *
 * Log level priority order is as follows, from minimum 
 * to maximum log information: 
 * Off (log is disabled), Severe, Warning, Info, Config, Fine, Debug.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */
enum LogLevel 
{
    /**
     * \brief Disable log.
     */
    Off,
    
    /**
     * \brief Severe log level.
     *
     * This is the lower log level, just errors are shown.
     */
    Severe,
    
    /**
     * \brief Warning log level.
     *
     * Errors and warnings are shown.
     */
    Warning,
    
    /**
     * \brief Info log level.
     *
     * Errors, warnings and information messages are shown.
     */
    Info,
    
    /**
     * \brief Config log level.
     *
     * Errors, warnings, information messages and configuration details
     * of the different components are shown.
     */
    Config,
    
    /**
     * \brief Fine log level.
     *
     * This is the higher and finest public log level, everything is dumped 
     * to the log.
     */
    Fine,
    
    /**
     * \brief Debug log level.
     *
     * This is for Sparksee development purposes and just works with debug versions 
     * of the library.
     */
    Debug
};


/**
 * \brief Export type.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */
enum ExportType
{
    /**
     * \brief Export to Graphviz format.
     * 
     * Graphviz home page: http://www.graphviz.org
     */
    Graphviz,
    
    /**
     * \brief Export to GraphML format.
     * 
     * GraphML home page: http://graphml.graphdrawing.org/
     */
    GraphML,
    
    /**
     * \brief Export to YGRAPHML format.
     * 
     * It is an GraphML format extended with a set of yWorks 
     * ("http://www.yworks.com") extensions. Thus, it allows for the
     * visualization of the exported graph with the public 
     * yEd visualization tool ("http://www.yworks.com/products/yed").
     */
    YGraphML
};


/**
 * \brief Node shape.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */
enum NodeShape 
{
    /**
     * Box shape.
     */
    Box,
    
    /**
     * Round shape.
     */
    Round
};


/**
 * \brief Platform data and statistics.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */
class SPARKSEE_EXPORT PlatformStatistics 
{
    friend class Platform; // to set private fields
    
private:
    
    /**
     * \brief Numver of CPUs.
     */
    int32_t nCPUs;
    
    /**
     * \brief Time in microseconds (since epoch).
     */
    int64_t realTime;
    
    /**
     * \brief CPU user time in microseconds.
     */
    int64_t userTime;
    
    /**
     * \brief CPU system time in microseconds.
     */
    int64_t sysTime;
    
    /**
     * \brief Physical memory in KB.
     */
    int64_t totalMem;
    
    /**
     * \brief Free physical memory in KB.
     */
    int64_t availMem;
    
    /**
     * \brief Non implemented to avoid use of copy constructor.
     */
    PlatformStatistics(const PlatformStatistics &stats);
    
    /**
     * \brief Non implemented to avoid use of assignment operator.
     */
    PlatformStatistics & operator =(const PlatformStatistics &stats);
    
public:
    
    /**
     * \brief Creates a new instance setting all values to 0.
     */
    PlatformStatistics()
    : nCPUs((int32_t)0)
    , realTime((int64_t)0)
    , userTime((int64_t)0)
    , sysTime((int64_t)0)
    , totalMem((int64_t)0)
    , availMem((int64_t)0)
    {
    }
    
    /**
     * \brief Gets the number of CPUs.
     * \return The number of CPUs.
     */
    int32_t GetNumCPUs() const
    {
        return nCPUs;
    }
    
    /**
     * \brief Gets time in microseconds (since epoch).
     * \return Time in microseconds (since epoch).
     */
    int64_t GetRealTime() const
    {
        return realTime;
    }
    
    /**
     * \brief Gets CPU user time.
     * \return CPU user time.
     */
    int64_t GetUserTime() const
    {
        return userTime;
    }
    
    /**
     * \brief Gets CPU system time.
     * \return CPU system time.
     */
    int64_t GetSystemTime() const
    {
        return sysTime;
    }
    
    /**
     * \brief Gets physical memory size in Bytes.
     * \return Physical memory size in Bytes.
     */
    int64_t GetTotalMem() const
    {
        return totalMem;
    }    
    
    /**
     * \brief Gets avialable (free) memory size in Bytes.
     * \return Avialable (free) memory size in Bytes.
     */
    int64_t GetAvailableMem() const
    {
        return availMem;
    }    
};


/**
 * \brief Platform class.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */
class SPARKSEE_EXPORT Platform
{
public:
    
    /**
     * \brief Gets platform data and statistics.
     * \param stats [in|out] This updates the given PlatformStatistics.
     */
    static void GetStatistics(PlatformStatistics & stats);
};


END_SPARKSEE_GDB_NAMESPACE

/** @} */
#endif // _SPARKSEE_COMMON_H_
