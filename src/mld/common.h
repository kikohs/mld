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

#ifndef MLD_COMMON_H
#define MLD_COMMON_H

// EXPORT API FOR DLL
#if( defined(_WIN32) || defined(_WIN64) )
#	ifdef MLD_NODLL
#		define MLD_API
#	else
#		ifdef MLD_API_EXPORTS
#			define MLD_API __declspec(dllexport)
#		else
#			define MLD_API __declspec(dllimport)
#		endif
#	endif
#else
#   if __GNUC__ >= 4 || __clang__
#       define MLD_API __attribute__ ((visibility("default")))
#   else
#       define MLD_API
#   endif
#endif

#define MLD_UNUSED(expr) do { (void)(expr); } while (0)

#include <assert.h>
#include "mld/utils/log.h"

namespace sparksee {
namespace gdb {
    class Objects;
    class ObjectsIterator;
    class AttributeList;
    class AttributeListIterator;
}}

namespace mld {

class DynamicGraph;

typedef std::shared_ptr<sparksee::gdb::Objects> ObjectsPtr;
typedef std::unique_ptr<sparksee::gdb::ObjectsIterator> ObjectsIt;
typedef std::unique_ptr<sparksee::gdb::AttributeList> AttributeListPtr;
typedef std::unique_ptr<sparksee::gdb::AttributeListIterator> AttributeListIt;

using DynGraphPtr = std::shared_ptr<DynamicGraph>;

static const double HLINK_DEF_VALUE = 1.0;
static const double VLINK_DEF_VALUE = 1.0;
static const double OLINK_DEF_VALUE = 0.0;
static const double CLINK_DEF_VALUE = 1.0;
static const double NODE_DEF_VALUE = 1.0;

static const int64_t INVALID_NODE_COUNT = -1;
static const int64_t INVALID_EDGE_COUNT = -1;

enum class TSDirection {
    PAST,
    FUTURE,
    BOTH
};

} // end namespace mld

#endif // MLD_COMMON_H
