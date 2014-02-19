/****************************************************************************
**
** Copyright (C) 2013 EPFL-LTS2
** Contact: Kirell Benzi (first.last@epfl.ch)
**
** This file is part of MLD.
**
**
** GNU General Public License Usage
** This file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.md included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements
** will be met: http://www.gnu.org/licenses/
**
****************************************************************************/

#ifndef MLD_LINK_H
#define MLD_LINK_H

#include <sparksee/gdb/common.h>
#include "mld/common.h"

namespace mld {

/**
 * @brief The Link abstract class, models an edge in the graph
 */
class MLD_API Link
{
public:
    virtual ~Link() = 0;

    sparksee::gdb::oid_t id() const { return m_id; }
    sparksee::gdb::oid_t source() const { return m_src; }
    sparksee::gdb::oid_t target() const { return m_tgt; }

protected:
    Link();
    Link( sparksee::gdb::oid_t src, sparksee::gdb::oid_t tgt );

    void setId( sparksee::gdb::oid_t id ) { m_id = id; }
    void setSource( sparksee::gdb::oid_t src ) { m_src = src; }
    void setTarget( sparksee::gdb::oid_t tgt ) { m_tgt = tgt; }

protected:
    sparksee::gdb::oid_t m_id;
    sparksee::gdb::oid_t m_src;
    sparksee::gdb::oid_t m_tgt;
};


/**
 * @brief The HLink concrete class, represent a H_LINK relationship
 * It links a SuperNode to another in the same Layer
 */
class MLD_API HLink : public Link
{
    friend class LinkDao;
    friend class MLGDao;
public:
    virtual ~HLink() override;

    double weight() const { return m_weight; }
    void setWeight( double v ) { m_weight = v; }

protected:
    HLink();
    HLink( sparksee::gdb::oid_t src, sparksee::gdb::oid_t tgt );
    HLink( sparksee::gdb::oid_t src, sparksee::gdb::oid_t tgt, double weight );

private:
    double m_weight;
};


/**
 * @brief The VLink concrete class, represent a V_LINK relationship
 * It links a SuperNode to another in a consecutive Layer (above or under the current layer)
 * There are 2 different implementation because HLINK are different
 * even if they look similar in the implementation
 */
class MLD_API VLink : public Link
{
    friend class LinkDao;
    friend class MLGDao;
public:
    virtual ~VLink() override;

    double weight() const { return m_weight; }
    void setWeight( double v ) { m_weight = v; }

private:
    VLink();
    VLink( sparksee::gdb::oid_t src, sparksee::gdb::oid_t tgt );
    VLink( sparksee::gdb::oid_t src, sparksee::gdb::oid_t tgt, double weight );

private:
    double m_weight;
};

} // end namespace mld

std::ostream& operator<<( std::ostream& out, const mld::HLink& hlink );
std::ostream& operator<<( std::ostream& out, const mld::VLink& vlink );


#endif // LINK_H
