#ifndef FIELD_H
#define FIELD_H

#include "defines.h"
#include "geo/geoobject.h"
#include "geo/geoprojection.h"
#include <vector>
#include "map.h"

class LISEM_API Field : public GeoObject
{

    std::vector<cTMap*> m_Maps;
    double m_ZStart = 0.0;
    double m_dz = 1.0;
public:


    inline Field()
    {



    }

    inline ~Field()
    {

        Clear();

    }

    inline Field(std::vector<Field*> maps, double z_start, double dz, bool copy = true)
    {

        SetFromMapList(maps,z_start,dz,copy);


    }

    inline void Clear()
    {
        for(int i = 0; i < m_Maps.size(); i++)
        {
            delete m_Maps.at(i);
        }
        m_Maps.clear();
        m_ZStart = 0.0;
        m_dz = 1.0;
    }

    inline void SetFromMapList(std::vector<cTMap*> maps, double z_start, double dz, bool copy = true)
    {

        if(copy)
        {
            for(int  i = 0; i < maps.size(); i++)
            {
                maps.at(i) = maps.at(i)->GetCopy();
            }
        }
        m_Maps = maps;
        m_ZStart = 0.0;
        m_dz = 1.0;

    }

    inline std::vector<cTMap*> GetMapList()
    {
        return m_Maps;
    }




    /*QString    AS_FileName          = "";
    bool           AS_writeonassign     = false;

    Field *        Assign            (Field *other);
    Field *        Assign            (float other);

    Field *        OpAdd             (Field *other);
    Field *        OpMul             (Field *other);

    Field *        OpNeg             ();
    Field *        OpSub             (Field *other);
    Field *        OpDiv             (Field *other);
    Field *        OpPow             (Field *other);
    Field *        OpAddAssign       (Field *other);
    Field *        OpSubAssign       (Field *other);
    Field *        OpMulAssign       (Field *other);
    Field *        OpDivAssign       (Field *other);
    Field *        OpPowAssign       (Field *other);

    Field *        OpAdd             (float other);
    Field *        OpMul             (float other);

    Field *        OpSub             (float other);
    Field *        OpDiv             (float other);
    Field *        OpPow             (float other);
    Field *        OpAddAssign       (float other);
    Field *        OpSubAssign       (float other);
    Field *        OpMulAssign       (float other);
    Field *        OpDivAssign       (float other);
    Field *        OpPowAssign       (float other);

    Field *        OpModAssign       (Field *other);
    Field *        OpModAssign       (float other);
    Field *        OpMod             (Field *other);
    Field *        OpMod             (float other);

    Field *        EqualTo           (Field *other);
    Field *        LargerThen        (Field *other);
    Field *        SmallerThen       (Field *other);
    Field *        NotEqualTo        (Field *other);
    Field *        LargerEqualThen   (Field *other);
    Field *        SmallerEqualThen  (Field *other);

    Field *        EqualTo           (float other);
    Field *        LargerThen        (float other);
    Field *        SmallerThen       (float other);
    Field *        NotEqualTo        (float other);
    Field *        LargerEqualThen   (float other);
    Field *        SmallerEqualThen  (float other);

    Field *        And               (Field *other);
    Field *        Or                (Field *other);
    Field *        Xor               (Field *other);

    Field *        And               (float other);
    Field *        Or                (float other);
    Field *        Xor               (float other);

    Field *        AndAssign         (Field *other);
    Field *        OrAssign          (Field *other);
    Field *        XorAssign         (Field *other);

    Field *        AndAssign         (float other);
    Field *        OrAssign          (float other);
    Field *        XorAssign         (float other);

    Field *        Negate            ();

    Field *        OpMod_r           (float other);
    Field *        OpAdd_r           (float other);
    Field *        OpMul_r           (float other);
    Field *        OpSub_r           (float other);
    Field *        OpDiv_r           (float other);
    Field *        OpPow_r           (float other);
    Field *        LargerThen_r      (float other);
    Field *        SmallerThen_r     (float other);
    Field *        NotEqualTo_r      (float other);
    Field *        LargerEqualThen_r (float other);
    Field *        SmallerEqualThen_r(float other);
    Field *        EqualTo_r         (float other);
    Field *        And_r             (float other);
    Field *        Or_r              (float other);
    Field *        Xor_r             (float other);/*



};


#endif // FIELD_H
