#ifndef GOMOPHOTOGRAMETRY_H
#define GOMOPHOTOGRAMETRY_H


namespace Gomo{

    namespace PhotoCamera {

        //相机信息，包括主距，主点坐标，像点大小，都按照数字相机处理，主点坐标为主点的像素坐标
        struct DigitalCameraInfo
        {
        public:
            double x0,y0;					//主点坐标为主点的像素坐标
            double f;						//focus length, mm
            double pixelsize;               //像素（象元）大小，mm
            long   height;                  //height, in pixels
            long   width;                   //width , in pixels

        public:

            inline DigitalCameraInfo &
                operator=(const DigitalCameraInfo & rs)
                {
                    x0=rs.x0;
                    y0=rs.y0;
                    f=rs.f;
                    pixelsize=rs.pixelsize;
                    height=rs.height;
                    width=rs.width;

                    return *this;
                };

        };



    }

}

#endif // GOMOPHOTOGRAMETRY_H
