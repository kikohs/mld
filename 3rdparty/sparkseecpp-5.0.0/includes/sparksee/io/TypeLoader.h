/*
 * SPARKSEE: a library for out-of-core graph-based data storage and query processing.
 *
 * See the file LICENSE.txt for redistribution information.
 *
 * Copyright (c) 2005-2010 Sparsity Technologies, Barcelona (Spain)
 */

#ifndef _SPARKSEE_TYPELOADER_H_
#define _SPARKSEE_TYPELOADER_H_

/**
 * \addtogroup io
 * @{
 *
 * \file TypeLoader.h
 * \brief It contains the definition of TypeLoader classes.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */

#include "io/RowReader.h"
#include "gdb/Exception.h"
#include "gdb/Graph.h"
#include <vector>


/*
 * Forward declaration of sparksee_core classes.
 */
namespace sparksee_core
{
    class Locale;
}

BEGIN_SPARKSEE_IO_NAMESPACE


/**
 * \brief Provides information about the progress of a TypeLoader instance.
 *
 * Check out the 'Data import' section in the SPARKSEE User Manual for more
 * details on this.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */
class SPARKSEE_EXPORT TypeLoaderEvent 
{
    friend class TypeLoader; // to call constructor and set values
    
private:
    
    /**
     * \brief Object type identifier.
     */
    sparksee::gdb::type_t type;
    /**
     * \brief Current number of objects created.
     */
    sparksee::gdb::int64_t count;
    /**
     * \brief Graph to be loaded.
     */
    sparksee::gdb::Graph * graph;
    /**
     * Current phases.
     */
    sparksee::gdb::int32_t phase;
    /**
     * Total number of phases.
     */
    sparksee::gdb::int32_t phases;
    /**
     * Current partition.
     */
    sparksee::gdb::int32_t partition;
    /**
     * Total number of partitions.
     */
    sparksee::gdb::int32_t partitions;
    /**
     * Total number of steps of the current partition.
     */
    sparksee::gdb::int32_t steps;
    /**
     * \brief Last event or not.
     */
    sparksee::gdb::bool_t last;

    /**
     * \brief Creates a new instance.
     */
    TypeLoaderEvent()
    {
    }
    
public:
    
    /**
     * \brief Destructor.
     */
    virtual ~TypeLoaderEvent()
    {
    }
    
    /**
     * \brief Gets the type identifier.
     * \return The type identifier.
     */
    sparksee::gdb::type_t GetTypeId() const
    {
        return type;
    }
    
    /**
     * \brief Gets the current number of objects created.
     * \return The current number of objects created.
     */
    sparksee::gdb::int64_t GetCount() const
    {
        return count;
    }
    

    /**
     * \brief Gets the current phase.
     * \return The current phase.
     */
    sparksee::gdb::int32_t GetPhase() const
    {
        return phase;
    }
    /**
     * \brief Gets the total number of phases.
     * \return The total number of phases.
     */
    sparksee::gdb::int32_t GetTotalPhases() const
    {
        return phases;
    }
    /**
     * \brief Gets the current partition.
     * \return The current partition.
     */
    sparksee::gdb::int32_t GetPartition() const
    {
        return partition;
    }
    /**
     * \brief Gets the total number of partitions.
     * \return The total number of partitions.
     */
    sparksee::gdb::int32_t GetTotalPartitions() const
    {
        return partitions;
    }
    /**
     * \brief Gets the total number of steps in the current partition.
     * \return The total number steps in the current partition.
     */
    sparksee::gdb::int32_t GetTotalPartitionSteps() const
    {
        return steps;
    }


    /**
     * \brief Gets if this is the last event or not.
     * \return TRUE if this is the last event, FALSE otherwise.
     */
    sparksee::gdb::bool_t IsLast() const
    {
        return last;
    }
    
private:
    
    /**
     * \brief Non implemented to avoid use of copy constructor.
     */
    TypeLoaderEvent(const TypeLoaderEvent & tle);
    
    /**
     * \brief Non implemented to avoid use of assignment operator.
     */
    TypeLoaderEvent & operator =(const TypeLoaderEvent & tle);
};


/**
 * \brief Easy STL printing operator redefinition.
 */
std::wostream & operator << (std::wostream & wostrm, 
                             const TypeLoaderEvent & ev);


/**
 * \brief Interface to be implemented to receive TypeLoaderEvent events 
 * from a TypeLoader.
 * 
 * Check out the 'Data import' section in the SPARKSEE User Manual for more
 * details on this.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */
class SPARKSEE_EXPORT TypeLoaderListener 
{
public:
    
    /**
     * Method to receive events from a {@link Loader}.
     * @param ev {@link Loader.LoaderEvent} with information from a
     * running {@link Loader}.
     */
    virtual void NotifyEvent(const TypeLoaderEvent & ev) = 0;

    /**
     * \brief Destructor.
     */
    virtual ~TypeLoaderListener()
    {
    }
    
private:
    
    /**
     * \brief Non implemented to avoid use of copy constructor.
     */
    TypeLoaderListener(const TypeLoaderListener & tel);
    
    /**
     * \brief Non implemented to avoid use of assignment operator.
     */
    TypeLoaderListener & operator =(const TypeLoaderListener & tel);
    
protected:
    
    /**
     * Protected because none should instantiate a RowWriter. Just inherited
     * classes may use this empty constructor.
     */
    TypeLoaderListener()
    {
    }
};


/**
 * \brief Base TypeLoader class.
 *
 * Base class to load a node or edge type from a graph using a RowReader.
 * 
 * TypeLoaderListener can be registered to receive information about
 * the progress of the load process by means of TypeLoaderEvent.
 * The default frequency of notification to listeners is 100000.
 *
 * Check out the 'Data import' section in the SPARKSEE User Manual for more
 * details on this.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */
class SPARKSEE_EXPORT TypeLoader
{
private:
    
    /**
     * \brief Input RowReader.
     */
    RowReader * rowReader;
    /**
     * \brief The locale of the read data.
     */
    sparksee_core::Locale* locale;
    /**
     * \brief Object type identifier to be loaded.
     */
    sparksee::gdb::type_t type;
    /**
     * \brief Type data of the type to be loaded.
     */
    const sparksee::gdb::Type * tdata;
    /**
     * \brief Stores if the type to be loaded corresponds to a node type or
     * not (edge type).
     */
    sparksee::gdb::bool_t isNodeType;
    /**
     * \brief Graph to be loaded.
     */
    sparksee::gdb::Graph * graph;
    /**
     * \brief Frequency.
     *
     * An LoaderEvent is sent to all listeners when
     * frequency steps have been executed.
     */
    sparksee::gdb::int32_t frequency;
    /**
     * \brief List of listeners.
     */
    std::vector<TypeLoaderListener *> listeners;
    /**
     * Number of partitions.
     * <p>
     * It is used when working in mode {@link Mode#N_PHASES}.
     */
    sparksee::gdb::int32_t partitions;
    /**
     * Column indexes in the {@link RowReader} for the attributes.
     */
    std::vector<sparksee::gdb::int32_t> attrArrayPos;
    /**
     * \brief Attribute identifiers to be loaded.
     */
    std::vector<sparksee::gdb::attr_t> attributes;
    /**
     * \brief In case of load an edge type, the position (index column)
     * for the head value, -1 otherwise.
     */
    sparksee::gdb::int32_t headPos;
    /**
     * \brief In case of load an edge type, the position (index column)
     * for the tail value, -1 otherwise.
     */
    sparksee::gdb::int32_t tailPos;
    /**
     * \brief In case of load an edge type, the attribute identifier
     * to get the value to be dumped for the head, InvalidAttribute otherwise.
     */
    sparksee::gdb::attr_t headAttr;
    /**
     * \brief In case of load and edge, the attribute data corresponding to
     * headAttr, NULL otherwise.
     */
    const sparksee::gdb::Attribute * headAttrData;
    /**
     * \brief In case of load an edge type, the attribute identifier
     * to get the value to be dumped for the tail, InvalidAttribute otherwise.
     */
    sparksee::gdb::attr_t tailAttr;
    /**
     * \brief In case of load and edge, the attribute data corresponding to
     * tailAttr, NULL otherwise.
     */
    const sparksee::gdb::Attribute * tailAttrData;
    /**
     * \brief If set, errors are logged here instead of throwing an exception.
     */
    std::wofstream * logError;
    /**
     * \brief, If set, the errors are logged or thrown. Otherwise they are ignored.
     */
    sparksee::gdb::bool_t logStatus;

    /**
     * \brief The timestamp format string
     * If it's NULL, the default timestamp format will be used.
     */
    std::string *tsFormat;

    /**
     * \brief  Array with all the valid default timestamp format strings.
     * This formats will be used only when no timestamp format had been specified.
     */
    static const char* DefaultTimeStampFormats[];
    /**
     * \brief The number of strings in defaultTimeStampFormats array.
     */
    static const sparksee::gdb::int32_t DefaultTimeStampFormatsSize;


    /**
     * \brief Default Frequency.
     */
    static const sparksee::gdb::int32_t DefaultFrequency;

    /**
     * General method to load data on a type.
     * @param rr {@link RowReader} with all the data
     * @param create If <code>True</code> create a new Object for each row
     * of the {@link RowReader}.
     * @param load Each position has the attribute identifier to load with the data
     * of the same column of the given {@link RowReader} for each object.
     * If <code>load[column] == Graph.INVALID_ATTRIBUTE</code>, the column won't be loaded.
     * @param tmp If the position is <code>True</code>, this column of the
     * {@link RowReader} row is written on a temporal file.
     * @param e {@Link LoaderEvent} passed to the {@link LoaderListener}s.
     * @param aux If <code>create == True</code> aux is the column used to write
     * the object identifier on the temporal file (If it is created).
     * If <code>create == False</code> aux is the colum of the RowReader that has
     * the object identifier.
     * @param partitionColumn Column used to compare the value of the row
     * (attribute value) with the partition number to check if is load in
     * this partition. (Only used with <code>N_PHASES</code> and
     * <code>partitions != 1 </code>).
     * @return If a temporal file is created, return the {@link File}. If not,
     * return null.
     * @throws IOException If bad things happen reading the
     * {@link RowReader} or writting the temporal file.
     */
    std::wstring* LoadProcess(RowReader &rr, sparksee::gdb::bool_t create,
                              std::vector<sparksee::gdb::attr_t> &load, std::vector<sparksee::gdb::bool_t> &tmp, TypeLoaderEvent &e,
                              sparksee::gdb::int32_t aux, sparksee::gdb::int32_t partitionColumn)
    throw(sparksee::gdb::IOException, sparksee::gdb::Error);
    
    /**
     * Gets a {@link Value} from the given String.
     * @param dataType Attribute data type of the attribute in the {@link Graph}.
     * @param valueString String representation.
     * @param value The result Value.
     * @throws IOException
     */
    void GetValueFromString(sparksee::gdb::DataType dataType, const std::wstring &valueString, sparksee::gdb::Value &value)
    throw(sparksee::gdb::IOException, sparksee::gdb::Error);
    
    /**
     * A simple hash function for the given Value.
     * @return Returns an int32_t value computed from the given value.
     */
    sparksee::gdb::int32_t ValueHashCode(const sparksee::gdb::Value &value);


protected:
    
    /**
     * Load can work in different ways.
     */
    enum Mode
    {
        /**
         * Performs the load in a phases.
         * <p>
         * Load all objects an attributes at the same time.
         */
        ONE_PHASE,
        /**
         * Performs the load in two phases.
         * <p>
         * Firstly load all objects (and create them if necessary) and
         * secondly loads all the attributes.
         * <p>
         * Working on this mode it is necessary to build a temporary file.
         */
        TWO_PHASES,
        /**
         * Performs the load in N phases.
         * <p>
         * Firstly load all objects (and create them if necessary) and
         * secondly loads all the attributes. But in this case, attributes
         * are loaded one by one. This way, if there are three attributes,
         * then 4 traverses to the {@link RowReader} are necessary.
         * <p>
         * Working on this mode it is necessary to build a temporary file.
         */
        N_PHASES
    };
    
    /**
     * \brief Checks that all the required settings are ready to run.
     * \return Returns true if all the settings are ready.
     */
    sparksee::gdb::bool_t CanRun();

    /**
     * \brief Runs load process.
     * \exception IOException If bad things happen reading from the RowReader.
     * \param ph [in] The load mode.
     * \param par [in] Number of horizontal partitions to perform the load.
     */
    void Run(Mode ph, sparksee::gdb::int32_t par)
    throw(sparksee::gdb::IOException, sparksee::gdb::Error);
    
    /**
     * \brief Creates a new instance with the minimum common required arguments.
     * \param rr [in] Input RowReader.
     * \param g [in] Graph.
     * \param t [in] Type identifier.
     * \param attrs [in] Attribute identifiers to be loaded
     * \param attrsPos [in] Attribute positions (column index >=0)
     */
    TypeLoader(RowReader & rr, sparksee::gdb::Graph & g, sparksee::gdb::type_t t,
               sparksee::gdb::AttributeList &attrs,
               sparksee::gdb::Int32List &attrsPos);
    
    /**
     * \brief Creates a new instance.
     */
    TypeLoader();

    /**
     * \brief Notifies progress to all registered listeners.
     * \param ev [in] Progress event to be notified.
     */
    void NotifyListeners(const TypeLoaderEvent & ev);
    

    /**
     * \brief Sets the attribute that will be used to find the head of the edge.
     * This method is protected because only the Edge loaders should have it.
     * \param attr [in] Head Attribute
     */
    void SetHeadAttribute(sparksee::gdb::attr_t attr);
    /**
     * \brief Sets the position of the head attribute in the source data.
     * This method is protected because only the Edge loaders should have it.
     * \param pos [in] Head position
     */
    void SetHeadPosition(sparksee::gdb::int32_t pos);
    /**
     * \brief Sets the attribute that will be used to find the tail of the edge.
     * This method is protected because only the Edge loaders should have it.
     * \param attr [in] Tail Attribute
     */
    void SetTailAttribute(sparksee::gdb::attr_t attr);
    /**
     * \brief Sets the position of the tail attribute in the source data.
     * This method is protected because only the Edge loaders should have it.
     * \param pos [in] Tail position
     */
    void SetTailPosition(sparksee::gdb::int32_t pos);

public:
    
    /**
     * \brief Sets a log error file.
     *
     * By default errors are thrown as a exception and the load process
     * ends. If a log file is set, errors are logged there and the load process
     * does not stop.
     * \exception IOException If bad things happen opening the file.
     */
    void SetLogError(const std::wstring & path)
    throw(sparksee::gdb::IOException);
    
    /**
     * \brief Truns off all the error reporting.
     * The log file will not be created and no exceptions for invalid data will be thrown.
     * If you just want to turn off the logs, but abort at the first error what you
     * should do is not call this method and not set a logError file.
     */
    void SetLogOff();

    /**
     * \brief Destructor.
     */
    virtual ~TypeLoader();
    
    /**
     * \brief Registers a new listener.
     * \param [in] tel TypeLoaderListener to be registered.
     */
    void Register(TypeLoaderListener & tel);

    /**
     * \brief Run the loader
     */
    virtual void Run()
    throw(sparksee::gdb::IOException, sparksee::gdb::Error) = 0;
    
    /**
     * \brief Run the loader for two phases loading.
     * <p>
     * Firstly load all objects (and create them if necessary) and
     * secondly loads all the attributes.
     * <p>
     * Working on this mode it is necessary to build a temporary file.
     */
    virtual void RunTwoPhases()
    throw(sparksee::gdb::IOException, sparksee::gdb::Error) = 0;
    
    /**
     * \brief Run the loader for N phases loading.
     * <p>
     * Firstly load all objects (and create them if necessary) and
     * secondly loads all the attributes. But in this case, attributes
     * are loaded one by one. This way, if there are three attributes,
     * then 4 traverses are necessary.
     * <p>
     * Working on this mode it is necessary to build a temporary file.
     * \param partitions [in] Number of horizontal partitions to perform the load.
     */
    virtual void RunNPhases(sparksee::gdb::int32_t partitions)
    throw(sparksee::gdb::IOException, sparksee::gdb::Error) = 0;

    /**
     * \brief Sets the input data source.
     * \param rr [in] Input RowReader.
     */
    void SetRowReader(RowReader & rr);
    /**
     * \brief Sets the graph where the data will be loaded.
     * \param g [in] Graph.
     */
    void SetGraph(sparksee::gdb::Graph & g);
    /**
     * \brief Sets the locale that will be used to read the data.
     * It should match the locale used in the rowreader.
     * \param localeStr [in] The locale string for the read data. See CSVReader.
     */
    void SetLocale(const std::wstring &localeStr);
    /**
     * \brief Sets the type to be loaded.
     * \param t [in] Type identifier.
     */
    void SetType(sparksee::gdb::type_t t);
    /**
     * \brief Sets the list of Attributes.
     * \param attrs [in] Attribute identifiers to be loaded
     */
    void SetAttributes(sparksee::gdb::AttributeList &attrs);
    /**
     * \brief Sets the list of attribute positions.
     * attrsPos [in] Attribute positions (column index >=0).
     */
    void SetAttributePositions(sparksee::gdb::Int32List &attrsPos);
    /**
     * \brief Sets a specific timestamp format.
     *
     */
    void SetTimestampFormat(const std::wstring &timestampFormat);

    /**
     * \brief Sets the frequency of listener notification.
     * freq [in] Frequency in number of rows managed to notify progress to all listeners
     */
    void SetFrequency(sparksee::gdb::int32_t freq);

};


END_SPARKSEE_IO_NAMESPACE

/** @} */
#endif // _SPARKSEE_TYPELOADER_H_
