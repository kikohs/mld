
#include <iostream>
#include <dex/gdb/Graph.h>
#include <dex/gdb/Objects.h>
#include <dex/gdb/ObjectsIterator.h>

#include <mld/common.h>
#include <mld/DexManager.h>

using namespace dex::gdb;
using namespace mld;

int main(int argc, char *argv[])
{
    //
    // Create a sample database
    mld::DexManager dexManager(mld::kRESOURCES_DIR + L"mydex.cfg");
    dexManager.createDatabase(mld::kRESOURCES_DIR + L"HelloDex.dex", L"HelloDex");
    SessionPtr sess = dexManager.newSession();
    Graph* g = sess->GetGraph();


    //
    // SCHEMA
    //

    // Add a node type for the movies, with a unique identifier and two indexed attributes
    type_t movieType = g->NewNodeType(L"MOVIE");
    attr_t movieIdType = g->NewAttribute(movieType, L"ID", Long, Unique);
    attr_t movieTitleType = g->NewAttribute(movieType, L"TITLE", String, Indexed);
    attr_t movieYearType = g->NewAttribute(movieType, L"YEAR", Integer, Indexed);

    // Add a node type for the people, with a unique identifier and an indexed attribute
    type_t peopleType = g->NewNodeType(L"PEOPLE");
    attr_t peopleIdType = g->NewAttribute(peopleType, L"ID", Long, Unique);
    attr_t peopleNameType = g->NewAttribute(peopleType, L"NAME", String, Indexed);

    // Add an undirected edge type with an attribute for the cast of a movie
    type_t castType = g->NewEdgeType(L"CAST", false, false);
    attr_t castCharacterType = g->NewAttribute(castType, L"CHARACTER", String, Basic);

    // Add a directed edge type restricted to go from people to movie for the director of a movie
    type_t directsType = g->NewRestrictedEdgeType(L"DIRECTS", peopleType, movieType, false);


    //
    // DATA
    //

    // Add some MOVIE nodes
    Value *value = new Value();

    oid_t mLostInTranslation = g->NewNode(movieType);
    g->SetAttribute(mLostInTranslation, movieIdType, value->SetLong(1));
    g->SetAttribute(mLostInTranslation, movieTitleType, value->SetString(L"Lost in Translation"));
    g->SetAttribute(mLostInTranslation, movieYearType, value->SetInteger(2003));

    oid_t mVickyCB = g->NewNode(movieType);
    g->SetAttribute(mVickyCB, movieIdType, value->SetLong(2));
    g->SetAttribute(mVickyCB, movieTitleType, value->SetString(L"Vicky Cristina Barcelona"));
    g->SetAttribute(mVickyCB, movieYearType, value->SetInteger(2008));

    oid_t mManhattan = g->NewNode(movieType);
    g->SetAttribute(mManhattan, movieIdType, value->SetLong(3));
    g->SetAttribute(mManhattan, movieTitleType, value->SetString(L"Manhattan"));
    g->SetAttribute(mManhattan, movieYearType, value->SetInteger(1979));


    // Add some PEOPLE nodes
    oid_t pScarlett = g->NewNode(peopleType);
    g->SetAttribute(pScarlett, peopleIdType, value->SetLong(1));
    g->SetAttribute(pScarlett, peopleNameType, value->SetString(L"Scarlett Johansson"));

    oid_t pBill = g->NewNode(peopleType);
    g->SetAttribute(pBill, peopleIdType, value->SetLong(2));
    g->SetAttribute(pBill, peopleNameType, value->SetString(L"Bill Murray"));

    oid_t pSofia = g->NewNode(peopleType);
    g->SetAttribute(pSofia, peopleIdType, value->SetLong(3));
    g->SetAttribute(pSofia, peopleNameType, value->SetString(L"Sofia Coppola"));

    oid_t pWoody = g->NewNode(peopleType);
    g->SetAttribute(pWoody, peopleIdType, value->SetLong(4));
    g->SetAttribute(pWoody, peopleNameType, value->SetString(L"Woody Allen"));

    oid_t pPenelope = g->NewNode(peopleType);
    g->SetAttribute(pPenelope, peopleIdType, value->SetLong(5));
    g->SetAttribute(pPenelope, peopleNameType, value->SetString(L"PenÃ©lope Cruz"));

    oid_t pDiane = g->NewNode(peopleType);
    g->SetAttribute(pDiane, peopleIdType, value->SetLong(6));
    g->SetAttribute(pDiane, peopleNameType, value->SetString(L"Diane Keaton"));



    // Add some CAST edges
    oid_t anEdge;
    anEdge = g->NewEdge(castType, mLostInTranslation, pScarlett);
    g->SetAttribute(anEdge, castCharacterType, value->SetString(L"Charlotte"));

    anEdge = g->NewEdge(castType, mLostInTranslation, pBill);
    g->SetAttribute(anEdge, castCharacterType, value->SetString(L"Bob Harris"));

    anEdge = g->NewEdge(castType, mVickyCB, pScarlett);
    g->SetAttribute(anEdge, castCharacterType, value->SetString(L"Cristina"));

    anEdge = g->NewEdge(castType, mVickyCB, pPenelope);
    g->SetAttribute(anEdge, castCharacterType, value->SetString(L"Maria Elena"));

    anEdge = g->NewEdge(castType, mManhattan, pDiane);
    g->SetAttribute(anEdge, castCharacterType, value->SetString(L"Mary"));

    anEdge = g->NewEdge(castType, mManhattan, pWoody);
    g->SetAttribute(anEdge, castCharacterType, value->SetString(L"Isaac"));



    // Add some DIRECTS edges
    anEdge = g->NewEdge(directsType, pSofia, mLostInTranslation);

    anEdge = g->NewEdge(directsType, pWoody, mVickyCB);

    anEdge = g->NewEdge(directsType, pWoody, mManhattan);



    //
    // QUERIES
    //

    // Get the movies directed by Woody Allen
    Objects *directedByWoody = g->Neighbors(pWoody, directsType, Outgoing);

    // Get the cast of the movies directed by Woody Allen
    Objects *castDirectedByWoody = g->Neighbors(directedByWoody, castType, Any);

    // We don't need the directedByWoody collection anymore, so we should delete it
    delete directedByWoody;


    // Get the movies directed by Sofia Coppola
    Objects *directedBySofia = g->Neighbors(pSofia, directsType, Outgoing);

    // Get the cast of the movies directed by Sofia Coppola
    Objects *castDirectedBySofia = g->Neighbors(directedBySofia, castType, Any);

    // We don't need the directedBySofia collection anymore, so we should delete it
    delete directedBySofia;


    // We want to know the people that acted in movies directed by Woody AND in movies directed by Sofia.
    Objects *castFromBoth = Objects::CombineIntersection(castDirectedByWoody, castDirectedBySofia);

    // We don't need the other collections anymore
    delete castDirectedByWoody;
    delete castDirectedBySofia;

    // Say hello to the people found
    ObjectsIterator *it = castFromBoth->Iterator();
    while (it->HasNext())
    {
        oid_t peopleOid = it->Next();
        g->GetAttribute(peopleOid, peopleNameType, *value);
        std::wcout << L"Hello " << value->GetString() << std::endl;
    }
    // The ObjectsIterator must be deleted
    delete it;

    // The Objects must be deleted
    delete castFromBoth;

    // The value is not needed anymore
    delete value;


    //
    // Close the database
    //
    sess.reset();

    return 0;
}
