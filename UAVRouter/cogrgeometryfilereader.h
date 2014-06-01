#ifndef COGRGEOMETRYFILEREADER_H
#define COGRGEOMETRYFILEREADER_H

#include <ogrsf_frmts.h>
#include <memory>
#include <string>


class COGRGeometryFileReader
{
public:
    COGRGeometryFileReader();

public:
    static std::auto_ptr<OGRGeometry> GetFirstOGRGeometryFromFile(std::string ogrfile);
    static std::auto_ptr<OGRGeometry> GetFirstOGRPointFromFile(std::string ogrfile);
    static std::auto_ptr<OGRGeometry> GetFirstOGRLineStringFromFile(std::string ogrfile);
    static std::auto_ptr<OGRGeometry> GetFirstOGRPolygonFromFile(std::string ogrfile);
};

#endif // COGRGEOMETRYFILEREADER_H
