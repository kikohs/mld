#ifndef MLD_HEAVYEDGECOARSENER_H
#define MLD_HEAVYEDGECOARSENER_H

#include "mld/common.h"
#include "mld/operator/AbstractOperator.h"

namespace mld {

class MLD_API HeavyEdgeCoarsener: public AbstractOperator
{
public:
    HeavyEdgeCoarsener( dex::gdb::Graph* g );
    virtual ~HeavyEdgeCoarsener() override;

protected:
    virtual void pre_exec() override;
    virtual void exec() override;
    virtual void post_exec() override;

private:
};

} // end namespace mld

#endif // MLD_HEAVYEDGECOARSENER_H
