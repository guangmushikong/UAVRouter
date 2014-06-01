//! \file
// \brief
// math class
//
// Revisions:
//   Date        Author     Description
//   ----------  --------   -------------------------------------------------
// - 2012-04-10  JiaoYi     Initial version

#ifndef niMath_H
#define niMath_H

#include <math.h>

#define ZEROF 1e-6
#define ZEROD 1e-12
#ifndef _PI_
#define _PI_ 3.1415926535897932384626433832795
#endif

/**
 * \brief math class
 *
 */
class niMath
{
public:

    //-----------------------------------------------------------------------------
    // FUNCTION _compare_
    //-----------------------------------------------------------------------------
    /**
    * compare 2 double values
    *
    * @return      -1:          _x < _y
    *              0:           _x equal _y
    *              1:           _x > _y
    */
    static inline int           _compare_(
        const double& _x,
        const double& _y)
    {
        if (_x == 0.0)
        {
            if (_y >= ZEROD)
                return -1;
            if (_y <= -ZEROD)
                return 1;
            return 0;
        }
        else if (_y == 0.0)
        {
            if (_x >= ZEROD)
                return 1;
            if (_x <= -ZEROD)
                return -1;
            return 0;
        }
        else
        {
            double q = _x/_y - 1.0;
            if (q >= ZEROD)
                return 1;
            if (q <= -ZEROD)
                return -1;
            return 0;
        }
    }

    //-----------------------------------------------------------------------------
    // FUNCTION _compare_
    //-----------------------------------------------------------------------------
    /**
    * compare 2 float values
    *
    * @return      -1:          _x < _y
    *              0:           _x equal _y
    *              1:           _x > _y
    */
    static inline int           _compare_(
        const float& _x,
        const float& _y)
    {
        if (_x == 0.0f)
        {
            if (_y >= ZEROF)
                return -1;
            if (_y <= -ZEROF)
                return 1;
            return 0;
        }
        else if (_y == 0.0f)
        {
            if (_x >= ZEROF)
                return 1;
            if (_x <= -ZEROF)
                return -1;
            return 0;
        }
        else
        {
            float q = _x/_y - 1.0f;
            if (q >= ZEROF)
                return 1;
            if (q <= -ZEROF)
                return -1;
            return 0;
        }
    }

    //-----------------------------------------------------------------------------
    // FUNCTION _in_range_
    //-----------------------------------------------------------------------------
    /**
    * Is _x in a range[_a, _b]
    *
    * @return      true:        in
    *              false:       not in
    */
    static inline bool          _in_range_(
        const double& _x,
        const double& _a,
        const double& _b)
    {
        if (_a > _b)
        {
            if (-1 == _compare_(_x, _b))
                return false;
            if (1 == _compare_(_x, _a))
                return false;
            return true;
        }
        else
        {
            if (-1 == _compare_(_x, _a))
                return false;
            if (1 == _compare_(_x, _b))
                return false;
            return true;
        }
    }

    //-----------------------------------------------------------------------------
    // FUNCTION _in_range_
    //-----------------------------------------------------------------------------
    /**
    * Is _x in a range[_a, _b]
    *
    * @return      true:        in
    *              false:       not in
    */
    static inline bool          _in_range_(
        const float& _x,
        const float& _a,
        const float& _b)
    {
        if (_a > _b)
        {
            if (-1 == _compare_(_x, _b))
                return false;
            if (1 == _compare_(_x, _a))
                return false;
            return true;
        }
        else
        {
            if (-1 == _compare_(_x, _a))
                return false;
            if (1 == _compare_(_x, _b))
                return false;
            return true;
        }
    }

    //-----------------------------------------------------------------------------
    // FUNCTION _is_overlap_
    //-----------------------------------------------------------------------------
    /**
    * Is _x zero
    *
    * @return       1:          overlap
    *               0:          connect
    *              -1:          separate
    */
    static inline int           _is_overlap_(
        const double& _a,
        const double& _b,
        const double& _c,
        const double& _d)
    {
        double range, length;
        if (_a < _b)
        {
            length = _b - _a;
            if (_c < _d)
            {
                //max = _b > _d ? _b : _d;
                //min = _a < _c ? _a : _c;
                range = (_b > _d ? _b : _d) - (_a < _c ? _a : _c);
                length += (_d - _c);
            }
            else
            {
                //max = _b > _c ? _b : _c;
                //min = _a < _d ? _a : _d;
                range = (_b > _c ? _b : _c) - (_a < _d ? _a : _d);
                length += (_c - _d);
            }
        }
        else
        {
            length = _a - _b;
            if (_c < _d)
            {
                //max = _a > _d ? _a : _d;
                //min = _b < _c ? _b : _c;
                range = (_a > _d ? _a : _d) - (_b < _c ? _b : _c);
                length += (_d - _c);
            }
            else
            {
                //max = _a > _c ? _a : _c;
                //min = _b < _d ? _b : _d;
                range = (_a > _c ? _a : _c) - (_b < _d ? _b : _d);
                length += (_c - _d);
            }
        }
        return _compare_(length, range);
    }

    //-----------------------------------------------------------------------------
    // FUNCTION _is_overlap_
    //-----------------------------------------------------------------------------
    /**
    * Is _x zero
    *
    * @return      -1:          overlap
    *               0:          connect
    *               1:          separate
    */
    static inline int           _is_overlap_(
        const float& _a,
        const float& _b,
        const float& _c,
        const float& _d)
    {
        float range, length;
        if (_a < _b)
        {
            length = _b - _a;
            if (_c < _d)
            {
                //max = _b > _d ? _b : _d;
                //min = _a < _c ? _a : _c;
                range = (_b > _d ? _b : _d) - (_a < _c ? _a : _c);
                length += (_d - _c);
            }
            else
            {
                //max = _b > _c ? _b : _c;
                //min = _a < _d ? _a : _d;
                range = (_b > _c ? _b : _c) - (_a < _d ? _a : _d);
                length += (_c - _d);
            }
        }
        else
        {
            length = _a - _b;
            if (_c < _d)
            {
                //max = _a > _d ? _a : _d;
                //min = _b < _c ? _b : _c;
                range = (_a > _d ? _a : _d) - (_b < _c ? _b : _c);
                length += (_d - _c);
            }
            else
            {
                //max = _a > _c ? _a : _c;
                //min = _b < _d ? _b : _d;
                range = (_a > _c ? _a : _c) - (_b < _d ? _b : _d);
                length += (_c - _d);
            }
        }
        return _compare_(length, range);
    }

    //-----------------------------------------------------------------------------
    // FUNCTION _is_zero_
    //-----------------------------------------------------------------------------
    /**
    * Is _x zero
    *
    * @return      true:        zero
    *              false:       not zero
    */
    static inline bool          _is_zero_(
        const double& _x)
    {
        if (_x >= ZEROD)
            return false;
        if (_x <= -ZEROD)
            return false;
        return true;
    }

    //-----------------------------------------------------------------------------
    // FUNCTION _is_zero_
    //-----------------------------------------------------------------------------
    /**
    * Is _x zero
    *
    * @return      true:        zero
    *              false:       not zero
    */
    static inline bool          _is_zero_(
        const float& _x)
    {
        if (_x >= ZEROF)
            return false;
        if (_x <= -ZEROF)
            return false;
        return true;
    }

};


#endif
