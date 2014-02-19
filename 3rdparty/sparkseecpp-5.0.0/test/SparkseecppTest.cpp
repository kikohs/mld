#include <stdio.h>
#include "gdb/Sparksee.h"
#include "gdb/Database.h"
#include "gdb/Session.h"
#include "gdb/Graph.h"

using namespace sparksee::gdb;

int main(int argc, char *argv[])
{
    SparkseeConfig cfg;
    Sparksee * sparksee = new Sparksee(cfg);
    Database * db = sparksee->Create(L"./graphdb.gdb", L"GraphDB");
    Session * sess = db->NewSession();
    Graph * graph = sess->GetGraph();
    type_t tNode = graph->NewNodeType(L"NodeTypeA");
    graph->DumpData(L"graphdb.data");
    graph->DumpStorage(L"graphdb.storage");
    delete graph;
    delete sess;
    delete db;
    delete sparksee;

    printf("Hello World Sparksee!\n");
    return 0;
}

