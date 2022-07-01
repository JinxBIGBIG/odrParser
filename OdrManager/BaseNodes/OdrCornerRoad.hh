/* ===================================================
 *  file:       OdrCornerRoad.hh
 * ---------------------------------------------------
 *  purpose:	CornerRoad information
 * ---------------------------------------------------
 *  first edit:	29.10.2006 by M. Dupuis @ VIRES GmbH
 * ===================================================
 */
#ifndef _OPENDRIVE_CORNER_ROAD_HH
#define _OPENDRIVE_CORNER_ROAD_HH

// ====== INCLUSIONS ======
#include "OdrNode.hh"

namespace OpenDrive
{

class CornerRoad : public Node
{
    public:
        /**
        * member variables according to specification
        */
        double  mS;
        double  mT;
        double  mDz;
        double  mHeight;
        int     mId;

    public:
        /**
        * constructor
        */
        explicit CornerRoad();

        /**
        * copy constructor
        *  @param  rhs source node which is to be copied
        */
        explicit CornerRoad( CornerRoad *rhs );

        /** 
        * Destroy the class. 
        */
        virtual ~CornerRoad();
        
        /**
        * print the data contents
        */
        virtual void printData() const;
        
       /**
        * read the contents of the node from a prepared attribute list
        * @param reader     the reader to be used for reading the data
        * @return failure/success
        */
        bool read( ReaderXML* reader );
        
        /**
        * get a copy of the node in base class type
        */
        virtual Node* getCopy( bool deep = false );

};
} // namespace OpenDRIVE

#endif /* _OPENDRIVE_CORNER_ROAD_HH */
