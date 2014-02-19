/*
 * SPARKSEE: a library for out-of-core graph-based data storage and query processing.
 *
 * See the file LICENSE.txt for redistribution information.
 *
 * Copyright (c) 2005-2010 Sparsity Technologies, Barcelona (Spain)
 */

#ifndef _SPARKSEE_OBJECTS_H_
#define _SPARKSEE_OBJECTS_H_

/**
 * \addtogroup gdb
 * @{
 *
 * \file Objects.h
 * \brief It contains the definition of Object class.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */

#include "gdb/Handler.h"
#include <list>

/*
 * Forward declaration of sparksee_core classes.
 */
namespace sparksee_core
{
    class Objects;
}

BEGIN_SPARKSEE_GDB_NAMESPACE

/*
 * Forward declaration of sparksee classes.
 */
class Session;
class ObjectsIterator;
class Graph;


/**
 * \brief Object identifier set class.
 *
 * It stores a collection of Sparksee object identifiers as a set. As a set,
 * there is no order and no duplicated elements.
 *
 * This class should be used just to store large collections. Otherwise,
 * it is strongly recommended to use common classes from the language API.
 *
 * This class is not thread-safe.
 *
 * ObjectsIterator must be used to traverse all the elements into the set.
 *
 * When the Objects instance is closed, it closes all existing and non-closed
 * ObjectsIterator instances too.
 *
 * \author Sparsity Technologies http://www.sparsity-technologies.com
 */
class SPARKSEE_EXPORT Objects
: private Handler<sparksee_core::Objects>
{
    friend class Session; // to access private constructor
    friend class Graph; // to access private constructor
    friend class ObjectsIterator; // to unregister
    
public:
    
    /**
     * \brief Invalid OID constant.
     */
    static const oid_t InvalidOID;
    
    /**
     * \brief Destructor.
     */
    virtual ~Objects();

    /**
     * \brief Creates a new Objects instance as a copy of the given one.
     * \return The new Objects instance.
     */
    Objects * Copy();
    
    /**
     * \brief Gets the number of elements into the collection.
     * \return The number of elements into the collection.
     */
    int64_t Count();
    
    /**
     * \brief Adds an element into the collection. 
     * \param e [in] Element to be added.
     * \return TRUE if the element is added, FALSE if the element
     * was already into the collection.
     */
    bool_t Add(oid_t e);
    
    /**
     * \brief Gets if the given element exists into the collection.
     * \param e [in] Element.
     * \return TRUE if the element exists into the collection, FALSE
     * otherwise.
     */
    bool_t Exists(oid_t e);
    
    /**
     * \brief Gets an element from the collection.
     * \return Any element from the collection.
     * \exception NoSuchElementException whether the collection is empty.
     */
    oid_t Any() throw(sparksee::gdb::NoSuchElementException, sparksee::gdb::Error);
    
    /**
     * \brief Removes an element from the collection.
     * \param e [in] Element to be removed.
     * \return TRUE if the element is removed, FALSE if the element
     * was not into the collection.
     */
    bool_t Remove(oid_t e);
    
    /**
     * \brief Clears the collection removing all its elements.
     */
    void Clear();

    /**
     * \brief Performs the union operation.
     *
     * This adds all existing elements of the given Objects instance 
     * to the Objects calling instance 
     * \param objs [in] Objects instance.
     * \return Number of elements into the collection once the operation
     * has been executed.
     */
    int64_t Union(Objects * objs);
    
    /**
     * \brief Performs the intersection operation.
     *
     * Updates the Objects calling instance setting those existing 
     * elements at both two collections and removing all others.
     * \param objs [in] Objects instance.
     * \return Number of elements into the collection once the operation
     * has been executed.
     */
    int64_t Intersection(Objects * objs);
    
    /**
     * \brief Performs the difference operation.
     *
     * This updates the Objects calling instance removing those existing 
     * elements at the given Objects instance.
     * \param objs [in] Objects instance.
     * \return Number of elements into the collection once the operation
     * has been executed.
     */
    int64_t Difference(Objects * objs);


    /**
     * \brief Checks if the given Objects contains the same information.
     *
     * \param objs [in] Objects instance.
     * \return True if the objects are equal or false otherwise.
     */
    bool_t Equals(Objects * objs);

    /**
     * Check if this objects contains the other one.
     * <p>
     * @param objs Objects collection.
     * @return True if it contains the given object.
     */
    bool_t Contains(Objects* objs);

    /**
     * \brief Creates a new Objects instance which is the union of
     * the two given.
     *
     * Two given Objects belong to the same Session.
     * \param objs1 [in] Objects instance.
     * \param objs2 [in] Objects instance.
     * \return New Objects instance.
     */
    static Objects * CombineUnion(Objects * objs1, Objects * objs2);
    
    /**
     * \brief Creates a new Objects instance which is the intersection of
     * the two given.
     *
     * Two given Objects belong to the same Session.
     * \param objs1 [in] Objects instance.
     * \param objs2 [in] Objects instance.
     * \return New Objects instance.
     */
    static Objects * CombineIntersection(Objects * objs1, Objects * objs2);
    
    /**
     * \brief Creates a new Objects instance which is the difference of
     * the two given.
     *
     * Two given Objects belong to the same Session.
     * \param objs1 [in] Objects instance.
     * \param objs2 [in] Objects instance.
     * \return New Objects instance.
     */
    static Objects * CombineDifference(Objects * objs1, Objects * objs2);
    
    /**
     * \brief Performs the copy operation.
     *
     * This updates the Objects calling instance and copies 
     * the given Objects instance.
     * \param objs [in] Objects instance.
     * \return Number of elements into the collection once the operation
     * has been executed.
     */
    int64_t Copy(Objects * objs);
    
    /**
     * \brief Creates a new Objects instance which is a sample of the
     * calling one.
     * \param exclude [in] If not NULL, elements into this collection 
     * will be excluded from the resulting one.
     * \param samples [in] Number of elements into the resulting collection.
     * \return Sample collection.
     */
    Objects * Sample(Objects * exclude, int64_t samples);
    
    /**
     * \brief Gets an ObjectsIterator.
     * \return ObjectsIterator instance.
     */
    ObjectsIterator * Iterator();
    
    /**
     * \brief Gets an ObjectsIterator skipping index elements.
     *
     * Objects collection has no order, so this method is 
     * implementation-dependent.
     * \param index [in] The number of elements to skip from the beginning.
     * It must be in the range [0..Size).
     * \return ObjectsIterator instance.
     */
    ObjectsIterator * IteratorFromIndex(int64_t index);
    
    /**
     * \brief Gets an ObjectsIterator starting from the given element.
     *
     * Objects collection has no order, so this method is 
     * implementation-dependent.
     * \param e [in] The first element to traverse in the resulting
     * ObjectsIterator instance.
     * \return ObjectsIterator instance.
     */
    ObjectsIterator * IteratorFromElement(oid_t e);
    
private:
    
    /**
     * \brief Creates a new instance.
     * \param core_objs [in] Handled sparksee_core Objects reference.
     * \param s [in] Parent Session reference.
     */
    Objects(sparksee_core::Objects * core_objs, Session * s);
    
    /**
     * \brief Non implemented to avoid use of copy constructor.
     */
    Objects(const Objects & objs);

    /**
     * \brief Non implemented to avoid use of assignment operator.
     */
    Objects & operator =(const Objects & objs);
    
    /**
     * \brief Parent Session reference.
     */
    Session *sess;

    /**
     * \brief List of open iterators.
     */
    std::list<ObjectsIterator*> m_iterators;

    /**
     * \brief Unregister the iterator.
     * It's called when an ObjectsIterator is deleted.
     */
    void UnregisterIterator( ObjectsIterator *it );

    /**
     * \brief Invalidates all the open iterators.
     */
    void InvalidateIterators();
};


END_SPARKSEE_GDB_NAMESPACE

/** @} */
#endif // _SPARKSEE_OBJECTS_H_
