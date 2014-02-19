/*
 * SPARKSEE: a library for out-of-core graph-based data storage and query processing.
 *
 * See the file LICENSE.txt for redistribution information.
 *
 * Copyright (c) 2005-2010 Sparsity Technologies, Barcelona (Spain)
 */

#ifndef _SPARKSEE_VALUE_H_
#define _SPARKSEE_VALUE_H_

/**
 * \addtogroup gdb
 * @{
 *
 * \file Value.h
 * \brief It contains the definitio of Value class.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */

#include "gdb/Handler.h"

/*
 * Forward declaration of sparksee_core classes.
 */
namespace sparksee_core
{
    class Value;
}

BEGIN_SPARKSEE_GDB_NAMESPACE

/*
 * Forward declaration of sparksee classes.
 */
class Graph;


/**
 * \brief Value class.
 *
 * It is a container which stores a value and its data type (domain).
 * A Value can be NULL.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */
class SPARKSEE_EXPORT Value
: private Handler<sparksee_core::Value>
{
    friend class Graph; // to access private handler
    friend class ValuesIterator; // to access private SetVoid(sparksee_core::Value)
    friend class ResultSet; // to access private SetVoid(sparksee_core::Value) and handler
    
public:
    /**
     * \brief Creates a new instance.
     *
     * It creates a NULL Value.
     */
    Value();
    
    /**
     * \brief Copy constructor.
     * \param v [in] Value to be copied.
     */
    Value(const Value & v);

    /**
     * \brief Destructor.
     */
    virtual ~Value();
    
    /**
     * \brief Assignment operator.
     * \param v [in] Value to be copied.
     */
    Value & operator =(const Value & v);
    
    /**
     * \brief Gets if this is a NULL Value.
     * \return TRUE if this is a NULL Value, FALSE otherwise.
     */
    bool_t IsNull() const;
    
    /**
     * \brief Sets the Value to NULL.
     */
    void SetNullVoid();
    
    /**
     * \brief Sets the Value to NULL.
     * \return The calling instance.
     */
    Value & SetNull()
    {
        SetNullVoid();
        return *this;
    }
    
    /**
     * \brief Gets the DataType.
     * 
     * Value cannot be NULL.
     * \return The DataType.
     */
    DataType GetDataType() const;
    
    /**
     * \brief Gets Boolean Value.
     *
     * This must be a non-NULL Boolean Value.
     * \return The Boolean Value.
     */
    bool_t GetBoolean() const;
    
    /**
     * \brief Gets Integer Value.
     *
     * This must be a non-NULL Integer Value.
     * \return The Integer Value.
     */
    int32_t GetInteger() const;
    
    /**
     * \brief Gets Long Value.
     *
     * This must be a non-NULL Long Value.
     * \return The Long Value.
     */
    int64_t GetLong() const;
    
    /**
     * \brief Gets Double Value.
     *
     * This must be a non-NULL Double Value.
     * \return The Double Value.
     */
    double64_t GetDouble() const;
    
    /**
     * \brief Gets Timestamp Value.
     *
     * This must be a non-NULL Timestamp Value.
     * \return The Timestamp Value.
     */
    int64_t GetTimestamp() const;
    
    /**
     * \brief Gets String Value.
     *
     * This must be a non-NULL String Value.
     * \return The String Value.
     */
    const std::wstring & GetString() const;
    
    /**
     * \brief Gets OID Value.
     *
     * This must be an non-NULL OID Value.
     * \return The OID Value.
     */
    oid_t GetOID() const;
    
    /**
     * \brief Sets the Value.
     * \param v [in] New Boolean value.
     */
    void SetBooleanVoid(bool_t v);
    
    /**
     * \brief Sets the Value.
     * \param v [in] Nex Boolean value.
     * \return The calling instance.
     */
    Value & SetBoolean(bool_t v)
    {
        SetBooleanVoid(v);
        return *this;
    }
    
    /**
     * \brief Sets the Value.
     * \param v [in] New Integer value.
     */
    void SetIntegerVoid(int32_t v);
    
    /**
     * \brief Sets the Value.
     * \param v [in] New Integer value.
     * \return The calling instance.
     */
    Value & SetInteger(int32_t v)
    {
        SetIntegerVoid(v);
        return *this;
    }
    
    /**
     * \brief Sets the Value.
     * \param v [in] New Long value.
     */
    void SetLongVoid(int64_t v);
    
    /**
     * \brief Sets the Value.
     * \param v [in] New Long value.
     * \return The calling instance.
     */
    Value & SetLong(int64_t v)
    {
        SetLongVoid(v);
        return *this;
    }
    
    /**
     * \brief Sets the Value.
     * \param v [in] New Double value.
     */
    void SetDoubleVoid(double64_t v);
    
    /**
     * \brief Sets the Value.
     * \param v [in] New Double value.
     * \return The calling instance.
     */
    Value & SetDouble(double64_t v)
    {
        SetDoubleVoid(v);
        return *this;
    }
    
    /**
     * \brief Sets the Value.
     * \param v [in] New Timestamp value.
     */
    void SetTimestampVoid(int64_t v);
    
    /**
     * \brief Sets the Value.
     * \param year [in] The year (>=1970).
     * \param month [in] The month ([1..12]).
     * \param day [in] The of the month ([1..31]).
     * \param hour [in] The hour ([0..23]).
     * \param minutes [in] The minutes ([0..59]).
     * \param seconds [in] The seconds ([0..59]).
     * \param millisecs [in] The milliseconds ([0..999]).
     */
    void SetTimestampVoid(int32_t year, int32_t month, int32_t day,
                          int32_t hour, int32_t minutes, int32_t seconds, int32_t millisecs);

    /**
     * \brief Sets the Value.
     * \param v [in] New Timestamp value.
     * \return The calling instance.
     */
    Value & SetTimestamp(int64_t v)
    {
        SetTimestampVoid(v);
        return *this;
    }
    
    /**
     * \brief Sets the Value.
     * \param year [in] The year (>=1970).
     * \param month [in] The month ([1..12]).
     * \param day [in] The of the month ([1..31]).
     * \param hour [in] The hour ([0..23]).
     * \param minutes [in] The minutes ([0..59]).
     * \param seconds [in] The seconds ([0..59]).
     * \param millisecs [in] The milliseconds ([0..999]).
     * \return The calling instance.
     */
    Value & SetTimestamp(int32_t year, int32_t month, int32_t day,
                         int32_t hour, int32_t minutes, int32_t seconds, int32_t millisecs)
    {
        SetTimestampVoid(year, month, day, hour, minutes, seconds, millisecs);
        return *this;
    }

    /**
     * \brief Sets the Value.
     * \param v [in] New String value.
     */
    void SetStringVoid(const std::wstring & v);
    
    /**
     * \brief Sets the Value.
     * \param v [in] New String value.
     * \return The calling instance.
     */
    Value & SetString(const std::wstring & v)
    {
        SetStringVoid(v);
        return *this;
    }
    
    /**
     * \brief Sets the OID Value.
     * \param v [in] New OID value.
     */
    void SetOIDVoid(oid_t v);
    
    /**
     * \brief Sets the Value.
     * \param v [in] New OID Value.
     * \return The calling instance.
     */
    Value & SetOID(oid_t v)
    {
        SetOIDVoid(v);
        return *this;
    }
    
    /**
     * \brief Sets the Value.
     * \param v [in] New value.
     */
    void SetVoid(Value & v)
    {
        SetVoid(*v.GetHandler());
    }
    
    /**
     * \brief Sets the Value.
     * \param v [in] New value.
     * \return The calling instance.
     */
    Value & Set(Value & v)
    {
        SetVoid(v);
        return *this;
    }
    
    /**
     * \brief Compares with the given Value.
     * 
     * It does not work for Text or if given Value objects does not 
     * have the same DataType.
     * \param v Given value to compare to.
     * \return 0 if this Value is equal to the given one; 
     * a value less than 0 if this Value is less than the given one;
     * and a value greater than 0 if this Value is greater than the given one.
     */
    int32_t Compare(const Value & v) const;
    
    /**
     * \brief Compares with the given Value.
     * 
     * It does not work for Text or if given Value objects does not 
     * have the same DataType.
     * \param v Given value to compare to.
     * \return TRUE if this Value is equal to the given one; 
     * FALSE otherwise.
     */
    bool_t Equals(const Value & v) const;
    
    /**
     * \brief Gets a string representation of the Value.
     *
     * It does not work for Text.
     * \param str String to be used. It is cleared and set with the string
     * representation of the Value.
     * \return The given string which has been updated.
     */
    std::wstring & ToString(std::wstring & str) const;
    

    /**
     * Maximum number of characters allowed for a String
     */
    static const int32_t MaxLengthString;

private:
    
    /**
     * \brief Sets the Value from a sparksee_core Value.
     * \param v [in] Sparkseecore Value.
     */
    void SetVoid(sparksee_core::Value & core_v);
    
    /**
     * \brief Creates a new instance from a sparksee_core Value.
     * \param core_v [in] Sparkseecore Value to be copied.
     */
    Value(const sparksee_core::Value & core_v);
};


END_SPARKSEE_GDB_NAMESPACE

/** @} */
#endif // _SPARKSEE_VALUE_H_
